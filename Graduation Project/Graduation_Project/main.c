// All the preprocessor Libaries and Macros
#define  F_CPU 16000000
#include <util/delay.h>
#include "ADC.h"
#include "DCM.h"
#include "Buzzer.h"
#include "DC_Motor.h"
#include "EEPROM.h"
#include "EXT_Interrupt.h"
#include "Keypad.h"
#include "LCD.h"
#include "LED.h"
#include "Relay.h"
#include "Timer.h"
#include "servoo.h"
#include "UART.h"
#include "emergency_case.h"
// Extern the values of keypad and PC counters
extern Uint8 KP_Counter;
extern Uint8 PC_Counter;
extern Uint8 AD_Counter;
extern Uint8 ans_check;
extern Uint16 eme_check;

int main(void)
{	

    // Initialization of Modules and peripherals
	ADC_Init();
	Buzzer_Init();
	EEPROM_INIT();
	INT0_Init();
	Keypad_Init();
	LCD_Init();
	LED0_Init();
	LED1_Init();
	LED2_Init();
	LED3_Init();
	LED4_Init();
	Servo_Init();
	Timer0_Init();
	Timer0_Start();
	PWM0_Gen(0);
	Timer2_Init();
	UART_Init();
	DC_Motor1_INIT();
		
	// Try not take initialized values as actual values for counters
	if(EEPROM_ReadByte(4,0) != 255)
	{
		KP_Counter = EEPROM_ReadByte(4,0); // Getting keypad counter value
	}
	else
	{
		KP_Counter = 0;	// Case of EEPROM initial value taking 0 value for Keypad counter
	}	
	
	if(EEPROM_ReadByte(4,1) != 255)		
	{
			PC_Counter = EEPROM_ReadByte(4,1); // Getting UART user counter value
	}
	else
	{
		PC_Counter = 0; // Case of EEPROM initial value taking 0 value for UART user counter
	}
	
	if(EEPROM_ReadByte(4,2) != 255)
	{
		AD_Counter = EEPROM_ReadByte(4,2);	// Getting UART admin counter value
	}
	else
	{
		AD_Counter = 4;	// Case of EEPROM initial value taking 4 value for UART admin counter
	}
	
	/*Declaring fixed admin username & password*/
	for(Uint8 Counter = 1; Counter < 5; Counter++)
		{
			EEPROM_WriteByte(5,Counter,'1'); // Admin fixed username assigned in EEPROM with 1111 
			EEPROM_WriteByte(6,Counter,'1'); // Admin fixed password assigned in EEPROM with 1111
		}
	
	// First time Admin enter
	UART_TX_Str("\r\n");
	UART_TX_Str("Welcome to the smart home\r\n");
	UART_TX_Str("\r\n");
	UART_TX_Str("1.Enter Admin\r\n");
	UART_TX_Str("2.Enter User\r\n");
	UART_TX_Str("\r\n");
	
	// Using timer(2) for clearing LCD without polling
	Timer2_Start();
	Timer2_SetDelay(5);
	
	// ADC pin that sensing the value of the temperature sensor
	ADC_Read(ADC_PIN7);
		
	while (1)
	
	{	
		/*Emergency case system*/	
		while (eme_check)
		{
			emergency_case(); // Function that contain all the emergency case functions
		}
		
	}
}
