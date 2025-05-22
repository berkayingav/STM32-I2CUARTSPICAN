#include "UartModBus.h"

/*  
		GPIOA 9 and GPIOA 10  GPIOA 8 -> DE/RE pin for half dublex
		Use RS485 Module
		if pin 8 is high transmitter is enable. Else receiver is enable 
*/

void Uart1_Config (void)
{
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 	// RCC->APB2ENR |= (1<<2);
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // RCC->APB2ENR |= (1<<14);
	
	GPIOA->CRH &= ~(0xFFF << 0);
	GPIOA->CRH |= (9<<4) | (4<<8);
	GPIOA->CRH |= (1<<0);
	
	USART1->CR1 &= ~((1 << 12) | (1 << 13));  
	USART1->CR1 |= (1<<13);   // UE bit
	USART1->CR1 &= ~(1<<12);  // M bit
	
	//Baudrate formula    tx/Rx baud = fck / (16*usartdiv);
	// for 115200     72*10^6 / (16*115200)  = 39,0625
	// fraction = 16*0.0625 = 1
	//Mantissa 39
	USART1->BRR = (1<<0) | (39<<4);
	
	USART1->CR1 |= (1<<3); // TE bit 
	USART1->CR1 |= (1<<2); // RE bit
	USART1->CR1 |= (1<<4);  //  IDLE bit
	USART1->CR1 |= (1<<6);  // TCIE
	NVIC_EnableIRQ(USART1_IRQn);
}


void uart1_Receive_IT_Callback(Uart_Callback uartCall)
{
	uart_rx_cb = uartCall;
}

void uart1_Transmit(uint8_t *data,uint16_t size,uint32_t timeout){
	TIM6->CNT = 0;
	
	for(uint16_t i = 0; i < size; i++) {
		
		if(TIM6->CNT > timeout){
			break;
		}
		
		while (!(USART1->SR & (1 << 7)));
		USART1->DR = data[i] & 0xFF;
	}
	while(!(USART1->SR & (1 << 6)));
	
}

void uart1_Transmit_IT(uint8_t *data,uint16_t size)
{
	buffer = data;
	bufSize = size;
	txIndex = 0;
	USART1->CR1 |= (1<<7);
	
}


void uart1_Receive(uint8_t *data,uint16_t size,uint32_t timeout)
{
	TIM6->CNT = 0;

	for(uint16_t i = 0; i < size; i++){
		
		if(TIM6->CNT > timeout){
			break;
		}
		
		while(!(USART1->SR & (1<<5))){
			 if (TIM6->CNT > timeout)
         {
          break; // timeout esnasinda da çik
         }
		}
		
			data[i] = USART1->DR;
		}

}

void uart1_Receive_IT(uint8_t *data,uint16_t size)
{
			
		rxBuffer = data;
		rxbufSize = size;
		rxIndex = 0;
		USART1->CR1 |= (1<<5);
	  USART1->CR1 |= (1<<4);

}

void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<7))
	{
		if(txIndex < bufSize)
		{
			USART1->DR = buffer[txIndex++];
		}
		else
		{
			USART1->CR1 &= ~(1 << 7);
			USART1->CR1 |= (1<<6);
		}
		
	}

	if (USART1->SR & (1 << 6))
    {
       USART1->CR1 &= ~(1 << 6);  
    }
		
	if(USART1->SR & (1<<5))
	{
		if(rxIndex < rxbufSize)
		{
			rxBuffer[rxIndex++] = USART1->DR;
		}
		else
		{
			USART1->CR1 &= ~(1 << 5);
		}
		
	}
	
	if(USART1->SR & (1<<4)){
		volatile uint32_t tmp;
		tmp = USART1->SR;
		tmp = USART1->DR;
		USART1->CR1 &= ~(1 << 5);
		
		if(uart_rx_cb && rxIndex>0){
			uart_rx_cb(rxBuffer,rxIndex);
		}
			rxIndex = 0;
	}
}

//SLAVE ADD - FUNCTION CODE - REG ADD HIGH - REG ADD LOW - No of Pts HIGH - No Of Pts Low - CRC Low - Crc High

void sendData(uint8_t *data,uint8_t len)
{
	//if pin 8 is high transmitter is enable. Else receiver is enable 
  // 0-15 BSRR
	GPIOA->BSRR |= (1<<8);   //RS485  RE/DE pin GPIOA 8
	uart1_Transmit(data,len,1000);
	GPIOA->BSRR |= (1<<24);
}

uint16_t calculate_modbus_crc(const uint8_t *data, int length) {
    uint16_t crc = 0xFFFF; // Initial value for Modbus CRC

    for (int i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    // Swap bytes (Modbus uses little-endian CRC)
    //crc = ((crc & 0xFF) << 8) | ((crc >> 8) & 0xFF);

    return crc;
}
//
//
uart1_Receive_IT(uint8_t rxData,32);

//For Read
void modbusDataRead(uint8_t slaveAddr,enum functionCodes fc,uint16_t regAddr,uint8_t regPts)
{		
	uint32_t sbt = 0;
	
	switch(fc){
		case rCoils:
			sbt = 1;
			break;
		case rDiscInputs:
			sbt = 10001;
			break;
		case rMHRegisters:
			sbt =40001;
			break;
		case rIRegisters:
			sbt = 30001;
			break;
		default:
			sbt = 0;
			break;
	}
		
	uint8_t txData[8];
	txData[0] = slaveAddr;
	txData[1] = fc;
	txData[2] = ((regAddr - sbt) >> 8) & 0xFF;  //MSB
	txData[3] = (regAddr - sbt) & 0xFF;   //LSB
	txData[4] = (regPts >> 8) & 0xFF;
	txData[5] = (regPts)& 0xFF; 

	uint16_t CRC = calculate_modbus_crc(txData,6);
	txData[6] = CRC & 0xFF;
	txData[7] = (CRC >> 8) & 0xFF;
	sendData(txData,8);
}

//if use multiple registers write you can use 0 for coilSize
void modbusDataWrite(uint8_t slaveAddr,enum functionCodes fc,uint16_t regAddr,uint8_t *data,uint8_t dataSize,uint8_t coilSize)
{
	if(dataSize == 1)
	{
		uint8_t txData[8];
		txData[0] = slaveAddr;
		txData[1] = fc;
		txData[2] = ((regAddr) >> 8) & 0xFF;  //MSB
		txData[3] = (regAddr) & 0xFF;   //LSB
	  if (fc == 0x05) { 
         txData[4] = (data[0]) ? 0xFF : 0x00;
         txData[5] = 0x00;
    } else { // 
         txData[4] = 0x00;
         txData[5] = data[0];
        }

		uint16_t CRC = calculate_modbus_crc(txData,6);
		txData[6] = CRC & 0xFF;
		txData[7] = (CRC >> 8) & 0xFF;
		sendData(txData,8);	
	}
	else if(dataSize > 1)
	{
		if (fc == 0x0F){
			uint8_t txData[(coilSize + 7)/8 + 9];
			txData[0] = slaveAddr;
			txData[1] = fc;
			txData[2] = ((regAddr) >> 8) & 0xFF;
			txData[3] = (regAddr) & 0xFF;
			txData[4] = 0;
			txData[5] = coilSize;
			txData[6] = (coilSize + 7)/8;
			for(uint8_t i = 0;i<(txData[6]);i++){
			txData[i + 7] = data[i];
			}
			uint16_t CRC = calculate_modbus_crc(txData,(txData[6]+7));
			txData[txData[6]+7] =  CRC & 0xFF;
			txData[txData[6]+8] = (CRC >> 8) & 0xFF;
			uint8_t lenn = (coilSize + 7)/8 + 9;
			sendData(txData,lenn);	
		}
		else{
			uint8_t txData[(dataSize*2)+9];
			txData[0] = slaveAddr;
			txData[1] = fc;
			txData[2] = ((regAddr) >> 8) & 0xFF;
			txData[3] = (regAddr) & 0xFF;
			txData[4] = 0;
			txData[5] = dataSize;
			txData[6] = dataSize*2;
			uint8_t index =7;
			for(uint8_t i = 0;i<dataSize;i++){
			txData[index++] = (data[i] >> 8) & 0xFF;
			txData[index++] = (data[i]) & 0xFF;
			}
			uint16_t CRC = calculate_modbus_crc(txData,index);
			txData[index++] = CRC & 0xFF;
			txData[index++] = (CRC >> 8) & 0xFF;
			sendData(txData,index);	
		}

	}
	
}

void ModBusRxCallback(uint8_t *data,uint16_t len)
{
	uart1_Receive_IT(rxData,sizeof(rxData));
}

int main(void)
{
	uint8_t rxData[64]; 
	SysClockConfig();
	TIM6Config();
	Uart1_Config();
	uart1_Receive_IT_Callback(ModBusRxCallback);
	uart1_Receive_IT(rxData,sizeof(rxData));
	
	while(1)
	{
		modbusDataRead(0x01,rMHRegisters,40001,2);
		delay_ms(100);
	}
}