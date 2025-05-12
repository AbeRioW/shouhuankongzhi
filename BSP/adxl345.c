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
    //ÑéÖ¤IDÉè±¸(0xe5)
    uint8_t devid = ADXL345_ReadRegister(ADXL345_REG_DEVID);
    if(devid != 0xE5)
    {
        //Éè±¸ÑéÖ¤Ê§°Ü
        while(1);
    }
    
    //ÉèÖÃÊı¾İËÙÂÊ (100Hz)
    ADXL345_WriteRegister(ADXL345_REG_BW_RATE, 0x0A);
    
    //ÉèÖÃÁ¿³ÌºÍÈ«·Ö±æÂÊÄ£Ê½
    ADXL345_WriteRegister(ADXL345_REG_DATA_FORMAT, 0x01);
    
    //½øÈë²âÁ¿Ä£Ê½
    ADXL345_WriteRegister(ADXL345_REG_POWER_CTL, 0x08);
}

void ADXL345_ReadData(ADXL345_Data *accel)
{
    uint8_t data[6];
    
    //¶Á6¸öÊı¾İ¼Ä´æÆ÷(X0,X1,Y0,Y1,Z0,Z1)
    ADXL345_ReadMultiRegisters(ADXL345_REG_DATAX0, data, 6);
    
    //×éºÏ¸ßµÍ×Ö½Ú
    accel->x = (int16_t)((data[1] << 8) | data[0]);
    accel->y = (int16_t)((data[3] << 8) | data[2]);
    accel->z = (int16_t)((data[5] << 8) | data[4]);
}

//×ª»»ÎªÖØÁ¦¼ÓËÙ¶È
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
        case 3: // ±16g
            scale_factor = 32.0f / 1024.0f;
            break;
        default:
            scale_factor = 8.0f / 1024.0f;
    }
    
    return raw * scale_factor;
}