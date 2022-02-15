#include "key.h"
#include "delay.h"


struct KEY_t
{
 uint16_t PressCount;
 uint8_t  PressFlag;
};


//����������
//���ذ���ֵ
//mode:0,��֧����������������ֻ����һ��ֵ;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up=1;     //�����ɿ���־
    if(mode==1)key_up=1;        //֧������
    if(key_up&&(WK_UP==ACTIVE_KEYVALUE))
    {
        delay_ms(10);
		//HAL_Delay(10);
        key_up=0;
        if(WK_UP==ACTIVE_KEYVALUE) return WKUP_PRES;          
    }else if(WK_UP!=ACTIVE_KEYVALUE)key_up=1;
    return 0;   //�ް�������
}

static struct KEY_t key1_s = {0,0};

// ����10msִ�к����У���������ʽ

uint8_t KeyScan(uint8_t longPress)
{

	if (key1_s.PressFlag == 0 && WK_UP == ACTIVE_KEYVALUE) // ����
	{
		key1_s.PressCount++;
		
		if (key1_s.PressCount >= 100 && longPress)	// ����1000ms
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
	else if (WK_UP != ACTIVE_KEYVALUE)		// �ɿ�
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
