#include "spi_protocol.h"

/*

	THE CSS PINS IS GPIOA 8,9,10 and 11
	
	when slave_1 so 1 slave selected you have to use them GPIOA 8.
	Also slave_2 selected you have to use them GPIOA 8 and 9. 
	Therefore if slave_3 selected you have to use them GPIOA 8,9,10 and
	if slave_4 selected you have to use them GPIOA 8,9,10 and 11


*/

void SPI_Config(void)
{
	RCC->APB2ENR |= (1<<12);
	SPI1->CR1 |= (1<<0) | (1<<1);  //CPHA = 1 , CPOL = 1;
	SPI1->CR1 |= (1<<2);
	SPI1->CR1 |= (0x2<<3);  //apb2 = 36 MHZ  36/8  
	SPI1->CR1 &= ~(1<<7);
	SPI1->CR1 |= (1<<8) | (1<<9);  //SSM = 1, SSI=1 for use GPIO pin like NSS pin
	SPI1->CR1 &= ~(1<<10);  //Full duplex
	SPI1->CR1 &= ~(1<<11);  // 8 bit data 
}

void GPIO_Config(void)
{
	//SCK PA5      MISO   PA6     MOSI   PA7    SLAVE SELECT   PA8
	RCC->APB2ENR |= (1<<2);
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	GPIOA->CRL |= (0x9<<20);
	GPIOA->CRL |= (0x4<<24);
	GPIOA->CRL |= (0x9<<28);  
	//GPIOA->CRH |=	(0x1<<0);
}

void SPI_Slave_Select(enum slaveNumber sn)
{	
	uint8_t x = sn;
	
	switch(x){
		case 3:
			GPIOA->CRH |= (0x1<<12);
		case 2:
			GPIOA->CRH |= (0x1<<8);
		case 1:
			GPIOA->CRH |= (0x1<<4);
		case 0:
			GPIOA->CRH |= (0x1<<0);
			break;
		}
}

void SPI_Enable(void)
{
	SPI1->CR1 |= (1<<6);
}

void SPI_Disable(void)
{
	SPI1->CR1 &= ~(1<<6);
}

void CS_Enable(uint8_t slavePin)
{
	GPIOA->BSRR = (1<<slavePin)<<16;
}

void CS_Disable(uint8_t slavePin)
{
	GPIOA->BSRR = (1<<slavePin);
}

void SPI_Transmit(uint8_t *data,int size)
{	
	int i = 0;
	while (i<size)
	{
			while (!(SPI1->SR & (1<<1))) {};
			SPI1->DR = data[i];
			i++;
	}
	
	
	while(!(SPI1->SR & (1<<1))) {};  // TXE = 1 olana kadar bekle
	while((SPI1->SR & (1<<7))) {};   //BSY = 1 oldugu sürece bekle
	volatile uint8_t temp = SPI1->DR;
	temp = SPI1->SR;
}

void SPI_Receive(uint8_t *data, int size)
{
	while(size){
		while((SPI1->SR & (1<<7))) {};
		SPI1->DR = 0;
		while (!(SPI1->SR & (1<<0))) {};
		*data++ = (SPI1->DR);
		size--;
	}
}

void SPI_SensorWrite(uint8_t address,uint8_t value,uint8_t slavePin)
{
	uint8_t data[2];
	data[0] = address;
	data[1] = value;
	CS_Enable(slavePin);
	SPI_Transmit(data,2);
	CS_Disable(slavePin);
}

uint8_t RxData[6];

void SPI_SensorRead(uint8_t address,uint8_t slavePin)
{
	CS_Enable(slavePin);
	SPI_Transmit(&address,1);   //Send address
	SPI_Receive(RxData,6);
	CS_Disable(slavePin);
}

int main()
{
	SysClockConfig();
	GPIO_Config();
	TIM6Config();
	SPI_Config();
	SPI_Enable();
	SPI_Slave_Select(slave_2);
	
	while(1)
	{
		SPI_SensorRead(0x32,8);
		delay_ms(500);
		SPI_SensorRead(0x33,9);
	}
}