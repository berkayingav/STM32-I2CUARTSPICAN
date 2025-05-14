#ifndef SPI_PROTOCOL_H
#define SPI_PROTOCOL_H
#include "RccConfig.h"
#include "ledTimer.h"

enum slaveNumber{slave_1,slave_2,slave_3,slave_4};

void SPI_Config(void);
void GPIO_Config(void);
void SPI_Slave_Select(enum slaveNumber sn);
void SPI_Enable(void);
void SPI_Disable(void);
void CS_Enable(uint8_t slavePin);
void CS_Disable(uint8_t slavePin);
void SPI_Transmit(uint8_t *data,int size);
void SPI_Receive(uint8_t *data, int size);
void SPI_SensorWrite(uint8_t address,uint8_t value,uint8_t slavePin);
void SPI_SensorRead(uint8_t address,uint8_t slavePin);

#endif