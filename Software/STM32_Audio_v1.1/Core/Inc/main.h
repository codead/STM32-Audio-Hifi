/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

  /* Define LCD PinOut */
#define ST7789_RST_GPIO_Port	GPIOC
#define ST7789_RST_Pin			GPIO_PIN_13
#define ST7789_DC_GPIO_Port		GPIOC
#define ST7789_DC_Pin			GPIO_PIN_15
#define ST7789_CS_GPIO_Port		GPIOA
#define ST7789_CS_Pin			GPIO_PIN_0
#define ST7789_BL_GPIO_Port  GPIOC
#define ST7789_BL_Pin      GPIO_PIN_14
  /* Define SX1509B PinOut */
#define SX1509B_GPIO_Port  GPIOA
#define SX1509B_RST_Pin    GPIO_PIN_1
#define SX1509B_INT_Pin    GPIO_PIN_2
  /* Define Flash PinOut */
#define W25Qxx_CS_GPIO_Port GPIOA
#define W25Qxx_CS_Pin GPIO_PIN_4
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Volume_Pin GPIO_PIN_3
#define Volume_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_4
#define FLASH_CS_GPIO_Port GPIOA
#define BAT_CHARGE_Pin GPIO_PIN_13
#define BAT_CHARGE_GPIO_Port GPIOB
#define BAT_CHARGE_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
