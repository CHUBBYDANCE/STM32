#ifndef __KEY_H
#define __KEY_H

//#include "sys.h"
#include "main.h"

//下面的方式是通过位带操作方式读取IO
//#define KEY0        PEin(4) 	//KEY0按键PE4
//#define KEY1        PEin(3) 	//KEY1按键PE3
//#define KEY2        PEin(2)	//KEY2按键PE2
//#define WK_UP       PAin(0) 	//WKUP按键PA0


//下面的方式是通过直接操作HAL库函数方式读取IO
//#define KEY0        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5)  //KEY0按键PC5
//#define KEY1        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) //KEY1按键PA15
#define WK_UP       HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)  //WKUP按键PA0


#define WKUP_PRES   1
#define ACTIVE_KEYVALUE	1

// 函数声明
//void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);
//uint8_t KeyScan(void);
uint8_t KeyScan(uint8_t longPress);
#endif