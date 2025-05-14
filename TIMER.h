#ifndef LED_TIMER_H
#define LED_TIMER_H
#include "stm32f10x.h"

void TIM6Config (void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

#endif