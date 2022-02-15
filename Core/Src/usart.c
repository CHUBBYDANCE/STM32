/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "sys.h"
#include "main.h" 
#include "stdio.h"

extern uint8_t  USART_RX_BUF[USART_REC_LEN]; 	//接收缓冲,�?大USART_REC_LEN个字�?.末字节为换行�? 
extern uint16_t USART_RX_STA;         			//接收状�?�标�?	
extern uint8_t aRxBuffer[RXBUFFERSIZE];			//HAL库USART接收Buffer
extern UART_HandleTypeDef huart1;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//加入以下代码,支持printf函数,而不�?要�?�择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发�??,直到发�?�完�?   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


/*=======================================================================================================
//串口接收中断函数处理
void USART1_IRQHandler(void)
{

	uint32_t timeout=0;

  	HAL_UART_IRQHandler(&huart1); 

//中断超时处理
  timeout=0;
    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) //get state of uart,keep loop until ready.
	{
	 timeout++;
     if(timeout - HAL_MAX_DELAY > 0) break;
	}

	timeout=0;
	while(HAL_UART_Receive_IT(&huart1, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) 
	{
	 timeout++;
	 if(timeout - HAL_MAX_DELAY > 0) break;
	}
//中断完成后打�?
   if(USART_RX_STA&0x8000)
		{ 
			// len=USART_RX_STA&0x3fff;  

			printf("\r\nyour data is as the following\r\n");
			
			HAL_UART_Transmit(&huart1,(uint8_t*)USART_RX_BUF,USART_RX_STA&0x3fff,1000);
			
			while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET);		
			
			printf("\r\n");
			
			USART_RX_STA=0;
		}
}
========================================================================================================*/


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//check whether is USART1
	{
		if((USART_RX_STA&0x8000)==0)//receive is in the progress
		{
			if(USART_RX_STA&0x4000)//received 0X0D 
			{
				if(aRxBuffer[0]!=0x0a) USART_RX_STA=0;//error and restart \r\n (continue to receive)
				else USART_RX_STA|=0x8000;	//receiving complecated \r\n(0X0D 0X0A)
			}
			else //do not receive 0X0D
			{
				if(aRxBuffer[0]==0x0d)USART_RX_STA|=0x4000; //set bit14 flag
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=aRxBuffer[0]; //buffer the input in an array once receive interrupt finished 
					USART_RX_STA++;//update count flags
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//error to restart
				}		 
			}
		}

	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
