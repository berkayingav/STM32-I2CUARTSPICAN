#include "i2c_protocol.h"

/*

	 I2C1_SCL  PB6 and I2C1_SCL PB7

*/

void i2cConfig (void) 
{

	RCC->APB2ENR |= (1<<3);    //GPIOB enabled
	RCC->APB1ENR |= (1<<21);   // I2C1 enabled
	
	GPIOB->CRL &= ~(0xFF << 24);
	GPIOB->CRL |= (13<<28) | (13<<24);   //aLTERNATE FUNCTION OPEN DRAIN 10MHz
	
	I2C1->CR1 |= (1 << 15);   // Yazilimsal reset etkinlestir
	I2C1->CR1 &= ~(1 << 15);  // Reset bitini temizle
	
	I2C1->CR2 &= ~(0X3F);
	I2C1->CR2 |= (18<<0);  //Peripoheral clock
	
	// Duty 0 and mode is slow mode 
	//calculate the CCR thigh and tlow formula
	// CCR formula (Tr(SCL) +Tw(SCLH))  / Tpclk
	// 55.56ns =  1/18
	//  (1000 + 4000) / 55.56     =  90;
	I2C1->CCR |= (90<<0);
	
	// TRISE = (Tr(SCL) / T(pclk) )+ 1
	// TRISE = 1000/55.56  + 1;
	// TRISE = 19;
	
	I2C1->TRISE |= (19<<0);
	I2C1->CR1 |= (1<<0);
}

void i2c_start(void){
	
	I2C1->CR1 |=(1<<8);
	
	while (!(I2C1->SR1 & (1<<0)));
	
}

void i2c_write(uint8_t data) {
	while (!(I2C1->SR1 & (1<<7))); // wait txe bit set 
	I2C1->DR = data;
	while(!(I2C1->SR1 & (1<<2))); // wait btf bit set
}

void i2c_adress (uint8_t address){
	
	I2C1->DR = address;
	while(!(I2C1->SR1 & (1<<1)));
}

void i2c_regadress (uint8_t regaddress)
{
	uint8_t temp = I2C1->SR1 | I2C1->SR2;
	temp*=temp;
	i2c_write(regaddress);
}

void i2c_stop (){
	I2C1->CR1 |= (1<<9);
}

void i2c_writemulti (uint8_t *data,uint8_t size){
	
	while(!(I2C1->SR1& (1<<7)));
	while(size)
	{
		while (!(I2C1->SR1 & (1<<7)));
		I2C1->DR = (volatile uint32_t )*data++;
		size--;
	}
	
	while (!(I2C1->SR1 & (1<<2)));
}


void i2c_readpro(uint8_t addr,uint8_t regaddr,uint8_t *buffer,uint8_t size){
	uint8_t rem = size;
	volatile uint8_t temp;
	i2c_start();
	i2c_adress((addr << 1) | 0);
	i2c_regadress (regaddr);
	i2c_start();
	i2c_adress((addr << 1) | 1);

	if(size == 1){
		I2C1->CR1 &=~(1<<10); //GENERATE NACK SIGNAL
		i2c_stop();
		temp =(I2C1->SR1 | I2C1->SR2);
		while(!(I2C1->SR1 & (1<<6)));
		buffer[size-rem]= I2C1->DR;
	}
	else{
		temp=I2C1->SR1 | I2C1->SR2; //Clear ADDR BIT
		while(rem>2){

			while(!(I2C1->SR1 & (1<<6)));      //CHECK DATA REGISTER NOT EMPTY
			buffer[size-rem]=I2C1->DR;   //READ DATA REGISTER
			I2C1->CR1 |=(1<<10);			   //GENERATE ACK SIGNAL
			rem--;
		}
		while(!(I2C1->SR1 & (1<<6)));
		buffer[size-rem]=I2C1->DR;
		I2C1->CR1 &=~(1<<10);
		i2c_stop();
		rem--;
		while(!(I2C1->SR1 & (1<<6)));
		buffer[size-rem]=I2C1->DR;
	}
	
}

void i2cwritepro(uint8_t addr,uint8_t regaddr,uint8_t *data,uint8_t size){
	 i2c_start();
	 i2c_adress((addr << 1) | 0);
	 i2c_regadress(regaddr);
	 i2c_writemulti(data,size);
	 i2c_stop();
}


