#ifndef __KEY_H
#define __KEY_H

//#include "sys.h"
#include "main.h"

//����ķ�ʽ��ͨ��λ��������ʽ��ȡIO
//#define KEY0        PEin(4) 	//KEY0����PE4
//#define KEY1        PEin(3) 	//KEY1����PE3
//#define KEY2        PEin(2)	//KEY2����PE2
//#define WK_UP       PAin(0) 	//WKUP����PA0


//����ķ�ʽ��ͨ��ֱ�Ӳ���HAL�⺯����ʽ��ȡIO
//#define KEY0        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5)  //KEY0����PC5
//#define KEY1        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) //KEY1����PA15
#define WK_UP       HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)  //WKUP����PA0


#define WKUP_PRES   1
#define ACTIVE_KEYVALUE	1

// ��������
//void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);
//uint8_t KeyScan(void);
uint8_t KeyScan(uint8_t longPress);
#endif