#ifndef  __DS18B20_H_
#define  __DS18B20_H_

#include "stdint.h"


#define  SKIP_ROM         0xCC    //Skip rom in DS18B20
#define  CONVERT_TEMP     0x44    //Transition temperature
#define  READ_SCRATCHPAD  0xBE    //Get data fromregistor in DS18B20


#define   DS_PORT_Pin  DS18B20_Pin
#define   DS_PORT_GPIO_Port  DS18B20_GPIO_Port

	#ifdef __cplusplus
		extern "C" {
	#endif
		uint8_t DS_Init(void);
		float Get_DS_Temperature(void);

	#ifdef __cplusplus
		}
	#endif

#endif

