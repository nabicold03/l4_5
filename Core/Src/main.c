/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "main_fsm.h"
#include "global.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t count_led_debug = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void displayTime();
void updateTime();
void main_fsm();
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
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // clear the lcd screen
  lcd_Clear(BLACK);

  // initialize time for ds3231
  updateTime();

  // initialize state for finite state machines
  status[0] = INIT;			// main fsm
  status[1] = TA_INIT;		// time adjust fsm
  status[2] = AL_INIT;		// alarm fsm

  // alarm_on indicates whether an alarm is on -> initialize 0
  alarm_on = 0;

  // Show MODE on the left corner of the lcd screen from the beginning til the end
  lcd_ShowStr(20, 20, "MODE", WHITE, RED, 24, 0);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // call main_fsm
	  main_fsm();

	  // flag_timer[0] every 50 ms
	  while(!flag_timer[0]);

	  // scan button
	  button_Scan();

	  // read time from ds3231 ic to ds3231 variables
	  ds3231_ReadTime();

	  // reset flag_timer[0] to 0
	  flag_timer[0] = 0;

	  // increase count_at
	  // count_at is a counter to keep track 10 times of flag_timer[0] -> Cycle: 50x10 = 500ms -> Frequency: f = 2Hz
	  // when count_at reaches 10 -> reset count_at and toggle the value of toggle_at
	  // toggle_at is used inside the main_fsm to hide or show the number of the time component adjusting 2 times/second
	  count_at = count_at + 1;
	  if(count_at == 1){		// 2Hz frequency toggle for adjust time
		  count_at = 0;
		  toggle_at = 1 - toggle_at;
	  }

	  // when there is an alarm on (an alarm is set already)
	  // and flag_alarm is active (this means the real time reaches the setup alarm)
	  // => WARNING ANNOUNCEMENT SHOWN ON THE SCREEN
	  if(alarm_on == 1 && flag_alarm){
		  // count_al and toggle_al are similar to the above count_at and toggle_at
		  // these 2 control signals are used to create the toggling animation of the WARNING on the screen
		  if(count_al == 10){
			  count_al = 0;
			  toggle_al = 1 - toggle_al;
		  } else {
			  count_al = count_al + 1;
		  }

		  // when toggle_al == 1 -> show the message
		  if(toggle_al == 1){
			  lcd_ShowStr(20, 150, "TIME TO WAKE UP!!!", RED, BLACK, 24, 1);
		  } else {		// when toggle_al == 0 => hide the message
			  lcd_ShowStr(20, 150, "TIME TO WAKE UP!!!", BLACK, BLACK, 24, 1);
		  }
	  }

	  // if button "0" is pressed -> stop alarm
	  if(button_count[13] == 1){
		  alarm_on = 0;
		  flag_alarm = 0;
		  lcd_ShowStr(20, 150, "TIME TO WAKE UP!!!", BLACK, BLACK, 24, 1);
	  }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	timer_init();
	button_init();
	lcd_init();
	ds3231_init();
	uart_init_rs232();
//	setTimer2(50);
	setTimer(50, 0);		// timer[0] -> 200 MHz frequency to scanbutton
	setTimer(500, 1);		// timer[1] -> 2 Hz frequency toggle time in adjust time mode
	setTimer(500, 2);		// timer[2] -> 2 Hz frequency for alarm animation
	setTimer(2000, 3);		// timer[3] -> display
	setTimer(200, 4);
}

void updateTime(){
	ds3231_Write(ADDRESS_YEAR, 24);
	ds3231_Write(ADDRESS_MONTH, 11);
	ds3231_Write(ADDRESS_DATE, 13);
	ds3231_Write(ADDRESS_DAY, 4);
	ds3231_Write(ADDRESS_HOUR, 9);
	ds3231_Write(ADDRESS_MIN, 45);
	ds3231_Write(ADDRESS_SEC, 0);
}
void displayTime(){
	lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
