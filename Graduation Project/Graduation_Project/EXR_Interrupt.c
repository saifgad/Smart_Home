/*
 * EXR_Interrupt.c
 *
 * Created: 9/21/2022 12:28:09 PM
 *  Author: Dell
 */
#define F_CPU 16000000
#define Delay 60

#include "EXT_Interrupt.h"

#include "Keypad.h"

#include "LCD.h"

#include "servoo.h"

#include "LED.h"

#include "Buzzer.h"

#include "Timer.h"

#include "Functions.h"

#include <util/delay.h>

#include <avr/interrupt.h>

Uint16 eme_check = 0;
Uint8 button = NULL_KEYPAD;
extern Uint8 KP_Counter;
extern Uint8 SYS_Check;
extern Uint8 UART_GetValue;
extern Uint8 First_Check;

void INT0_Init(void) {
        /*GLobal & Peripherals Enable / Disable*/
        SREG |= Global_INT << 7;
        GICR |= Ext0_INT << 6;

        /*INT0 Pin Direction*/
        DIO_SetPin_Dir(DIO_PORTD, PIN_2, PIN_IN);

        /*EXT INT0 Trigger*/
        MCUCR |= EXT_INT0_Trigger;
}

void INT1_Init(void) {
        /*GLobal & Peripherals Enable / Disable*/
        SREG |= Global_INT << 7;
        GICR |= Ext1_INT << 7;

        /*INT0 Pin Direction*/
        DIO_SetPin_Dir(DIO_PORTD, PIN_3, PIN_IN);

        /*EXT INT0 Trigger*/
        MCUCR |= EXT_INT1_Trigger;
}

void INT2_Init(void) {
        /*GLobal & Peripherals Enable / Disable*/
        SREG |= Global_INT << 7;
        GICR |= Ext2_INT << 5;

        /*INT0 Pin Direction*/
        DIO_SetPin_Dir(DIO_PORTB, PIN_2, PIN_IN);

        /*EXT INT0 Trigger*/
        MCUCR |= EXT_INT2_Trigger;
}

ISR(INT0_vect) {
        // Write the first external interrupt handler here

        /*----------------->login to KEYPAD-LCD system <-----------------*/
        if (!eme_check) {
                Uint8 k = 0;
                Uint8 i = 0;
                Uint8 user[2][4];
                LCD_Clear();
                UART_TX_Str("\r\n");
                UART_TX_Str("The system is busy!\r\n");
                Servoo_Motor(97); //Closing the door in case the system stuck here for a while 

                /*----------------->Reciving username & password from KEYPAD <-----------------*/
                for (Uint8 j = 0; j < 3; j++) {
                        i = 0;
                        LCD_Write_String("user name:");
                        while (i < 4) {
                                button = Keypad_GetValue();

                                while (button != NULL_KEYPAD) {
                                        user[0][i] = button;
                                        LCD_Write_Char(button);
                                        button = NULL_KEYPAD;
                                        i++;
                                }
                        }
                        i = 0;
                        LCD_Goto(1, 0);
                        LCD_Write_String("password:");
                        while (i < 4) {
                                button = Keypad_GetValue();
                                while (button != NULL_KEYPAD) {
                                        user[1][i] = button;
                                        LCD_Write_Char('*');
                                        button = NULL_KEYPAD;
                                        i++;
                                }
                        }

                        /*----------------->Option for showing the passward  <-----------------*/
                        LCD_Clear();
                        LCD_Write_String("Show password *");
                        LCD_Goto(1, 0);
                        LCD_Write_String("other press #");

                        while (!(button == '*' || button == '#'))
                                button = Keypad_GetValue();

                        if (button == '*') {
                                LCD_Clear();
                                LCD_Write_String("password:");
                                i = 0;
                                while (i < 4) {
                                        LCD_Write_Char(user[1][i]);
                                        i++;
                                }
                                _delay_ms(500);

                        }
                        LCD_Clear();

                        for (Uint8 Check_Counter = 0; Check_Counter <= KP_Counter; Check_Counter += 4) {
                                /*----------------->Username Check<------------------*/

                                if (user[0][0] == EEPROM_ReadByte(0, Check_Counter + 1) && user[0][1] == EEPROM_ReadByte(0, Check_Counter + 2) && user[0][2] == EEPROM_ReadByte(0, Check_Counter + 3) && user[0][3] == EEPROM_ReadByte(0, Check_Counter + 4))

                                {
                                        /*----------------->Password Check<------------------*/

                                        if (user[1][0] == EEPROM_ReadByte(1, Check_Counter + 1) && user[1][1] == EEPROM_ReadByte(1, Check_Counter + 2) && user[1][2] == EEPROM_ReadByte(1, Check_Counter + 3) && user[1][3] == EEPROM_ReadByte(1, Check_Counter + 4))

                                        {
                                                /*----------------->right Username & Password<-----------------*/
                                                j = 4;
                                                k = 4;

                                                /*----------------->Check if there is admin uses the system to ask for accessing the KEYPAD-LCD system <-----------------*/
                                                if (SYS_Check == 0 || SYS_Check == 2) //Case of there is no one controlling yet via PC/Mobile or there is a user
                                                {
                                                        eme_check = 1; //Changing it to 1 to use the emergency case function (controlling system with KEYPAD-LCD)
                                                        LCD_Write_String("User verified");
                                                        LCD_Goto(1, 0);
                                                        LCD_Write_String("Welcome!");
                                                        Check_Counter = KP_Counter + 1; //To exit the for loop of user-pass check
                                                        _delay_ms(500);
                                                        LCD_Clear();
                                                } else if (SYS_Check == 1) //Case of Admin is controlling 
                                                {
                                                        LCD_Clear();
                                                        LCD_Write_String("Asking Admin");
                                                        _delay_ms(500);
                                                        LCD_Clear();
                                                        /*----------------->Asking for acceptance the request or Not <-----------------*/
                                                        UART_TX_Str("\r\n");
                                                        UART_TX_Str("1.Accept user request to control the system\r\n");
                                                        UART_TX_Str("2.Reject user request to control the system\r\n");
                                                        UART_TX_Str("\r\n");
                                                        UART_TX_Str("\r\n");
                                                        while (!Get_Bit(UCSRA, 7));
                                                        UART_GetValue = UDR; //Getting response from al Admin 
                                                        UART_TX_Str("\r\n");
                                                        if (UART_GetValue == '1') {
                                                                eme_check = 1;
                                                                LCD_Write_String("User verified");
                                                                LCD_Goto(1, 0);
                                                                LCD_Write_String("Welcome!");
                                                        } else if (UART_GetValue == '2') {
                                                                eme_check = 0;
                                                                LCD_Clear();
                                                                LCD_Write_String("Admin Denied");
                                                        }
                                                        Check_Counter = KP_Counter + 1;
                                                        _delay_ms(500);
                                                        LCD_Clear();
                                                        UART_GetValue = 8;
                                                }
                                        }
                                }
                        }

                        /*----------------->Wrong Username or Password<-----------------*/
                        if (k != 4) {
                                LCD_Write_String("User/Pass Wrong");
                                LCD_Goto(1, 0);
                                LCD_Write_String("Try again");
                                _delay_ms(500);
                                LCD_Clear();
                        }
                        k++;
                        /*----------------->Firing alarm in the third wrong trial of entering user and password <-----------------*/
                        if (k == 3) {
                                LCD_Clear();
                                LCD_Write_String("Firing Alarm!");

                                while (1) {
                                        Alarm_Fn();
                                }
                        }
                }

                UART_TX_Str("System online!\r\n");
                UART_TX_Str("Press any key to control the system\r\n");
                UART_TX_Str("\r\n");
                UART_TX_Str("\r\n");
        }

        /*----------------->logout from KEYPAD-LCD system <-----------------*/
        else {
                eme_check = 0;
                LCD_Clear();
        }
}