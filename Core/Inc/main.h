/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define DEBUG_LED_Pin GPIO_PIN_4
#define DEBUG_LED_GPIO_Port GPIOE
#define OUTPUT_Y0_Pin GPIO_PIN_5
#define OUTPUT_Y0_GPIO_Port GPIOE
#define OUTPUT_Y1_Pin GPIO_PIN_6
#define OUTPUT_Y1_GPIO_Port GPIOE
#define FSMC_RES_Pin GPIO_PIN_13
#define FSMC_RES_GPIO_Port GPIOC
#define INPUT_X0_Pin GPIO_PIN_6
#define INPUT_X0_GPIO_Port GPIOA
#define INPUT_X1_Pin GPIO_PIN_7
#define INPUT_X1_GPIO_Port GPIOA
#define INPUT_X2_Pin GPIO_PIN_4
#define INPUT_X2_GPIO_Port GPIOC
#define INPUT_X5_Pin GPIO_PIN_5
#define INPUT_X5_GPIO_Port GPIOC
#define LD_LATCH_Pin GPIO_PIN_6
#define LD_LATCH_GPIO_Port GPIOG
#define T_CS_Pin GPIO_PIN_7
#define T_CS_GPIO_Port GPIOG
#define T_CLK_Pin GPIO_PIN_8
#define T_CLK_GPIO_Port GPIOG
#define T_PEN_Pin GPIO_PIN_8
#define T_PEN_GPIO_Port GPIOC
#define T_MOSI_Pin GPIO_PIN_9
#define T_MOSI_GPIO_Port GPIOC
#define FSMC_BLK_Pin GPIO_PIN_8
#define FSMC_BLK_GPIO_Port GPIOA
#define T_MISO_Pin GPIO_PIN_12
#define T_MISO_GPIO_Port GPIOC
#define BTN_LOAD_Pin GPIO_PIN_3
#define BTN_LOAD_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
