#include "led.h"


void LED_Toggle(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void LED_off(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void LED_on(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}