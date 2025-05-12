#ifndef __ADXL345_H
#define __ADXL345_H 

#include "gpio.h"
#include "spi.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ADXL345_Data;

void ADXL345_CS_Disable(void);
void ADXL345_CS_Enable(void);
void ADXL345_Init(void);
void ADXL345_ReadData(ADXL345_Data *accel);
float ADXL345_ToGravity(int16_t raw, uint8_t range);

#endif

