#ifndef CANBUS_PROTOCOL_H
#define CANBUS_PROTOCOL_H
#include "RccConfig.h"
#include "ledTimer.h"

enum pclk{MHZ_72,MHZ_36,MHZ_18,MHZ_9,MHZ_4_5};
enum bitrate{KBPS_125,KBPS_250_KBPS_5O0};
enum fifonumber{FIFO0,FIFO1};

typedef const struct
{
	uint8_t brp;
	uint8_t	ts1;
	uint8_t ts2;
}can_btr;

typedef const struct
{
	uint16_t pclkrate;
	can_btr btrValues[3];
}rcc_apbp1;

rcc_apbp1 bitRatesSet[5] = {
{720,{{8,50,11},{4,44,11},{2,56,9}}},
{360,{{5,36,11},{2,36,11},{1,27,8}}},
{180,{{2,36,11},{1,27,8},{0,27,8}}},
{90,{{0,56,15},{0,27,8},{0,13,4}}},
{45,{{0,27,8},{0,13,4},{0,6,2}}},
};

void CAN_Config(enum pclk p,enum bitrate btr);
void SetFilter(uint8_t bank,uint16_t id,uint16_t mask);
void setFiltersFIFO(uint8_t filterNumber,enum fifonumber fn);
int CAN_Send_Data(uint16_t id,uint8_t *data,uint8_t length);
int CAN_Receive_Data(uint16_t *id,uint8_t *data,uint8_t *length);





#endif