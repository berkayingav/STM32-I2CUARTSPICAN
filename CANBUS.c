#include "canbus.h"

// This settings applied then default GPIO pins so rx PA11 and tx PA12. Please use then.

void CAN_Config(enum pclk p,enum bitrate btr)
{
	RCC->APB1ENR |= (1<<25);
	RCC->APB2ENR |= (1<<0);
	//Default PA11 -> CANRX ,PA12 -> CANTX
	RCC->APB2ENR |=  (1<<2);
	//CAN1 rx -> input floating  
	//CAN1 tx -> alternate function push pull
	GPIOA->CRH |= (0x4<<12);
	GPIOA->CRH |= (0xB<<16);
	CAN1->MCR |= (1<<0);
	CAN1->MCR |= (1 << 4); 
	CAN1->MCR |= (1<<6);
	
	CAN1->BTR &= ~((0x07) << 20);
	CAN1->BTR &= ~((0x0F) << 16);
	CAN1->BTR &= ~((0x3FF) << 0);

  CAN1->BTR |=  (((bitRatesSet[p].btrValues[btr].ts2 ) & 0x07) << 20);
	CAN1->BTR |=  (((bitRatesSet[p].btrValues[btr].ts1 ) & 0x0F) << 16);
	CAN1->BTR |=  (((bitRatesSet[p].btrValues[btr].brp ) & 0x3FF) << 0);
	
	CAN1->FMR |= (1<<0);
	CAN1->FM1R = 0x0000;       // All of banks is mask mode
	CAN1->FS1R = 0x3FFF;        // 32 bit mode
	CAN1->FFA1R = 0x0000;
	CAN1->FA1R = 0x3FFF;
	
	for (uint8_t i = 0; i < 14; i++) {
    CAN1->sFilterRegister[i].FR1 = 0x00000000;  // Filter ID = 0
    CAN1->sFilterRegister[i].FR2 = 0x00000000;  // Filter MASK = 0 
	}
	
	CAN1->FMR &= ~(1 << 0);
	CAN1->MCR   &= ~(1<<0); 
}


void SetFilter(uint8_t bank,uint16_t id,uint16_t mask)
{
		CAN1->FMR = (1<<0);
		CAN1->sFilterRegister[bank].FR1 = (id<<21);
		CAN1->sFilterRegister[bank].FR2 = (mask<<21);
	  CAN1->FMR &= ~(1 << 0); // Exit filter init mode
}

void setFiltersFIFO(uint8_t filterNumber,enum fifonumber fn)
{
	CAN1->FMR = (1<<0);
	CAN1->FFA1R &= ~(1 << filterNumber);            
  CAN1->FFA1R |= ((fn & 0x01) << filterNumber);
	CAN1->FMR &= ~(1 << 0);
}


int CAN_Send_Data(uint16_t id,uint8_t *data,uint8_t length)
{
	uint8_t mailbox = 0;
	
	while(!((CAN1->TSR & CAN_TSR_TME0) | (CAN1->TSR & CAN_TSR_TME1) | (CAN1->TSR & CAN_TSR_TME2))){};
	if (CAN1->TSR & CAN_TSR_TME0)
			mailbox = 0;
	else if (CAN1->TSR & CAN_TSR_TME1)
			mailbox = 1;
	else if (CAN1->TSR & CAN_TSR_TME2)
			mailbox = 2;
				
	CAN1->sTxMailBox[mailbox].TIR = (id<<21);
	CAN1->sTxMailBox[mailbox].TDTR &= ~(0xF << 0); 
	if (length > 8) length = 8;
	CAN1->sTxMailBox[mailbox].TDTR |= ((length & 0x0F) << 0);
	CAN1->sTxMailBox[mailbox].TDLR = 0;
	CAN1->sTxMailBox[mailbox].TDHR = 0;
	for (int i = 0; i < length; ++i)
  {
     if (i < 4)
        CAN1->sTxMailBox[mailbox].TDLR |= ((uint32_t)data[i] << (8 * i));
     else
        CAN1->sTxMailBox[mailbox].TDHR |= ((uint32_t)data[i] << (8 * (i - 4)));
  }
	 CAN1->sTxMailBox[mailbox].TIR |= (1<<0);
	
	uint32_t timeout = 1000000;

	while ((CAN1->sTxMailBox[mailbox].TIR & (1 << 0)) && --timeout);

	if (timeout == 0)
			return -1;
	else
			return 0;
}

int CAN_Receive_Data(uint16_t *id,uint8_t *data,uint8_t *length)
{
	uint8_t fifo = 0;
	
	if ((CAN1->RF0R & 0x03) != 0)
      fifo = 0;
  else if ((CAN1->RF1R & 0x03) != 0)
      fifo = 1;
	else
			return -1;
	
	*id = (CAN1->sFIFOMailBox[fifo].RIR >> 21) & 0x7FF;
	*length = (CAN1->sFIFOMailBox[fifo].RDTR >> 0) & 0x0F;
		
	for (int i = 0; i < *length; ++i)
  {
     if (i < 4)
				data[i] = (CAN1->sFIFOMailBox[fifo].RDLR >> 8*i) & 0xFF;
     else
				data[i] = (CAN1->sFIFOMailBox[fifo].RDHR >> 8*(i-4)) & 0xFF;
  }

	 if (fifo == 0)
        CAN1->RF0R |= (1 << 5); // RFOM0
   else
        CAN1->RF1R |= (1 << 5); // RFOM1
	
	 return 0;
}




