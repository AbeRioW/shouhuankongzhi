/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "ds3231.h"
#include "string.h"
#include "max30102.h"
#include "adxl345.h"
#include "ds18b20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void ui_setting(void);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	Ds_time ds_time;
	char show_data[20];
		int32_t HR_Value,BO_Value;
	int8_t HR_Valid,BO_Valid;
	ADXL345_Data accel_data;
	char uart_send[20];
	  float num_d;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
	OLED_Clear();
	
	    //init MAX30102
		Max30102_Init();
	  Max30102_Safety();
		
		ADXL345_Init();
		if(DS_Init()==0)
		{
		}
#if 0
	OLED_ShowString(0,16,(uint8_t*)"hello",16,1);
	OLED_Refresh();
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		DS3231_GetTime(&ds_time);
		sprintf(show_data,"%d-%d-%d %d:%d:%d",ds_time.year,ds_time.month,ds_time.date,ds_time.hour,ds_time.min,ds_time.sec);
		OLED_ShowString(0,0,(uint8_t*)show_data,8,1);
		OLED_Refresh();
		
		Max30102_Calculate_HR_BO_Value(&HR_Value,&HR_Valid,&BO_Value,&BO_Valid);
		if(HR_Valid==1 && BO_Valid==1)
		{
			OLED_ShowString(1,10,"HR:",8,1);
			//OLED_ShowString(1,20,"BO:",8,1);
			OLED_ShowNum(20,10,HR_Value,3,8,1);
			//OLED_ShowNum(20,20,BO_Value,3,8,1);
			OLED_Refresh();
			sprintf(uart_send,"HR:%03d",HR_Value);
			HAL_UART_Transmit(&huart2,(uint8_t*)uart_send,6,0xffff);
		}
		
		 ADXL345_ReadData(&accel_data);
		 float x_g = ADXL345_ToGravity(accel_data.x, 1); //
     float y_g = ADXL345_ToGravity(accel_data.y, 1);
     float z_g = ADXL345_ToGravity(accel_data.z, 1);
		
		sprintf(show_data,"x:%.2f y:%.2f z:%.2f",x_g,y_g,z_g);
		OLED_ShowString(0,20,(uint8_t*)show_data,8,1);
		OLED_Refresh();
//					sprintf(uart_send,"HR:%03d",HR_Value);
			HAL_UART_Transmit(&huart2,(uint8_t*)show_data,21,0xffff);
		
		num_d = Get_DS_Temperature();		
		oled_showFnum(1,30,num_d,8,1);
		OLED_Refresh();
		if(num_d>37)
		{
			//beep_led_control(true);
		}
		else
		{
				//beep_led_control(false);
		}
		
		if(button==ENSURE_BUTTON)
		{
				HAL_Delay(300);
			  button = UNPRESSED;
			  OLED_Clear();
			  ui_setting();
			  
		}
		 HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void ui_setting(void)
{
	  OLED_ShowString(0,20,(uint8_t*)"123",8,1);
		OLED_Refresh();
	  while(1)
		{
				
		}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
