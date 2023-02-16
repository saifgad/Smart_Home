/*
 * UART.c
 *
 * Created: 9/29/2022 1:02:18 PM
 *  Author: Dell
 */
#include "UART.h"

#include "servoo.h"

#include "LED.h"

#include "LCD.h"

#include "Buzzer.h"

#include "Relay.h"

#include "Timer.h"

// All the preprocessor Libaries and Macros
#include <avr/interrupt.h>

#include <util/delay.h>

#define Delay 60

// Extern global values  
extern Uint16 Temp; // extern temp value from ADC driver after taking value from temperature sensor
extern Uint8 eme_check; // extern eme_check value to determine whether there is a keypad user assigned the system or not

// Initialization global variables and arrays
Uint8 UART_GetValue = 0;
Uint8 SYS_Check = 0;
Uint8 A_U_Check = 0;
Uint8 CMD_Check = 0;
Uint8 KP_Counter = 0;
Uint8 PC_Counter = 0;
Uint8 AD_Counter = 4;
Uint8 Dimmer_Array[3] = {
        0,
        0,
        0
};
f32 Dimmer_Value = 0;
Uint8 Temp_Display = 0;
Uint8 First_Check = 0;

void UART_Init(void) {
        // Global Interrupt
        SREG |= Global_INT << 7;

        // Peripheral Interrupt
        UCSRB = (UART_TX_INT << 6) | (UART_RX_INT << 7) | (UCSRB & 0X3F);

        // TX Pin Direction
        DIO_SetPin_Dir(DIO_PORTD, PIN_1, PIN_OUT);

        // RX Pin Direction
        DIO_SetPin_Dir(DIO_PORTD, PIN_0, PIN_IN);

        // UART Receiver/ Transmitter Enable
        UCSRB = (1 << 3) | (1 << 4) | (UCSRB & 0XE7);

        // UART Doubled Speed Mode
        UCSRA = (UART_Doubled_Speed << 1) | (UCSRA & 0XFD);

        // Parity Bit Mode
        // Data Bits Mode
        // Stop Bit  Mode
        UCSRC = (1 << 7) | (UART_Data_Bits << 1) | (UART_Stop_Bits << 3) | (UART_Parity_Mode << 4) | (UCSRC & 0X41);

        // UART Baud Rate
        UBRRL = (Uint8) UART_Baud_Rate;
        UBRRH = (UART_Baud_Rate >> 8);
}

void UART_TX_Char(Uint8 data) {
        // Sending Data
        UDR = data;

        // Checking the data is sent
        while (!Get_Bit(UCSRA, 5));
}

void UART_TX_Str(Uint8 * str) {
        for (Uint8 i = 0; str[i] != '\n'; i++) {
                UART_TX_Char(str[i]);
        }
}

Uint8 UART_RX_Char(void) {
        // Receiving data 
        Uint8 Value = 0;

        // Checking the data is received 
        while (!Get_Bit(UCSRA, 5));

        // Storing the data received in a variable
        Value = UDR;
        return Value;
}

ISR(USART_RXC_vect) {
        // Write UART receive handler here
        /*---------> Where all the UART system work <---------*/

        // Initialization of local arrays used for taking username and password of any user
        Uint8 User[4] = {
                0
        };
        Uint8 Pass[4] = {
                0
        };

        // if the there is no keypad user start the timer that clear LCD if there is keypad user don't clear the LCD 
        if (!eme_check)
                Timer2_Start();

        // For login the system where SYS_Check value is 0 	
        if (!SYS_Check) {
                while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                A_U_Check = UDR; // Receiving the character 

                // 3 times for login if the user exceed 3 times the alarm will fire
                for (Uint8 i = 0; i < 3; i++) {
                        // Login as admin
                        if (A_U_Check == '1') {
                                // Admin enter the system
                                /*------------->Entering Username<-------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter your username\r\n");
                                UART_TX_Str("\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                }

                                /*------------->Entering Password<-------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter your password\r\n");
                                UART_TX_Str("\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        Pass[i] = UART_RX_Char(); // Receiving the character in password array
                                }

                                /*---------------> Checking Username & Password<---------------*/

                                for (Uint8 Check_Counter = 0; Check_Counter <= AD_Counter; Check_Counter += 4) {
                                        /*----------------->Username Check<------------------*/
                                        if (User[0] == EEPROM_ReadByte(5, Check_Counter + 1) && User[1] == EEPROM_ReadByte(5, Check_Counter + 2) && User[2] == EEPROM_ReadByte(5, Check_Counter + 3) && User[3] == EEPROM_ReadByte(5, Check_Counter + 4)) {
                                                /*----------------->Password Check<------------------*/

                                                if (Pass[0] == EEPROM_ReadByte(6, Check_Counter + 1) && Pass[1] == EEPROM_ReadByte(6, Check_Counter + 2) && Pass[2] == EEPROM_ReadByte(6, Check_Counter + 3) && Pass[3] == EEPROM_ReadByte(6, Check_Counter + 4)) {
                                                        /*----------------->right Username & Password<-----------------*/
                                                        UART_TX_Str("\r\n");
                                                        UART_TX_Str("\r\n");
                                                        UART_TX_Str("Admin exist, Welcome!\r\n");
                                                        UART_TX_Str("\r\n");
                                                        Check_Counter = AD_Counter + 1; // Exiting the Check for loop
                                                        SYS_Check = 1; // Means that admin entered the system (Admin = 1) 
                                                        CMD_Check = 1; // *Not Important to Know* it used to not take UART_Get value variable after login the system as 0 value
                                                        i = 5; // Exiting the Alarm for loop  
                                                }
                                        }
                                }

                                /*----------------->Wrong Username or Password<-----------------*/
                                if (!SYS_Check) {
                                        UART_TX_Str("\r\n");
                                        UART_TX_Str("\r\n");
                                        UART_TX_Str("Admin does not exist. Please try again\r\n");
                                        UART_TX_Str("\r\n");
                                }

                        }
                        // Login as UART user
                        else if (A_U_Check == '2') {
                                // PC user enter the system
                                /*------------->Entering Username<-------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter your username\r\n");
                                UART_TX_Str("\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                }

                                /*------------->Entering Password<-------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter your password\r\n");
                                UART_TX_Str("\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        Pass[i] = UART_RX_Char(); // Receiving the character in password array
                                }

                                /*---------------> Checking Username & Password<---------------*/
                                for (Uint8 Check_Counter = 0; Check_Counter <= PC_Counter; Check_Counter += 4) {
                                        /*----------------->Username Check<------------------*/
                                        if (User[0] == EEPROM_ReadByte(2, Check_Counter + 1) && User[1] == EEPROM_ReadByte(2, Check_Counter + 2) && User[2] == EEPROM_ReadByte(2, Check_Counter + 3) && User[3] == EEPROM_ReadByte(2, Check_Counter + 4)) {
                                                /*----------------->Password Check<------------------*/
                                                if (Pass[0] == EEPROM_ReadByte(3, Check_Counter + 1) && Pass[1] == EEPROM_ReadByte(3, Check_Counter + 2) && Pass[2] == EEPROM_ReadByte(3, Check_Counter + 3) && Pass[3] == EEPROM_ReadByte(3, Check_Counter + 4)) {
                                                        /*----------------->right Username & Password<-----------------*/
                                                        UART_TX_Str("\r\n");
                                                        UART_TX_Str("\r\n");
                                                        UART_TX_Str("User exist, Welcome!\r\n");
                                                        UART_TX_Str("\r\n");
                                                        Check_Counter = PC_Counter + 1; // Exiting the Check for loop
                                                        SYS_Check = 2; // Means that UART user entered the system (PC user = 1)
                                                        CMD_Check = 1; // *Not Important to Know* it used to not take UART_Get value variable after login the system as 0 value
                                                        i = 5; // Exiting the Alarm for loop
                                                }
                                        }
                                }

                                /*----------------->Wrong Username or Password<-----------------*/
                                if (!SYS_Check) {
                                        UART_TX_Str("\r\n");
                                        UART_TX_Str("\r\n");
                                        UART_TX_Str("User does not exist. Please try again\r\n");
                                        UART_TX_Str("\r\n");
                                }
                        }
                        // If user entered wrong command
                        else {
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter command number\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("1.Enter Admin\r\n");
                                UART_TX_Str("2.Enter User\r\n");
                                UART_TX_Str("\r\n");
                                i--;
                                while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                A_U_Check = UDR; // Receiving the character
                        }

                        // Case of firing alarm (3 times wrong username/password) 
                        if (i == 2) {
                                LCD_Clear();
                                LCD_Write_String("Firing Alarm!");

                                while (1) {
                                        Alarm_Fn(); // Interrupt function that polling the system and does not exit except with reset pin
                                }
                        }
                }
        }

        /*----------> Admin Loged-in <----------*/
        if (SYS_Check == 1) {
                // *Not Important to Know* it used to not take UART_Get value variable after login the system as 0 value (do nothing after entering the system)
                if (!CMD_Check) {
                        UART_GetValue = UDR;
                } else {
                        UART_GetValue = 8;
                        CMD_Check = 0;
                }

                if (UART_GetValue == '1') {
                        // Open the door
                        Servoo_Motor(316); // 316 gives 90 degree servo rotation 
                        // If there is no keypad user, using LCD to write the operating device
                        if (!eme_check) {
                                LCD_Clear();
                                LCD_Write_String("Door Opened!");
                                Timer2_SetDelay(2000); // Clearing LCD after 2 seconds using timer 2
                        }
                } else if (UART_GetValue == '2') {
                        // Close the door
                        Servoo_Motor(97); // 97 gives 0 degree servo
                        // If there is no keypad user, using LCD to write the operating device
                        if (!eme_check) {
                                LCD_Clear();
                                LCD_Write_String("Door Closed!");
                                Timer2_SetDelay(2000); // Clearing LCD after 2 seconds using timer 2
                        }
                } else if (UART_GetValue == '3') {
                        // Open lamps
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Which lamp do you want to be opened(From 1 to 5)\r\n");
                        UART_TX_Str("6.All Lamps\r\n");
                        UART_TX_Str("\r\n");

                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                        UART_GetValue = UART_RX_Char(); // Receiving the command

                        // Open first lamp
                        if (UART_GetValue == '1') {
                                LED0_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(1) Opened");
                                }
                        }
                        // Open second lamp
                        else if (UART_GetValue == '2') {
                                LED1_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(2) Opened");
                                }
                        }
                        // Open third lamp			
                        else if (UART_GetValue == '3') {
                                LED2_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(3) Opened");
                                }
                        }
                        // Open fourth lamp
                        else if (UART_GetValue == '4') {
                                LED3_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(4) Opened");
                                }
                        }
                        // Open fifth lamp
                        else if (UART_GetValue == '5') {
                                LED4_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(5) Opened");
                                }
                        }
                        // Open all lamps
                        else if (UART_GetValue == '6') {
                                LED0_ON();
                                LED1_ON();
                                LED2_ON();
                                LED3_ON();
                                LED4_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("All Lamps Are");
                                        LCD_Goto(1, 0);
                                        LCD_Write_String("Opened");
                                }
                        }
                        // If there is no keypad user, using LCD to write & clear the operating device
                        if (!eme_check)
                                Timer2_SetDelay(2000); // Clearing LCD after 2 seconds using timer 2
                } else if (UART_GetValue == '4') {
                        // Close lamps
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Which lamp do you want to be Closed(From 1 to 5)\r\n");
                        UART_TX_Str("6.All Lamps\r\n");
                        UART_TX_Str("\r\n");

                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                        UART_GetValue = UART_RX_Char(); // Receiving the command

                        // CLose first lamp
                        if (UART_GetValue == '1') {
                                LED0_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(1) Closed");
                                }
                        }
                        // Close second lamp
                        else if (UART_GetValue == '2') {
                                LED1_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(2) Closed");
                                }
                        }
                        // Close third lamp
                        else if (UART_GetValue == '3') {
                                LED2_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(3) Closed");
                                }
                        }
                        // Close fourth lamp
                        else if (UART_GetValue == '4') {
                                LED3_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(4) Closed");
                                }
                        }
                        // Close fifth lamp
                        else if (UART_GetValue == '5') {
                                LED4_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(5) Closed");
                                }
                        }
                        // Close all lamps
                        else if (UART_GetValue == '6') {
                                LED0_OFF();
                                LED1_OFF();
                                LED2_OFF();
                                LED3_OFF();
                                LED4_OFF();
                                LCD_Clear();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Write_String("All Lamps Are");
                                        LCD_Goto(1, 0);
                                        LCD_Write_String("Closed");
                                }
                        }
                        // If there is no keypad user, using LCD to write & clear the operating device
                        if (!eme_check)
                                Timer2_SetDelay(2000); // Clearing LCD after 2 seconds using timer 2
                } else if (UART_GetValue == '5') {
                        /*Dimmer circuit*/
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Please enter the percentage of the lamp\r\n");
                        UART_TX_Str("\r\n");

                        Dimmer_Value = 0; // making sure that dimmer value equal 0

                        // Taking the dimmer value as characters 
                        for (Uint8 i = 0; i < 3; i++) {
                                while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                Dimmer_Array[i] = UART_RX_Char(); // Receiving dimmer character values

                                // Converting Dimmer value from characters to integer to send it as duty cycle to timer(0) 
                                if (Dimmer_Array[i] >= 48 && Dimmer_Array[i] <= 57) {
                                        Dimmer_Value = Dimmer_Value * 10 + (Dimmer_Array[i] - 48);
                                }
                                // if the value of character not from 48 to 57 (which is the number characters in ASCII table) these means that user entered the desired value and want to  escape other values 
                                else {
                                        i = 5; // Exiting the for loop in which user enter the character values of dimmer circuit
                                }

                        }

                        // If there is no keypad user, using LCD to write the operating device
                        if (!eme_check) {
                                LCD_Clear();
                                LCD_Write_String("Lamp ON!");
                                LCD_Goto(1, 0);
                                LCD_Write_String("Percentage:");
                                LCD_Write_Integer(Dimmer_Value);
                                LCD_Write_Char('%');
                                Timer2_SetDelay(2000);
                        }

                        // To avoid twitch of dimmer lamp
                        if (Dimmer_Value >= 58 && Dimmer_Value <= 60) {
                                Dimmer_Value = 55;
                        } else if (Dimmer_Value > 60 && Dimmer_Value <= 62) {
                                Dimmer_Value = 65;
                        }

                        PWM0_Gen(Dimmer_Value); // Sending dimmer as duty cycle to timer(0)
                } else if (UART_GetValue == '6') {
                        /*--------------->Display temperature<----------------*/
                        // Declaring temperature array which take the temperature value from temperature integer that take its value from the temperature
                        Uint8 Temp_array[2] = {
                                0
                        };
                        Uint8 Temp_Divide = Temp; // Saving the value in other variable to do some operations

                        // Converting the integer value to characters and saved in the temperature array
                        for (Uint8 i = 0; i < 2; i++) {
                                Temp_array[i] = Temp_Divide % 10;
                                Temp_Divide /= 10;
                        }

                        // Display the temperature
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Temperature:\r\n");
                        UART_TX_Char(Temp_array[1] + 48);
                        UART_TX_Char(Temp_array[0] + 48);
                        UART_TX_Str("\r\n");

                        // If there is no keypad user, using LCD to write the operating device
                        if (!eme_check) {
                                LCD_Clear();
                                LCD_Write_String("Temperature:");
                                LCD_Write_Integer(Temp);
                                Timer2_SetDelay(2000);
                        }
                } else if (UART_GetValue == '7') {
                        // Add new user

                        /*-------------->Determine The Type of User<---------------*/
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("1.Keypad user\r\n");
                        UART_TX_Str("2.PC user\r\n");
                        UART_TX_Str("3.AD user\r\n");

                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                        UART_GetValue = UART_RX_Char(); // Receiving the command

                        if (UART_GetValue == '1') {
                                /*--------------->Adding Keypad User<-------------------*/
                                Uint8 j = 0;

                                /*----------------->Adding New Username<-----------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter 4 numbers as username\r\n");
                                for (Uint8 i = 0; i < 4; i++) {
                                        j = i + 1;
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                        EEPROM_WriteByte(0, j += KP_Counter, User[i]); // Adding the username in EEPROM
                                }

                                /*----------------->Adding New Password<-----------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter 4 numbers as password\r\n");
                                for (Uint8 i = 0; i < 4; i++) {
                                        j = i + 1;
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        Pass[i] = UART_RX_Char(); // Receiving the character in password array
                                        EEPROM_WriteByte(1, j += KP_Counter, Pass[i]); // Adding the password in EEPROM
                                }

                                KP_Counter += 4; // Counter to determine the number of keypad users
                                EEPROM_WriteByte(4, 0, KP_Counter); // Saving the counter in EEPROM
                        } else if (UART_GetValue == '2') {
                                /*--------------->Adding PC User<-------------------*/
                                Uint8 j = 0;

                                /*----------------->Adding New Username<-----------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter 4 numbers as username\r\n");
                                for (Uint8 i = 0; i < 4; i++) {
                                        j = i + 1;
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                        EEPROM_WriteByte(2, j += PC_Counter, User[i]); // Adding the username in EEPROM
                                }

                                /*----------------->Adding New Password<-----------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter 4 numbers as password\r\n");
                                for (Uint8 i = 0; i < 4; i++) {
                                        j = i + 1;
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        Pass[i] = UART_RX_Char(); // Receiving the character in password array
                                        EEPROM_WriteByte(3, j += PC_Counter, Pass[i]); // Adding the password in EEPROM
                                }

                                PC_Counter += 4; // Counter to determine the number of PC users
                                EEPROM_WriteByte(4, 1, PC_Counter); // Saving the counter in EEPROM
                        } else if (UART_GetValue == '3') {
                                /*--------------->Adding Admin User<-------------------*/
                                Uint8 j = 0;

                                /*----------------->Adding New Username<-----------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter 4 numbers as username\r\n");
                                for (Uint8 i = 0; i < 4; i++) {
                                        j = i + 1;
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                        EEPROM_WriteByte(5, j += AD_Counter, User[i]); // Adding the username in EEPROM
                                }

                                /*----------------->Adding New Password<-----------------*/
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter 4 numbers as password\r\n");
                                for (Uint8 i = 0; i < 4; i++) {
                                        j = i + 1;
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        Pass[i] = UART_RX_Char(); // Receiving the character in password array
                                        EEPROM_WriteByte(6, j += AD_Counter, Pass[i]); // Adding the password in EEPROM
                                }

                                AD_Counter += 4; // Counter to determine the number of PC users
                                EEPROM_WriteByte(4, 2, AD_Counter); // Saving the counter in EEPROM
                        }

                        /*------------>Added Successfully<-------------------*/
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("User added successfully\r\n");
                } else if (UART_GetValue == '8') {
                        // Delete an exist user
                        Uint8 j = 0;
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Do you want to delete keypad user , PC user or Admin user\r\n");
                        UART_TX_Str("1.Keypad User\r\n");
                        UART_TX_Str("2.PC User\r\n");
                        UART_TX_Str("3.AD User\r\n");

                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                        UART_GetValue = UART_RX_Char(); // Receiving the command

                        /*--------------->Deleting Keypad User<-------------------*/
                        if (UART_GetValue == '1') {
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter the KP username\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                }

                                /*---------------->Seaching For The User Want To Be Deleted<---------------------*/
                                for (Uint8 Check_Counter = 0; Check_Counter <= KP_Counter; Check_Counter += 4) {
                                        /*----------------->Username Check<------------------*/
                                        if (User[0] == EEPROM_ReadByte(0, Check_Counter + 1) && User[1] == EEPROM_ReadByte(0, Check_Counter + 2) && User[2] == EEPROM_ReadByte(0, Check_Counter + 3) && User[3] == EEPROM_ReadByte(0, Check_Counter + 4)) {
                                                /*----------------->Exist  Username<-----------------*/
                                                // Shifting username/password characters and overwrite on the desired user to be deleted
                                                for (Uint8 Check_Counter_2 = Check_Counter + 1; Check_Counter_2 <= KP_Counter; Check_Counter_2++) {
                                                        EEPROM_WriteByte(0, Check_Counter_2, EEPROM_ReadByte(0, Check_Counter_2 + 4)); // Overwrite on the 4 characters of username in EEPROM Then shift all users place backward by shifting each character 4 places backward 
                                                        EEPROM_WriteByte(1, Check_Counter_2, EEPROM_ReadByte(1, Check_Counter_2 + 4)); // Overwrite on the 4 characters of password in EEPROM Then shift all users place backward by shifting each character 4 places backward
                                                }

                                                KP_Counter -= 4; // Reduce the number of users in counter
                                                EEPROM_WriteByte(4, 0, KP_Counter); // Saving the counter
                                                Check_Counter = KP_Counter + 1; // Exiting the check for loop
                                                UART_TX_Str("\r\n");
                                                UART_TX_Str("\r\n");
                                                UART_TX_Str("User deleted successfully\r\n");
                                                j = 1; // To determine whether we find the user or not
                                        }
                                }
                        }
                        /*--------------->Deleting PC User<-------------------*/
                        else if (UART_GetValue == '2') {
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter the PC username\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                }

                                /*---------------->Seaching For The User Want To Be Deleted<---------------------*/
                                for (Uint8 Check_Counter = 0; Check_Counter <= PC_Counter; Check_Counter += 4) {
                                        /*----------------->Username Check<------------------*/
                                        if (User[0] == EEPROM_ReadByte(2, Check_Counter + 1) && User[1] == EEPROM_ReadByte(2, Check_Counter + 2) && User[2] == EEPROM_ReadByte(2, Check_Counter + 3) && User[3] == EEPROM_ReadByte(2, Check_Counter + 4)) {
                                                /*----------------->Exist  Username<-----------------*/
                                                // Shifting username/password characters and overwrite on the desired user to be deleted
                                                for (Uint8 Check_Counter_2 = Check_Counter + 1; Check_Counter_2 <= PC_Counter; Check_Counter_2++) {
                                                        EEPROM_WriteByte(2, Check_Counter_2, EEPROM_ReadByte(2, Check_Counter_2 + 4)); // Overwrite on the 4 characters of username in EEPROM Then shift all users place backward by shifting each character 4 places backward 
                                                        EEPROM_WriteByte(3, Check_Counter_2, EEPROM_ReadByte(3, Check_Counter_2 + 4)); // Overwrite on the 4 characters of password in EEPROM Then shift all users place backward by shifting each character 4 places backward
                                                }

                                                PC_Counter -= 4; // Reduce the number of users in counter
                                                EEPROM_WriteByte(4, 1, PC_Counter); // Saving the counter
                                                Check_Counter = PC_Counter + 1; // Exiting the check for loop
                                                UART_TX_Str("\r\n");
                                                UART_TX_Str("\r\n");
                                                UART_TX_Str("User deleted successfully\r\n");
                                                j = 1; // To determine whether we find the user or not
                                        }
                                }
                        }
                        /*--------------->Deleting Admin User<-------------------*/
                        else if (UART_GetValue == '3') {
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Please enter the AD username\r\n");

                                for (Uint8 i = 0; i < 4; i++) {
                                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                        User[i] = UART_RX_Char(); // Receiving the character in username array
                                }

                                /*---------------->Seaching For The User Want To Be Deleted<---------------------*/
                                for (Uint8 Check_Counter = 4; Check_Counter <= AD_Counter; Check_Counter += 4) {
                                        /*----------------->Username Check<------------------*/
                                        if (User[0] == EEPROM_ReadByte(5, Check_Counter + 1) && User[1] == EEPROM_ReadByte(5, Check_Counter + 2) && User[2] == EEPROM_ReadByte(5, Check_Counter + 3) && User[3] == EEPROM_ReadByte(5, Check_Counter + 4)) {
                                                /*----------------->Exist  Username<-----------------*/
                                                for (Uint8 Check_Counter_2 = Check_Counter + 1; Check_Counter_2 <= AD_Counter; Check_Counter_2++) {
                                                        EEPROM_WriteByte(5, Check_Counter_2, EEPROM_ReadByte(5, Check_Counter_2 + 4)); // Overwrite on the 4 characters of username in EEPROM Then shift all users place backward by shifting each character 4 places backward
                                                        EEPROM_WriteByte(6, Check_Counter_2, EEPROM_ReadByte(6, Check_Counter_2 + 4)); // Overwrite on the 4 characters of password in EEPROM Then shift all users place backward by shifting each character 4 places backward
                                                }

                                                AD_Counter -= 4; // Reduce the number of users in counter
                                                // To make sure that the main admin does not deleted
                                                if (Check_Counter == 0) {
                                                        AD_Counter += 4;
                                                }
                                                EEPROM_WriteByte(4, 2, AD_Counter); // Saving the counter
                                                Check_Counter = AD_Counter + 1; // Exiting the check for loop
                                                UART_TX_Str("\r\n");
                                                UART_TX_Str("\r\n");
                                                UART_TX_Str("User deleted successfully\r\n");
                                                j = 1; // To determine whether we find the user or not
                                        }
                                }
                        }
                        // If we not find the user
                        if (!j) {
                                UART_TX_Str("\r\n");
                                UART_TX_Str("\r\n");
                                UART_TX_Str("User is not exist\r\n");
                        }
                } else if (UART_GetValue == '0') {
                        /*------------->Loging-Out From The System<--------------*/
                        SYS_Check = 0; // 0 is the value that take the system to the login operation
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Bye-Bye user\r\n");
                }

                // *Not Important to Know* where the CMD_Check take the system to do nothing and avoid any complicated link between login operation and admin logged-in oparation
                else if (UART_GetValue == 8) {

                }
                // If user entered wrong command
                else {
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Please enter command number\r\n");
                }

                // If there is a keypad user
                if (eme_check) {
                        if (UART_GetValue == '9') {
                                /*---------------->Firing Keypad User From The System<---------------*/
                                eme_check = 0; // 0 is the value that take the system to the login operation for keypad also
                                LCD_Clear();
                                UART_TX_Str("\r\n");
                                UART_TX_Str("Keypad user kicked out\r\n");
                        }
                }

                /*--------------->Commands List<-----------------*/
                // If there is user
                if (SYS_Check) {
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Waiting for your command\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("1.Open the door\r\n");
                        UART_TX_Str("2.Close the door\r\n");
                        UART_TX_Str("3.Open lamps\r\n");
                        UART_TX_Str("4.Close lamps\r\n");
                        UART_TX_Str("5.Variant lamp\r\n");
                        UART_TX_Str("6.Display the temperature\r\n");
                        UART_TX_Str("7.Add new user\r\n");
                        UART_TX_Str("8.Delete exist user\r\n");
                        if (eme_check)
                                UART_TX_Str("9.Fire Keypad user\r\n");
                        UART_TX_Str("0.Logout\r\n");
                        UART_TX_Str("Reply with the number of the command\r\n");
                        UART_TX_Str("\r\n");
                }
                // if there is not user
                else {
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("1.Enter Admin\r\n");
                        UART_TX_Str("2.Enter User\r\n");
                        UART_TX_Str("\r\n");
                }

        }
        /*----------> User Loged-in <----------*/
        else if (SYS_Check == 2) {
                UART_GetValue = UDR; // Receiving the command

                // *Not Important to Know* it used to not take UART_Get value variable after login the system as 0 value (do nothing after entering the system)
                if (CMD_Check) {
                        UART_GetValue = 8;
                        CMD_Check = 0;
                }

                if (UART_GetValue == '1') {
                        // Open lamps
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Which lamp do you want to be opened(From 1 to 5)\r\n");
                        UART_TX_Str("6.All Lamps\r\n");
                        UART_TX_Str("\r\n");

                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                        UART_GetValue = UART_RX_Char(); // Receiving the command

                        // Open first lamp
                        if (UART_GetValue == '1') {
                                LED0_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(1) Opened");
                                }
                        }
                        // Open second lamp
                        else if (UART_GetValue == '2') {
                                LED1_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(2) Opened");
                                }
                        }
                        // Open third lamp
                        else if (UART_GetValue == '3') {
                                LED2_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(3) Opened");
                                }
                        }
                        // Open fourth lamp
                        else if (UART_GetValue == '4') {
                                LED3_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(4) Opened");
                                }
                        }
                        // Open fifth lamp
                        else if (UART_GetValue == '5') {
                                LED4_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(5) Opened");
                                }
                        }
                        // Open all lamps
                        else if (UART_GetValue == '6') {
                                LED0_ON();
                                LED1_ON();
                                LED2_ON();
                                LED3_ON();
                                LED4_ON();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("All Lamps Are");
                                        LCD_Goto(1, 0);
                                        LCD_Write_String("Opened");
                                }
                        }
                        // If there is no keypad user, using LCD to write & clear the operating device
                        if (!eme_check)
                                Timer2_SetDelay(2000); // Clearing LCD after 2 seconds using timer 2
                } else if (UART_GetValue == '2') {
                        // Close lamps
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Which lamp do you want to be Closed(From 1 to 5)\r\n");
                        UART_TX_Str("6.All Lamps\r\n");
                        UART_TX_Str("\r\n");

                        while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                        UART_GetValue = UART_RX_Char(); // Receiving the command

                        // CLose first lamp
                        if (UART_GetValue == '1') {
                                LED0_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(1) Closed");
                                }
                        }
                        // Close second lamp
                        else if (UART_GetValue == '2') {
                                LED1_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(2) Closed");
                                }
                        }
                        // Close third lamp
                        else if (UART_GetValue == '3') {
                                LED2_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(3) Closed");
                                }
                        }
                        // Close fourth lamp
                        else if (UART_GetValue == '4') {
                                LED3_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(4) Closed");
                                }
                        }
                        // Close fifth lamp
                        else if (UART_GetValue == '5') {
                                LED4_OFF();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Clear();
                                        LCD_Write_String("Lamp(5) Closed");
                                }
                        }
                        // Close all lamps
                        else if (UART_GetValue == '6') {
                                LED0_OFF();
                                LED1_OFF();
                                LED2_OFF();
                                LED3_OFF();
                                LED4_OFF();
                                LCD_Clear();
                                // If there is no keypad user, using LCD to write the operating device
                                if (!eme_check) {
                                        LCD_Write_String("All Lamps Are");
                                        LCD_Goto(1, 0);
                                        LCD_Write_String("Closed");
                                }
                        }
                        // If there is no keypad user, using LCD to write & clear the operating device
                        if (!eme_check)
                                Timer2_SetDelay(2000); // Clearing LCD after 2 seconds using timer 2
                } else if (UART_GetValue == '3') {
                        /*Dimmer circuit*/
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Please enter the percentage of the lamp\r\n");
                        UART_TX_Str("\r\n");

                        Dimmer_Value = 0; // making sure that dimmer value equal 0

                        // Taking the dimmer value as characters
                        for (Uint8 i = 0; i < 3; i++) {
                                while (!Get_Bit(UCSRA, 7)); // Polling until the user enter a character
                                Dimmer_Array[i] = UART_RX_Char(); // Receiving dimmer character values

                                // Converting Dimmer value from characters to integer to send it as duty cycle to timer(0)
                                if (Dimmer_Array[i] >= 48 && Dimmer_Array[i] <= 57) {
                                        Dimmer_Value = Dimmer_Value * 10 + (Dimmer_Array[i] - 48);
                                }
                                // if the value of character not from 48 to 57 (which is the number characters in ASCII table) these means that user entered the desired value and want to  escape other values
                                else {
                                        i = 5; // Exiting the for loop in which user enter the character values of dimmer circuit
                                }

                        }

                        // If there is no keypad user, using LCD to write the operating device
                        if (!eme_check) {
                                LCD_Clear();
                                LCD_Write_String("Lamp ON!");
                                LCD_Goto(1, 0);
                                LCD_Write_String("Percentage:");
                                LCD_Write_Integer(Dimmer_Value);
                                LCD_Write_Char('%');
                                Timer2_SetDelay(2000);
                        }

                        // To avoid twitch of dimmer lamp
                        if (Dimmer_Value >= 58 && Dimmer_Value <= 60) {
                                Dimmer_Value = 55;
                        } else if (Dimmer_Value > 60 && Dimmer_Value <= 62) {
                                Dimmer_Value = 65;
                        }

                        PWM0_Gen(Dimmer_Value); // Sending dimmer as duty cycle to timer(0)
                } else if (UART_GetValue == '4') {
                        /*--------------->Display temperature<----------------*/
                        // Declaring temperature array which take the temperature value from temperature integer that take its value from the temperature
                        Uint8 Temp_array[2] = {
                                0
                        };
                        Uint8 Temp_Divide = Temp; // Saving the value in other variable to do some operations

                        // Converting the integer value to characters and saved in the temperature array
                        for (Uint8 i = 0; i < 2; i++) {
                                Temp_array[i] = Temp_Divide % 10;
                                Temp_Divide /= 10;
                        }

                        // Display the temperature
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Temperature:\r\n");
                        UART_TX_Char(Temp_array[1] + 48);
                        UART_TX_Char(Temp_array[0] + 48);
                        UART_TX_Str("\r\n");

                        // If there is no keypad user, using LCD to write the operating device
                        if (!eme_check) {
                                LCD_Clear();
                                LCD_Write_String("Temperature:");
                                LCD_Write_Integer(Temp);
                                Timer2_SetDelay(2000);
                        }
                } else if (UART_GetValue == '0') {
                        /*------------->Loging-Out From The System<--------------*/
                        SYS_Check = 0; // 0 is the value that take the system to the login operation
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Bye-Bye user\r\n");
                }
                // *Not Important to Know* where the CMD_Check take the system to do nothing and avoid any complicated link between login operation and admin logged-in oparation
                else if (UART_GetValue == 8) {

                }
                // Entering wrong command
                else {
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Please enter command number\r\n");
                }
                /*----------->Command List<--------------*/
                // If there is a user
                if (SYS_Check) {
                        UART_TX_Str("\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("Waiting for your command\r\n");
                        UART_TX_Str("\r\n");
                        UART_TX_Str("1.Open lamps\r\n");
                        UART_TX_Str("2.Close lamps\r\n");
                        UART_TX_Str("3.Variant lamp\r\n");
                        UART_TX_Str("4.Display the temperature\r\n");
                        UART_TX_Str("0.Logout\r\n");
                        UART_TX_Str("Reply with the number of the command\r\n");
                        UART_TX_Str("\r\n");
                }
                // if there is not user
                else {
                        UART_TX_Str("1.Enter Admin\r\n");
                        UART_TX_Str("2.Enter User\r\n");
                        UART_TX_Str("\r\n");
                }
        }
}

ISR(USART_TXC_vect) {
        // Write UART transmit handler here

}