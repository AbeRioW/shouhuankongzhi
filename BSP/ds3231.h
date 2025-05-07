#ifndef __DS3231_H
#define __DS3231_H 

#include "main.h"
#include "i2c.h"

#define DS3231_ADDRESS 0xD0  // DS3231的I2C地址(7位地址左移1位)

typedef struct ds_time{
		uint8_t sec;
		uint8_t min;
		uint8_t hour;
		uint8_t week;
		uint8_t date;
		uint8_t month;
		uint8_t year;
}Ds_time;


HAL_StatusTypeDef DS3231_GetTime(Ds_time *ds_time);
#endif
