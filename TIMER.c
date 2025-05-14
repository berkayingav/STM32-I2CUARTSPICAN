#include "RccConfig.h"

void TIM6Config (void)
{
	// Enable timer clock
	// set the prescalar and the arr
	// enable the timer and wait for the update flag to set 
	
		RCC->APB1ENR |= (1<<4);
		
		TIM6->PSC = 45 - 1;
		TIM6->ARR = 0xffff - 1;
		TIM6->CR1 |= (1<<0);
		while(!(TIM6->SR & (1<<0)));
	
}

void delay_us(uint16_t us)
{
	TIM6->CNT = 0;
	while (TIM6->CNT < us);
}

void delay_ms(uint16_t ms)
{
		for(uint16_t i = 0; i<ms;i++){
			delay_us(1000);
		}
}

