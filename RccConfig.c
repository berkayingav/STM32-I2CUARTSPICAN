
#include "RccConfig.h"


void SysClockConfig (void)
{
	//enable HSE and wait for the HSE to become ready
	// set the power enable clock and voltage regulator 
	// Configure the flash prefetch and the latency related settings
	// Configure the prescalars HCLK , PCLK1 , PCLK2
	
	
	#define PLL 8
	
	RCC->CR |= RCC_CR_HSEON;
	
	while(!(RCC->CR & RCC_CR_HSERDY));
	
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	
	FLASH->ACR |= FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;
	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	RCC->CFGR |= RCC_CFGR_PLLMULL9;
	RCC->CFGR |= RCC_CFGR_PLLSRC;
	
	// enable the PLL and wait for it to become ready 
	
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
	
	// set the clock  source
	
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
}
