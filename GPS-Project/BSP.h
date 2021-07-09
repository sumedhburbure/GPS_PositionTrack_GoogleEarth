#ifndef __BSP_H
#define __BSP_H
	#include "stm32f10x.h"
	
	#define LED_TOGGLE 		0
	#define LED_ON 				1
	#define LED_OFF 			2
	extern int led_status;
	extern int wifi_status;
	//function definations
	void DelayMs(unsigned long t);
	void systick_init(void);
#endif	

