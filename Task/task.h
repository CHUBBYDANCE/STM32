#ifndef _TASK_H_
#define _TASK_H_

/********ϵͳ���õ���ͷ�ļ�***********/
#include <stdint.h>

#define TICK_RATE_HZ		1000

// ��ʱ�����д���
#define   TIMER_ALWAYS_ALIVE      0xFFFF
#define   TIMER_ALWAYS_UNALIVE     0x0000

// ��ʱ��״̬
enum timerState
{
	TIMER_UNALIVE      = 0x00, 
	TIMER_ALIVE        = 0x01,
};

// ��ʱ���ṹ��
struct timer_t
{
	uint8_t  id;					// ��ʱ����ID
	enum timerState flags;			// ���б�־
	uint32_t expires;				// ����
	uint32_t expires_t;				// ���ڣ�������װ��
	uint16_t runs;					// ���д���
	void *args;						// �ص���������
	void (*timer_cb)(void *args);	// �ص�����
	struct timer_t *next;			// ��һ����ʱ��ָ��
};

// ��ʱ������
uint8_t spTimerAdd(uint8_t id, enum timerState flag, uint32_t period, uint16_t runs, void (*cb)(void*), void *args);
//uint8_t spTimerStart(uint8_t timer_id,uint32_t time,uint16_t num);
//uint8_t spTimerStart(uint8_t timer_id, enum timerState flag, uint32_t time, uint16_t num);
uint8_t spTimerStart(uint8_t timer_id, uint32_t time_t, uint32_t time, uint16_t num);
uint8_t spTimerStop(uint8_t timer_id);
void spTimerTickerHandle(void);				// �ŵ���ʱ�����棬Ƶ����TICK_RATE_HZ
void spTimerScheduler(void *args);				// �ŵ�while(1)��


// task״̬
enum taskState
{
	TASK_UNALIVE		= 0x00, 
	TASK_ALIVE			= 0x01,
	TASK_ALWAYS_ALIVE	= 0xFF,
};

// task�ṹ��
struct task_t
{
	uint8_t id;					// �����־��ţ����һ������TASK_SOFT_TIMER
	enum taskState flags;		// �Ƿ����б�־�� ALWAYS_ALIVE:ʼ��ѭ����UNALIVE:�����п�ͨ����������
	void *args;					// �������в���
	void (*handle)(void *);	// �������лص�����
	struct task_t *next;		// ��һ������
};




// task����
uint8_t spTaskInit(void);
uint8_t spTaskAdd(uint8_t id, enum taskState flag, void (*handle)(void *), void *args); 	// id��1��ʼ
uint8_t spTaskSet(uint8_t task_id, enum taskState flag);
void spTaskScheduler(void);	// �ŵ�while(1)����


#endif // #ifndef _TASK_H_