#include "adxl345.h"

#define ADXL345_READ_BIT 0x80
#define ADXL345_MULTIBYTE_BIT 0x40

#define ADXL345_REG_DEVID 0x00
#define ADXL345_REG_POWER_CTL 0x2D
#define ADXL345_REG_DATA_FORMAT 0x31
#define ADXL345_REG_BW_RATE 0x2C
#define ADXL345_REG_DATAX0 0x32


void ADXL345_CS_Enable(void)
{
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
}

void ADXL345_CS_Disable(void)
{
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
}



uint8_t ADXL345_ReadRegister(uint8_t reg)
{
    uint8_t tx_data = reg | ADXL345_READ_BIT;
    uint8_t rx_data = 0;
    
    ADXL345_CS_Enable();
    HAL_SPI_Transmit(&hspi2, &tx_data, 1, 100);
    HAL_SPI_Receive(&hspi2, &rx_data, 1, 100);
    ADXL345_CS_Disable();
    
    return rx_data;
}

void ADXL345_WriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t tx_data[2] = {reg, value};
    
    ADXL345_CS_Enable();
    HAL_SPI_Transmit(&hspi2, tx_data, 2, 100);
    ADXL345_CS_Disable();
}

void ADXL345_ReadMultiRegisters(uint8_t reg, uint8_t *data, uint8_t count)
{
    uint8_t tx_data = reg | ADXL345_READ_BIT | ADXL345_MULTIBYTE_BIT;
    
    ADXL345_CS_Enable();
    HAL_SPI_Transmit(&hspi2, &tx_data, 1, 100);
    HAL_SPI_Receive(&hspi2, data, count, 100);
    ADXL345_CS_Disable();
}

void ADXL345_Init(void)
{
    //验证ID设备(0xe5)
    uint8_t devid = ADXL345_ReadRegister(ADXL345_REG_DEVID);
    if(devid != 0xE5)
    {
        //设备验证失败
        while(1);
    }
    
    //设置数据速率 (100Hz)
    ADXL345_WriteRegister(ADXL345_REG_BW_RATE, 0x0A);
		//ADXL345_WriteRegister(ADXL345_REG_BW_RATE, 0x03);
    
    //设置量程和全分辨率模式
    ADXL345_WriteRegister(ADXL345_REG_DATA_FORMAT, 0x01);
    
    //进入测量模式
    ADXL345_WriteRegister(ADXL345_REG_POWER_CTL, 0x08);
}

void ADXL345_ReadData(ADXL345_Data *accel)
{
    uint8_t data[6];
    
    //读6个数据寄存器(X0,X1,Y0,Y1,Z0,Z1)
    ADXL345_ReadMultiRegisters(ADXL345_REG_DATAX0, data, 6);
    
    //组合高低字节
    accel->x = (int16_t)((data[1] << 8) | data[0]);
    accel->y = (int16_t)((data[3] << 8) | data[2]);
    accel->z = (int16_t)((data[5] << 8) | data[4]);
}

//转换为重力加速度
float ADXL345_ToGravity(int16_t raw, uint8_t range)
{
    float scale_factor;
    
    switch(range)
    {
        case 0: //+-2g
            scale_factor = 4.0f / 1024.0f;
            break;
        case 1: //+-4g
            scale_factor = 8.0f / 1024.0f;
            break;
        case 2: // +-8g
            scale_factor = 16.0f / 1024.0f;
            break;
        case 3: //+-16g
            scale_factor = 32.0f / 1024.0f;
            break;
        default:
            scale_factor = 8.0f / 1024.0f;
    }
    
    return raw * scale_factor;
}

// 绉诲姩骞冲潎婊ゆ尝
static float MovingAverage_Filter(StepCounter *sc, float new_value) {
    sc->moving_avg[sc->avg_index] = new_value;
    sc->avg_index = (sc->avg_index + 1) % 5;
    
    float sum = 0;
    for(int i = 0; i < 5; i++) {
        sum += sc->moving_avg[i];
    }
    return sum / 5.0f;
}

// 璁℃鍣ㄥ垵濮嬪寲
void StepCounter_Init(StepCounter *sc) {
    sc->step_count = 0;
    sc->threshold_high = 1.2f;  // 楂橀槇鍊�(鍙牴鎹疄闄呰皟鏁�)
    sc->threshold_low = 0.9f;   // 浣庨槇鍊�
    sc->step_detected = 0;
    sc->last_magnitude = 1.0f;  // 鍒濆1g
    sc->avg_index = 0;
    for(int i = 0; i < 5; i++) {
        sc->moving_avg[i] = 1.0f;
    }
}

// 璁℃鍣ㄦ洿鏂板嚱鏁�
// 杩斿洖1琛ㄧず妫�娴嬪埌涓�姝ワ紝0琛ㄧず鏃犳鏁�
uint8_t StepCounter_Update(StepCounter *sc, float x, float y, float z) {
    // 璁＄畻鍚堟垚鍔犻�熷害(鍘婚櫎閲嶅姏褰卞搷)
    float magnitude = sqrt(x*x + y*y + z*z);
    
    // 绉诲姩骞冲潎婊ゆ尝
    magnitude = MovingAverage_Filter(sc, magnitude);
    
    uint8_t step_detected = 0;
    
    // 姝ユ暟妫�娴嬬畻娉�
    if(!sc->step_detected && magnitude > sc->threshold_high) {
        sc->step_detected = 1;
    }
    if(sc->step_detected && magnitude < sc->threshold_low) {
        sc->step_detected = 0;
        sc->step_count++;
        step_detected = 1;
    }
    
    sc->last_magnitude = magnitude;
    return step_detected;
}
