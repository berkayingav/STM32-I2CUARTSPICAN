#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include "RccConfig.h"
#include "ledTimer.h"

void i2cConfig (void);
void i2c_start(void);
void i2c_write(uint8_t data);
void i2c_adress (uint8_t address);
void i2c_regadress (uint8_t regaddress);
void i2c_stop();
void i2c_writemulti (uint8_t *data,uint8_t size);
void i2c_readpro(uint8_t addr,uint8_t regaddr,uint8_t *buffer,uint8_t size);
void i2cwritepro(uint8_t addr,uint8_t regaddr,uint8_t *data,uint8_t size);

#endif