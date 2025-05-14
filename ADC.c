#include "adc_using.h"

void ADC_Init(void){
	
	RCC->APB2ENR |= (1<<9);
	RCC->APB2ENR |= (1<<2);
	
	RCC->CFGR |= (1<<15);   // adc prescaler pclk2/4   max adc clock = 14MHz 
	ADC1->CR1 |= (1<<8);
	
	ADC1->CR2 |= (1<<1);
	ADC1->CR2 &= ~(1 << 11);
	
	//Analog Watchdog enable
	ADC1->CR1 |= (1<<6);
	ADC1->CR1 |= (1<<23);
	
	ADC1->CR2 |= (1<<23); // Temp sensor and voltage channel 16-17
	
	ADC1->CR1 |= (16<<0);
	
	ADC1->HTR = 1000; //Random Value
	ADC1->LTR = 0;    //Random Value
	
	//sampling time for the channels 
	ADC1->SMPR1 &= ~((1 << 18) | (1 << 21)); //channel 16 and 17
	ADC1->SMPR2 &= ~((1 << 3) | (1 << 12)); //channel 1 and channel 4 
	
	//Channel sequence
	ADC1->SQR1 &= ~(0xF << 20);  
	ADC1->SQR1 |= (3 << 20);     // 4 channel
	ADC1->SQR3 = (1 << 0) | (4 << 5) | (16 << 10) | (17 << 15);
	
	//GPIO Pin Settings
	GPIOA->CRL &= ~(0xF << 4);  
	GPIOA->CRL &= ~(0xF << 16);  
}

void ADC_Enable(void)
{
	ADC1->CR2 |= (1<<0);
	delay_us(10);
}

void ADC_Start(void)
{
	ADC1->SR = 0;
	ADC1->CR2 |= (1<<22);
	
}

void ADC_WaitForConv(void){
	
	while(!(ADC1->SR & (1<<1)));

}

void ADC_Disable(void) 
{
	ADC1->CR2 &= ~(1<<0);
}

uint16_t ADC_GetVal(void)
{
	return ADC1->DR;
}

void ADC_ReadChannels(uint16_t *results) {
	for(int i = 0;i < 4; i++){
		ADC_Start();
		ADC_WaitForConv();
		results[i] = ADC_GetVal();
	}
	
	if(ADC1->SR & (1<<0)) {
		GPIOA->BSRR = (1 << 5);
		delay_ms(100);          
		GPIOA->BSRR = (1 << 21);  
    ADC1->SR &= ~(1 << 0);  // AWD bayragini temizle
	}
}

uint16_t adc_Values[4] = {0,0,0,0};

int main(){
	SysClockConfig();
	ADC_Init();
	ADC_Enable();
	
	while(1){
			ADC_ReadChannels(adc_Values);
	}
}




