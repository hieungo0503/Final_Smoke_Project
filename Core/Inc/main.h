/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
UART_WakeUpTypeDef WakeUpSelection;
IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim16;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#ifndef SWO_DEBUG
#define SWO_DEBUG (0)
#endif
#ifndef HW_DEBUG
#define HW_DEBUG (0)
#endif
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
void SystemClock_Config(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RI_Pin_Pin GPIO_PIN_1
#define RI_Pin_GPIO_Port GPIOA
#define button_Pin GPIO_PIN_5
#define button_GPIO_Port GPIOA
#define button_EXTI_IRQn EXTI9_5_IRQn
#define Buzzer_Pin GPIO_PIN_6
#define Buzzer_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOA
#define MCU_PSMe_Pin GPIO_PIN_0
#define MCU_PSMe_GPIO_Port GPIOB
#define Reset_Module_Pin GPIO_PIN_1
#define Reset_Module_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
