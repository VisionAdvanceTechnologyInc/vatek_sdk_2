/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define RP_PIN22_Pin GPIO_PIN_2
#define RP_PIN22_GPIO_Port GPIOE
#define RP_PIN07_Pin GPIO_PIN_3
#define RP_PIN07_GPIO_Port GPIOE
#define RP_PIN11_Pin GPIO_PIN_4
#define RP_PIN11_GPIO_Port GPIOE
#define RP_PIN13_Pin GPIO_PIN_5
#define RP_PIN13_GPIO_Port GPIOE
#define RP_PIN06_Pin GPIO_PIN_6
#define RP_PIN06_GPIO_Port GPIOE
#define SPI_1_CS0_Pin GPIO_PIN_4
#define SPI_1_CS0_GPIO_Port GPIOA
#define LCD_01_Pin GPIO_PIN_8
#define LCD_01_GPIO_Port GPIOE
#define LCD_02_Pin GPIO_PIN_9
#define LCD_02_GPIO_Port GPIOE
#define LCD_03_Pin GPIO_PIN_10
#define LCD_03_GPIO_Port GPIOE
#define SW_01_Pin GPIO_PIN_11
#define SW_01_GPIO_Port GPIOE
#define SW_02_Pin GPIO_PIN_12
#define SW_02_GPIO_Port GPIOE
#define SW_03_Pin GPIO_PIN_13
#define SW_03_GPIO_Port GPIOE
#define SW_04_Pin GPIO_PIN_14
#define SW_04_GPIO_Port GPIOE
#define SW_05_Pin GPIO_PIN_15
#define SW_05_GPIO_Port GPIOE
#define LCD_K_Pin GPIO_PIN_8
#define LCD_K_GPIO_Port GPIOD
#define LCD_RS_Pin GPIO_PIN_9
#define LCD_RS_GPIO_Port GPIOD
#define LCD_RW_Pin GPIO_PIN_10
#define LCD_RW_GPIO_Port GPIOD
#define LCD_EE_Pin GPIO_PIN_11
#define LCD_EE_GPIO_Port GPIOD
#define LCD_D4_Pin GPIO_PIN_12
#define LCD_D4_GPIO_Port GPIOD
#define LCD_D5_Pin GPIO_PIN_13
#define LCD_D5_GPIO_Port GPIOD
#define LCD_D6_Pin GPIO_PIN_14
#define LCD_D6_GPIO_Port GPIOD
#define LCD_D7_Pin GPIO_PIN_15
#define LCD_D7_GPIO_Port GPIOD
#define RP_PINCS1_Pin GPIO_PIN_0
#define RP_PINCS1_GPIO_Port GPIOD
#define RP_PIN32_Pin GPIO_PIN_1
#define RP_PIN32_GPIO_Port GPIOD
#define RP_PIN31_Pin GPIO_PIN_2
#define RP_PIN31_GPIO_Port GPIOD
#define RP_PIN36_Pin GPIO_PIN_3
#define RP_PIN36_GPIO_Port GPIOD
#define RP_PIN37_Pin GPIO_PIN_4
#define RP_PIN37_GPIO_Port GPIOD
#define RP_PIN35_Pin GPIO_PIN_5
#define RP_PIN35_GPIO_Port GPIOD
#define RP_PIN33_Pin GPIO_PIN_6
#define RP_PIN33_GPIO_Port GPIOD
#define RP_PIN29_Pin GPIO_PIN_7
#define RP_PIN29_GPIO_Port GPIOD
#define RP_PIN38_Pin GPIO_PIN_8
#define RP_PIN38_GPIO_Port GPIOB
#define RP_PIN18_Pin GPIO_PIN_9
#define RP_PIN18_GPIO_Port GPIOB
#define RP_PIN15_Pin GPIO_PIN_0
#define RP_PIN15_GPIO_Port GPIOE
#define RP_PIN16_Pin GPIO_PIN_1
#define RP_PIN16_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
