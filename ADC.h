#ifndef ADC_CONFIGG_H
#define ADC_CONFIGG_H

#include "RccConfig.h"
#include "ledTimer.h"

void ADC_Init(void);
void ADC_Enable(void);
void ADC_Start(void);
void ADC_WaitForConv(void);
void ADC_Disable(void);
uint16_t ADC_GetVal (void);
void ADC_ReadChannels(uint16_t *results);
int main();

#endif