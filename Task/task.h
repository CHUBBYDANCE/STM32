#ifndef _TASK_H_
#define _TASK_H_

/********系统中用到的头文件***********/
#include <stdint.h>

#define TICK_RATE_HZ		1000

// 定时器运行次数
#define   TIMER_ALWAYS_ALIVE      0xFFFF
#define   TIMER_ALWAYS_UNALIVE     0x0000

// 定时器状态
enum timerState
{
	TIMER_UNALIVE      = 0x00, 
	TIMER_ALIVE        = 0x01,
};

// 定时器结构体
struct timer_t
{
	uint8_t  id;					// 定时任务ID
	enum timerState flags;			// 运行标志
	uint32_t expires;				// 周期
	uint32_t expires_t;				// 周期，用来重装用
	uint16_t runs;					// 运行次数
	void *args;						// 回调函数参数
	void (*timer_cb)(void *args);	// 回调函数
	struct timer_t *next;			// 下一个定时器指针
};

// 定时器函数
uint8_t spTimerAdd(uint8_t id, enum timerState flag, uint32_t period, uint16_t runs, void (*cb)(void*), void *args);
//uint8_t spTimerStart(uint8_t timer_id,uint32_t time,uint16_t num);
//uint8_t spTimerStart(uint8_t timer_id, enum timerState flag, uint32_t time, uint16_t num);
uint8_t spTimerStart(uint8_t timer_id, uint32_t time_t, uint32_t time, uint16_t num);
uint8_t spTimerStop(uint8_t timer_id);
void spTimerTickerHandle(void);				// 放到定时器里面，频率是TICK_RATE_HZ
void spTimerScheduler(void *args);				// 放到while(1)里


// task状态
enum taskState
{
	TASK_UNALIVE		= 0x00, 
	TASK_ALIVE			= 0x01,
	TASK_ALWAYS_ALIVE	= 0xFF,
};

// task结构体
struct task_t
{
	uint8_t id;					// 任务标志编号，如第一步中添TASK_SOFT_TIMER
	enum taskState flags;		// 是否运行标志， ALWAYS_ALIVE:始终循环，UNALIVE:不运行可通过函数启动
	void *args;					// 任务运行参数
	void (*handle)(void *);	// 任务运行回调函数
	struct task_t *next;		// 下一个任务
};




// task函数
uint8_t spTaskInit(void);
uint8_t spTaskAdd(uint8_t id, enum taskState flag, void (*handle)(void *), void *args); 	// id从1开始
uint8_t spTaskSet(uint8_t task_id, enum taskState flag);
void spTaskScheduler(void);	// 放到while(1)里面


#endif // #ifndef _TASK_H_