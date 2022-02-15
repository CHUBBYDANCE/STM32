/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

/* USER CODE BEGIN Private defines */
#define TIM2_Start()	do{__HAL_TIM_CLEAR_FLAG(&htim2, TIM_SR_UIF);HAL_TIM_Base_Start_IT(&htim2);} while(0)
#define TIM2_Stop()		do{HAL_TIM_Base_Stop_IT(&htim2);__HAL_TIM_SET_COUNTER(&htim2, 0);} while(0)
/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);

/* USER CODE BEGIN Prototypes */
extern uint32_t	t_rec;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
