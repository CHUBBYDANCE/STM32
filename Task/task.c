#include "task.h"
#include <stdlib.h>
#include "main.h"



static struct timer_t *pTimerHead;		// 定时器链表头
static struct task_t *pTaskHead;		// 任务链表头


// 添加定时器任务
uint8_t spTimerAdd(uint8_t id, enum timerState flag, uint32_t period, uint16_t runs, void (*cb)(void*), void *args)
{
	struct timer_t *node;

	// 如果没初始化表头
	if (pTimerHead == NULL)
	{
		pTimerHead = (struct timer_t *)malloc(sizeof(struct timer_t));
		if (pTimerHead == NULL)
			return -1;

		pTimerHead->id = id;
		pTimerHead->flags = flag;
		pTimerHead->runs = runs;
		pTimerHead->expires = pTimerHead->expires_t = period;
		pTimerHead->timer_cb = cb;
		pTimerHead->args = args;
		pTimerHead->next =NULL;

		return 0;
	}
	

	node = pTimerHead;

	while (node->next != NULL)
	{
		node = node->next;
	}

	node->next = (struct timer_t *)malloc(sizeof(struct timer_t));
	node = node->next;
	if (node == NULL)
		return -2;

	node->id = id;
	node->flags = flag;
	node->runs = runs;
	node->expires = node->expires_t = period;
	node->timer_cb = cb;
	node->args = args;
	node->next = NULL;

	return 0;
}

struct timer_t *getTimerIndex(uint8_t id)
{
	struct timer_t *node = pTimerHead;

	while (node != NULL)
	{
		if (node->id == id)
		{
			return node;
		}
		else
		{
			node = node->next;
		}
	}
	return NULL;
}

/*
  * time_t后，以time为周期运行num次，
  * 如果time_t == 0则立即以time为周期运行num次
*/
uint8_t spTimerStart(uint8_t timer_id, uint32_t time_t, uint32_t time, uint16_t num)
{
	double timeTmp = 0;
	struct timer_t *node = getTimerIndex(timer_id);
	if (node == NULL)
		return -1;

	timeTmp = 1000.00/TICK_RATE_HZ;
	
	node->expires_t = (uint32_t )(time/timeTmp);		// 真正的周期
	
	if(time == time_t)
	{
		node->expires = node->expires_t;
	}
	else
	{
		timeTmp = 1000.00/TICK_RATE_HZ;
		node->expires = (uint32_t )(time_t/timeTmp);	// 第一次周期
	}
	
	node->runs = num;
	
	return 0;
}

/*
uint8_t spTimerStart(uint8_t timer_id,uint32_t time,uint16_t num)
{
	double timeTmp = 0;
	struct timer_t *node = getTimerIndex(timer_id);
	if (node == NULL)
		return -1;

	timeTmp = 1000.00/TICK_RATE_HZ;
	timeTmp = time/timeTmp;
	time = (uint32_t )timeTmp;

	node->expires = node->expires_t = time;
	node->runs = num;

	return 0;
}
*/
uint8_t spTimerStop(uint8_t timer_id)
{
	struct timer_t *node = getTimerIndex(timer_id);
	if (node == NULL)
		return -1;

	node->runs = TIMER_ALWAYS_UNALIVE;
	node->flags = TIMER_UNALIVE;

	return 0;
}

// 放到定时器中
void spTimerTickerHandle(void)
{
	struct timer_t *node;

	for (node = pTimerHead; node != NULL; node = node->next)
	{
		if (node->runs != TIMER_ALWAYS_UNALIVE)
		{
			if (node->expires != 0)
			{
				--(node->expires);
			}
			if (node->expires == 0) 
			//else
			{
				//node->expires = 0;
				node->flags = TIMER_ALIVE;
			}
		}
	}
}

// 放到while(1)里
void spTimerScheduler(void *args)
{
	struct timer_t *node;

	for (node = pTimerHead; node != NULL; node = node->next)
	{
		if (node->flags == TIMER_ALIVE)
		{
			if (node-> runs == TIMER_ALWAYS_UNALIVE)
			{
				node->flags = TIMER_UNALIVE;
				node->expires = 0;
				continue;
			}
			else
			{
				if (node->runs != TIMER_ALWAYS_ALIVE)
					node->runs--;
				node->timer_cb(node->args);			// 执行回调函数
				node->expires = node->expires_t;		// 重载周期
				node->flags = TIMER_UNALIVE;			// 运行标志
			}
		}
	}
}


/*************************任务*******************************/

uint8_t spTaskInit(void)
{
	// 初始化任务
	if (pTaskHead != NULL)
		return -1;
	pTaskHead = (struct task_t *)malloc(sizeof(struct task_t));
	if (pTaskHead == NULL)
		return -2;


	// 初始化任务头节点
	pTaskHead->next = NULL;
	pTaskHead->args = NULL;
	pTaskHead->id = 0;
	pTaskHead->flags = TASK_ALWAYS_ALIVE;
	pTaskHead->handle = spTimerScheduler;

	return 0;
}

uint8_t spTaskAdd(uint8_t id, enum taskState flag, void (*handle)(void *), void *args)
{
	struct task_t *node = pTaskHead;

	if (node == NULL)
		return -1;

	while (node->next != NULL)
	{
		node = node->next;
	}

	//for (node = pTaskHead; node->next != NULL; )
	//	node = node->next;

	node->next = (struct task_t *)malloc(sizeof(struct task_t));
	node = node->next;
	if (node == NULL)
		return -2;

	node->id = id;
	node->flags = flag;
	node->handle = handle;
	node->args = args;
	node->next = NULL;

	return 0;
}

uint8_t spTaskSet(uint8_t task_id, enum taskState flag)
{
	struct task_t *node = pTaskHead;

	if (node == NULL)
		return -1;

	while (node != NULL)
	{
		if (node->id == task_id)
		{
			break;
		}
		node = node->next;
	}
	if (node == NULL)		// task id not found
	{
		return -2;
	}

	node->flags = flag;

	return 0;
}

void spTaskScheduler(void)
{
	struct task_t *node;

	for (node = pTaskHead; node != NULL; node = node->next)
	{
		if (node->flags == TASK_ALIVE)
		{
			node->handle(node->args);
			node->flags = TASK_UNALIVE;
		}
		else if (node->flags == TASK_ALWAYS_ALIVE)
		{
			node->handle(node->args);
		}
	}
}