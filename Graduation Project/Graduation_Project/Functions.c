/*
 * Functions.c
 *
 * Created: 11/4/2022 2:01:36 PM
 *  Author: dell
 */ 
#include "Functions.h"
#define Delay  60
#include <util/delay.h>
void Alarm_Fn(void){
	
	Buzzer_ON();
	LED3_ON();
	LED4_ON();
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	_delay_ms(Delay);
	LED3_ON();
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	_delay_ms(Delay);
	
	
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	LED2_ON();
	_delay_ms(Delay);
	
	LED0_OFF();
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_OFF();
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	LED2_ON();
	_delay_ms(Delay);
	
	LED0_OFF();
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	Buzzer_OFF();
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	_delay_ms(Delay);
	
	
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	LED2_ON();
	_delay_ms(Delay);
	
	LED0_OFF();
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	LED2_ON();
	_delay_ms(Delay);
	
	LED0_OFF();
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	LED0_ON();
	_delay_ms(Delay);
	LED0_OFF();
	LED2_ON();
	_delay_ms(Delay);
	LED2_OFF();
	_delay_ms(Delay);
	
	
	
	
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(200);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(500);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(100);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(10);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(200);
	LED0_Toggle();
	LED2_Toggle();
	LED3_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(100);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(Delay);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(1000);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(10);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(10);
	LED2_Toggle();
	LED3_Toggle();
	LED0_Toggle();
	LED1_Toggle();
	LED4_Toggle();
	Buzzer_Toggle();
	_delay_ms(100);
	
	LED0_Toggle();
	Buzzer_ON();
	_delay_ms(250);
	LED0_Toggle();
	Buzzer_Toggle();
	_delay_ms(250);
	LED0_Toggle();
	Buzzer_ON();
	_delay_ms(Delay);
	Buzzer_OFF();
	
	
}