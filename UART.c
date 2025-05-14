
#include "uart_tx_rx.h"

/*  
		GPIOA 9 and GPIOA 10
*/

void uart1config (void)
{
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 	// RCC->APB2ENR |= (1<<2);
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // RCC->APB2ENR |= (1<<14);
	
	GPIOA->CRH &= ~(0xFF << 4);
	GPIOA->CRH |= (9<<4) | (4<<8);
	
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
	
}

void uart1_SendChar(uint8_t c){
	
	while (!(USART1->SR & (1 << 7)));
	USART1->DR = c;	
	
}

void uart1_SendString (char *string)
{
	while (*string) uart1_SendChar(*string++);
}

uint8_t uart1_GetChar(void){
	
	uint8_t temp;
	
	while(!(USART1->SR & (1<<5)));
	temp = USART1->DR;
	return temp;
	
}
