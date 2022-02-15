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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "led.h"
#include "key.h"
#include "task.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SPI_HandleTypeDef hspi1;

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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
#define KEY_EXTI_IRQn EXTI0_IRQn
#define SX126x_DIO1_Pin GPIO_PIN_1
#define SX126x_DIO1_GPIO_Port GPIOA
#define SX126x_DIO1_EXTI_IRQn EXTI1_IRQn
#define SX126x_NSS_Pin GPIO_PIN_4
#define SX126x_NSS_GPIO_Port GPIOA
#define SX126x_RST_Pin GPIO_PIN_11
#define SX126x_RST_GPIO_Port GPIOA
#define SX126x_BUSY_Pin GPIO_PIN_12
#define SX126x_BUSY_GPIO_Port GPIOA
#define TXW_I_Pin GPIO_PIN_7
#define TXW_I_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define USART_REC_LEN  			200  		//定义�?大接收字节数 200
#define EN_USART1_RX 			  1			  //使能�?1�?/禁止�?0）串�?1接收
#define RXBUFFERSIZE        1 			//缓存大小

#define TIMER_LED		0
#define TIMER_KEY		1
#define TIMER_CAD		2
#define TIMER_ADXL_MES	3
#define TIMER_ADXL_PWR	4
#define TIMER_LORA_SND	5
#define TIMER_BAT		6
#define TIMER_LP		7
#define TIMER_TXW		8
#define TIMER_RXW		9
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
