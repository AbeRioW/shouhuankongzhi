#ifndef __ADXL345_H
#define __ADXL345_H 

#include "gpio.h"
#include "spi.h"
#include "math.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ADXL345_Data;


typedef struct {
    uint32_t step_count;
    float threshold_high;
    float threshold_low;
    uint8_t step_detected;
    float last_magnitude;
    float moving_avg[5]; // 移动平均窗口
    uint8_t avg_index;
} StepCounter;

void ADXL345_CS_Disable(void);
void ADXL345_CS_Enable(void);
void ADXL345_Init(void);
void ADXL345_ReadData(ADXL345_Data *accel);
float ADXL345_ToGravity(int16_t raw, uint8_t range);


void StepCounter_Init(StepCounter *sc);
uint8_t StepCounter_Update(StepCounter *sc, float x, float y, float z);

#endif

