/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_D0_Pin GPIO_PIN_13
#define OLED_D0_GPIO_Port GPIOC
#define OELD_D1_Pin GPIO_PIN_14
#define OELD_D1_GPIO_Port GPIOC
#define OLED_RES_Pin GPIO_PIN_15
#define OLED_RES_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_0
#define OLED_DC_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_1
#define OLED_CS_GPIO_Port GPIOA
#define DS18B20_Pin GPIO_PIN_4
#define DS18B20_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOA
#define BEEP_Pin GPIO_PIN_6
#define BEEP_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOA
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define KEY_R_Pin GPIO_PIN_8
#define KEY_R_GPIO_Port GPIOA
#define KEY_R_EXTI_IRQn EXTI9_5_IRQn
#define KEY_M_Pin GPIO_PIN_9
#define KEY_M_GPIO_Port GPIOA
#define KEY_M_EXTI_IRQn EXTI9_5_IRQn
#define KEY_L_Pin GPIO_PIN_10
#define KEY_L_GPIO_Port GPIOA
#define KEY_L_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
