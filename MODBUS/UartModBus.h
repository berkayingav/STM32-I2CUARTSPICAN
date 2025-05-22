#ifndef MODBUS_H
#define MODBUS_H
#include "RCC_Config.h"
#include "TIMERR.h"

//Functions vb
volatile uint8_t *buffer;
volatile uint16_t bufSize;
volatile uint16_t txIndex;

volatile uint8_t *rxBuffer;
volatile uint16_t rxbufSize;
volatile uint16_t rxIndex;

uint8_t RxData[32];


enum functionCodes
{
	rCoils = 0x01,
	rDiscInputs = 0x02,
	rMHRegisters = 0x03,
	rIRegisters = 0x04,
	wSCoil = 0x05,
	wSHRegister = 0x06,
	wMCoils = 0x0F,
	wMHRegisters = 0x10
};

static uint16_t crc = 0xFFFF;

typedef void (*Uart_Callback)(uint8_t data,uint16_t size);
Uart_Callback uart_rx_cb = 0;

void Uart1_Config (void);
void uart1_Receive_IT_Callback(Uart_Callback uartCall);
void uart1_Transmit(uint8_t *data,uint16_t size,uint32_t timeout);
void uart1_Transmit_IT(uint8_t *data,uint16_t size);
void uart1_Receive(uint8_t *data,uint16_t size,uint32_t timeout);
void uart1_Receive_IT(uint8_t *data,uint16_t size);
void USART1_IRQHandler(void);
void sendData(uint8_t *data,uint8_t len);
uint16_t calculate_modbus_crc(const uint8_t *data, int length);
void modbusDataRead(uint8_t slaveAddr,enum functionCodes fc,uint16_t regAddr,uint8_t regPts);
void modbusDataWrite(uint8_t slaveAddr,enum functionCodes fc,uint16_t regAddr,uint8_t *data,uint8_t dataSize,uint8_t coilSize);
void ModBusRxCallback(uint8_t *data,uint16_t len);
int main(void);
#endif
