/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
enum BUTTON button =UNPRESSED;
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OLED_D0_Pin|OELD_D1_Pin|OLED_RES_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OLED_DC_Pin|OLED_CS_Pin|DS18B20_Pin|LED_Pin
                          |BEEP_Pin|LED2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : OLED_D0_Pin OELD_D1_Pin OLED_RES_Pin */
  GPIO_InitStruct.Pin = OLED_D0_Pin|OELD_D1_Pin|OLED_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : OLED_DC_Pin OLED_CS_Pin LED_Pin BEEP_Pin
                           LED2_Pin */
  GPIO_InitStruct.Pin = OLED_DC_Pin|OLED_CS_Pin|LED_Pin|BEEP_Pin
                          |LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DS18B20_Pin */
  GPIO_InitStruct.Pin = DS18B20_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_CS_Pin */
  GPIO_InitStruct.Pin = SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI2_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_R_Pin KEY_M_Pin KEY_L_Pin */
  GPIO_InitStruct.Pin = KEY_R_Pin|KEY_M_Pin|KEY_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
void beep_control(bool open)
{
	HAL_GPIO_WritePin(GPIOA, BEEP_Pin, open?GPIO_PIN_RESET:GPIO_PIN_SET);
}

void led1_control(bool open)
{
	HAL_GPIO_WritePin(GPIOA, LED_Pin, open?GPIO_PIN_RESET:GPIO_PIN_SET);
}

void led2_control(bool open)
{
	HAL_GPIO_WritePin(GPIOA, LED2_Pin, open?GPIO_PIN_RESET:GPIO_PIN_SET);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
		if(GPIO_Pin==KEY_L_Pin)
		{
			  button = DOWN_BUTTON;
		}
		
		if(GPIO_Pin==KEY_M_Pin)
		{
				button = UP_BUTTON;
		}
		
		if(GPIO_Pin==KEY_R_Pin)
		{
				button  = ENSURE_BUTTON;
		}
}

/* USER CODE END 2 */
