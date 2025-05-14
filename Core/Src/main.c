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
#include "tim.h"
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
uint8_t hr_bnd=150,bo_band=100;
uint8_t temp_band=30;
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

	char uart_send[20];
	float num_d;
	ADXL345_Data accel_data;
	static int page_count=0;
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
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
	OLED_Clear();
	
	    //init MAX30102
		Max30102_Init();
	  Max30102_Safety();
		
		ADXL345_Init();
		
		// 初始化计步器
    StepCounter_Init(&step_counter);
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
			 if(timer1_up)
			{
				timer1_up = false;
				DS3231_GetTime(&ds_time);
			  
				//OLED_ShowString(0,0,(uint8_t*)"time:",16,1);
				sprintf(show_data,"%d-%d-%d %d:%d:%d",ds_time.year,ds_time.month,ds_time.date,ds_time.hour,ds_time.min,ds_time.sec);
				OLED_ShowString(0,0,(uint8_t*)show_data,8,1);
				OLED_Refresh();	
			
			}		
		
			//显示等加速度
			ADXL345_ReadData(&accel_data);
			float x_g = ADXL345_ToGravity(accel_data.x, 1); 
			float y_g = ADXL345_ToGravity(accel_data.y, 1);
			float z_g = ADXL345_ToGravity(accel_data.z, 1);	
			sprintf(show_data,"x:%.1f y:%.1f z:%.1f",x_g,y_g,z_g);
			OLED_ShowString(0,40,(uint8_t*)show_data,8,1);
			OLED_Refresh();			
				//OLED_Refresh();
				// 更新计步器
			  
//				if(StepCounter_Update(&step_counter, x_g, y_g, z_g)) {
//					// 检测到一步
//					sprintf(show_data,"Step count: %d",step_counter.step_count);
//					OLED_ShowString(0,40,(uint8_t*)show_data,8,1);
//					OLED_Refresh();
//				}		
				//	HAL_Delay(4);


//				//显示心率和血氧
				Max30102_Calculate_HR_BO_Value(&HR_Value,&HR_Valid,&BO_Value,&BO_Valid);
				if(HR_Valid==1 && BO_Valid==1)
				{
					OLED_ShowString(0,10,(uint8_t*)"HR:",8,1);
					OLED_ShowNum(20,10,HR_Value,3,8,1);
			
					OLED_ShowString(50,10,(uint8_t*)"BO:",8,1);
					OLED_ShowNum(60,10,BO_Value,3,8,1);
					OLED_Refresh();
			
					sprintf(uart_send,"HR:%03d BO:%03d",HR_Value,BO_Value);
					HAL_UART_Transmit(&huart2,(uint8_t*)uart_send,13,0xffff);
				}

				// HAL_UART_Transmit(&huart2,(uint8_t*)show_data,21,0xffff);
		
				//显示体温
				num_d = Get_DS_Temperature();		
				oled_showFnum(1,30,num_d,8,1);
				OLED_Refresh();	
	
		 if(button==ENSURE_BUTTON)
		 {
				 HAL_Delay(300);
			   button = UNPRESSED;
			   OLED_Clear();
			   ui_setting();
			  
		 }
	 }
		 //HAL_Delay(500);
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
	  int position=0;
	  char show[30];
	  Ds_time ds_time;
	  DS3231_GetTime(&ds_time);
		OLED_ShowString(20,0,(uint8_t*)"Setting",8,1);
	  while(1)
		{
			sprintf(show,"20%02d",ds_time.year);
			OLED_ShowString(0,10,(uint8_t*)show,8,position==0?0:1);			
			sprintf(show,"-%02d",ds_time.month);
			OLED_ShowString(25,10,(uint8_t*)show,8,position==1?0:1);			
			sprintf(show,"-%02d",ds_time.date);
			OLED_ShowString(43,10,(uint8_t*)show,8,position==2?0:1);
			
			sprintf(show," %02d:",ds_time.hour);
			OLED_ShowString(67,10,(uint8_t*)show,8,position==3?0:1);			
			sprintf(show,"%02d:",ds_time.min);
			OLED_ShowString(91,10,(uint8_t*)show,8,position==4?0:1);			
			sprintf(show,"%02d",ds_time.sec);
			OLED_ShowString(109,10,(uint8_t*)show,8,position==5?0:1);
			
			sprintf(show,"HR:%03d ",hr_bnd);
			OLED_ShowString(0,20,(uint8_t*)show,8,position==6?0:1);
			sprintf(show,"BO:%03d",bo_band);
			OLED_ShowString(50,20,(uint8_t*)show,8,position==7?0:1);
			
			sprintf(show,"temp:%02d",temp_band);
			OLED_ShowString(0,30,(uint8_t*)show,8,position==8?0:1);
			
			OLED_ShowString(0,40,(uint8_t*)"BACK",8,position==9?0:1);
			OLED_Refresh();
			
			if(button==ENSURE_BUTTON)
		 {
				 HAL_Delay(300);
			   button = UNPRESSED;
			   position++;
			   if(position>9)
					 position=0;
		 }
		 
		 if(button==DOWN_BUTTON)
		 {
			 	 HAL_Delay(300);
			   button = UNPRESSED;
			   switch(position)
				{
					case 0:
						ds_time.year--;
            if(ds_time.year<0)
						{
							 ds_time.year=99;
						}
						break;
					case 1:
						ds_time.month--;
            if(ds_time.month<1)
						{
							 ds_time.month=12;
						}
						break;
					case 2:
						ds_time.date--;
            if(ds_time.date<1)
						{
							 ds_time.date=31;
						}
						break;
					case 3:
						ds_time.hour--;
            if(ds_time.hour<0)
						{
							 ds_time.hour=23;
						}
						break;
					case 4:
						ds_time.min--;
            if(ds_time.min<0)
						{
							 ds_time.min=59;
						}
						break;
					case 5:
						ds_time.min--;
            if(ds_time.min<0)
						{
							 ds_time.min=59;
						}
						break;
					case 6:
						hr_bnd--;
            if(hr_bnd<30)
						{
							 hr_bnd=200;
						}
					break;
					case 7:
						bo_band--;
            if(bo_band<30)
						{
							 bo_band=200;
						}
					break;
					case 8:
						temp_band--;
            if(temp_band<10)
						{
							 temp_band=50;
						}
					break;
				}
		 }
		 
		 if(button==UP_BUTTON)
		 {
			 	 HAL_Delay(300);
			   button = UNPRESSED;
			   switch(position)
				{
					case 0:
						ds_time.year++;
            if(ds_time.year>99)
						{
							 ds_time.year=0;
						}
						break;
					case 1:
						ds_time.month++;
            if(ds_time.month>12)
						{
							 ds_time.month=1;
						}
						break;
					case 2:
						ds_time.date++;
            if(ds_time.date>31)
						{
							 ds_time.date=1;
						}
						break;
					case 3:
						ds_time.hour++;
            if(ds_time.hour>23)
						{
							 ds_time.hour=0;
						}
						break;
					case 4:
						ds_time.min++;
            if(ds_time.min>59)
						{
							 ds_time.min=0;
						}
						break;
					case 5:
						ds_time.min++;
            if(ds_time.min>59)
						{
							 ds_time.min=0;
						}
						break;
					case 6:
						hr_bnd++;
            if(hr_bnd>200)
						{
							 hr_bnd=30;
						}
					break;
					case 7:
						bo_band++;
            if(bo_band>200)
						{
							 bo_band=30;
						}
					break;
					case 8:
						temp_band++;
            if(temp_band>50)
						{
							 temp_band=10;
						}
					break;
					case 9:
					OLED_Clear();
					DS3231_WriteTime(&ds_time);
					return;
					break;
				}
		 }
			
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
