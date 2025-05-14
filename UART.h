#ifndef UARTTXRX_H
#define UARTTXRX_H

#include "RccConfig.h"
#include "ledTimer.h"
void uart1config (void);
void uart1_SendChar(uint8_t c);
void uart1_SendString (char *string);
uint8_t uart1_GetChar(void);

#endif