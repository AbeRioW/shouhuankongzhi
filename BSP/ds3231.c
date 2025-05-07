#include "ds3231.h"

// BCD转10进制数
uint8_t BCD2DEC(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// 10进制数转BCD
uint8_t DEC2BCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

HAL_StatusTypeDef DS3231_GetTime(Ds_time *ds_time)
{
    uint8_t get_time[7];
    HAL_StatusTypeDef status;
    
    status = HAL_I2C_Mem_Read(&hi2c2, DS3231_ADDRESS, 0x00, 1, get_time, 7, 100);
    
    if(status == HAL_OK)
    {
        ds_time->sec= BCD2DEC(get_time[0] & 0x7F);  //去掉最高位
        ds_time->min =  BCD2DEC(get_time[1]);
        ds_time->hour = BCD2DEC(get_time[2] & 0x3F);     // 24???
        ds_time->week = BCD2DEC(get_time[3]);
        ds_time->date = BCD2DEC(get_time[4]);
        ds_time->month = BCD2DEC(get_time[5] & 0x1F); 
        ds_time->year = BCD2DEC(get_time[6]);
    }
    
    return status;
}

//设置时间
HAL_StatusTypeDef DS3231_WriteTime(Ds_time *time) {
    uint8_t data[7];
    
    data[0] = DEC2BCD(time->sec);
    data[1] = DEC2BCD(time->min);
    data[2] = DEC2BCD(time->hour);    
    data[3] = DEC2BCD(time->week);
    data[4] = DEC2BCD(time->date);
    data[5] = DEC2BCD(time->month);
    data[6] = DEC2BCD(time->year);
    
    // 写入从0x00地址开始的7个寄存器
    return HAL_I2C_Mem_Write(&hi2c2, DS3231_ADDRESS, 0x00, 1, data, 7, HAL_MAX_DELAY);
}