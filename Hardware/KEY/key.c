#include "key.h"
#include "delay.h"


struct KEY_t
{
 uint16_t PressCount;
 uint8_t  PressFlag;
};


//按键处理函数
//返回按键值
//mode:0,不支持连续按，连续按只返回一次值;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up=1;     //按键松开标志
    if(mode==1)key_up=1;        //支持连按
    if(key_up&&(WK_UP==ACTIVE_KEYVALUE))
    {
        delay_ms(10);
		//HAL_Delay(10);
        key_up=0;
        if(WK_UP==ACTIVE_KEYVALUE) return WKUP_PRES;          
    }else if(WK_UP!=ACTIVE_KEYVALUE)key_up=1;
    return 0;   //无按键按下
}

static struct KEY_t key1_s = {0,0};

// 放在10ms执行函数中，非阻塞方式

uint8_t KeyScan(uint8_t longPress)
{

	if (key1_s.PressFlag == 0 && WK_UP == ACTIVE_KEYVALUE) // 按下
	{
		key1_s.PressCount++;
		
		if (key1_s.PressCount >= 100 && longPress)	// 按下1000ms
		{
			key1_s.PressFlag = 1;
			return 2;
		}
		else if (key1_s.PressCount >= 5 && !longPress)	// 50ms
		{
			key1_s.PressFlag = 1;
			return 1;
		}
	}
	else if (WK_UP != ACTIVE_KEYVALUE)		// 松开
	{
		if (key1_s.PressFlag)
		{
			key1_s.PressFlag = 0;
			key1_s.PressCount = 0;
		}
		if (longPress && key1_s.PressFlag == 0 && key1_s.PressCount >= 5) // >=50ms
		{
			key1_s.PressCount = 0;
			return 1;
		}
	}
	return 0;
}
