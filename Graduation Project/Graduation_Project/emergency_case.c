/*
 * CFile1.c
 *
 * Created: 22/10/2022 3:16:23 PM
 *  Author: sara-
 */

#include "emergency_case.h"

// Defining the used Variables
extern Uint8 button;
extern Uint16 Temp;
extern f32 Dimmer_Value;
extern Uint8 Dimmer_Array[3];
extern Uint8 eme_check;

void emergency_case(void) {
        LCD_Clear(); // Clearing The LCD
        LCD_Write_String("1.open lamps"); //Choosing 1 For Lamps
        LCD_Goto(1, 0);
        LCD_Write_String("2.more"); //Choosing 1 For more Options
        button = NULL_KEYPAD; // Initialize The Keypad button =0

        while (!(button == '1' || button == '2') && eme_check) // Determine Our Choice (1 or 2)
                button = Keypad_GetValue();

        if (button == '1') { // If we Choose 1 in first we need to choose which lamp will open 
                LCD_Clear();
                LCD_Write_String("1.lamp1  2.lamp2");
                LCD_Goto(1, 0);
                LCD_Write_String("3.lamp3  4.more");

                button = NULL_KEYPAD;
                while (!(button == '1' || button == '2' || button == '3' || button == '4') && eme_check) // This For Choosing Which lamb to open
                        button = Keypad_GetValue();
                /*   This Fuction For Opening Lambs   */
                if (button == '1') { // Lamb 1 will Open
                        LED0_ON();
                } else if (button == '2') { // Lamb 2 will Open
                        LED1_ON();
                } else if (button == '3') { // Lamb 3 will Open
                        LED2_ON();
                } else if (button == '4') { // Here to Show more options 
                        LCD_Clear();
                        LCD_Write_String("1.lamp4  2.lamp5"); // The option is open Lamb 4 or 5 or all
                        LCD_Goto(1, 0);
                        LCD_Write_String("3.all lamps");
                        button = NULL_KEYPAD; // Initialize the keypad button =0
                        while (!(button == '1' || button == '2' || button == '3') && eme_check) // Choosing the Button To do The function
                                button = Keypad_GetValue();
                        if (button == '1') { // If We Choose 1 Lamb 4 Will open
                                LED3_ON();
                        } else if (button == '2') { // If We Choose 2 Lamb 5 Will open
                                LED4_ON();
                        } else if (button == '3') { // If We Choose 3 all Lambs Will open
                                LED0_ON(); /* Opening all Lambs*/
                                LED1_ON();
                                LED2_ON();
                                LED3_ON();
                                LED4_ON();
                        }
                }

        } else if (button == '2') // If We Choose 2 In first Choice This Will Open more Options

        {
                LCD_Clear();
                LCD_Write_String("3.close lamps"); // Choosing 3 For Closing The Lambs
                LCD_Goto(1, 0);
                LCD_Write_String("4.more"); // Choosing 4 For more Options  
                button = NULL_KEYPAD;

                while (!(button == '3' || button == '4') && eme_check) // Determine Our Choice (3 or 4)
                        button = Keypad_GetValue();

                if (button == '3') { // If We Choose 3 We Will Close Lambs
                        LCD_Clear();
                        LCD_Write_String("1.lamp1  2.lamp2");
                        LCD_Goto(1, 0);
                        LCD_Write_String("3.lamp3  4.more");
                        button = NULL_KEYPAD;

                        while (!(button == '1' || button == '2' || button == '3' || button == '4') && eme_check) //This For Choosing Which lamb to Close
                                button = Keypad_GetValue();

                        if (button == '1') { //Lamb 1 will Close
                                LED0_OFF();
                        } else if (button == '2') { //Lamb 2 will Close
                                LED1_OFF();
                        } else if (button == '3') { //Lamb 3 will Close
                                LED2_OFF();
                        } else if (button == '4') { // Here to Show more options 
                                LCD_Clear();
                                LCD_Write_String("1.lamp4  2.lamp5"); // The option is Close Lamb 4 or 5 or all
                                LCD_Goto(1, 0);
                                LCD_Write_String("3.all lamps");
                                button = NULL_KEYPAD;

                                while (!(button == '1' || button == '2' || button == '3') && eme_check) // Choosing the Button To do The function
                                        button = Keypad_GetValue();

                                if (button == '1') { // If We Choose 1 Lamb 4 Will Close
                                        LED3_OFF();
                                } else if (button == '2') { // If We Choose 2 Lamb 5 Will Close
                                        LED4_OFF();
                                } else if (button == '3') { // If We Choose 3 all Lambs Will Close
                                        LED0_OFF();
                                        LED1_OFF();
                                        LED2_OFF();
                                        LED3_OFF();
                                        LED4_OFF();
                                }
                        }

                } else if (button == '4') { // If We Choose 4 In Second Choice This Will Open more Options
                        LCD_Clear();
                        LCD_Write_String("5.variant lamp"); // Choosing 5 For Variant Lamb 
                        LCD_Goto(1, 0);
                        LCD_Write_String("6.Display temp"); // Choosing 6 For Display Temperature
                        button = NULL_KEYPAD;

                        while (!(button == '5' || button == '6') && eme_check) // Determine Our Choice (5 or 6)
                                button = Keypad_GetValue();

                        if (button == '5') { // Choosing 5 for Determine The percentage of variant Lambs
                                LCD_Clear();
                                LCD_Write_String("Please enter the");
                                LCD_Goto(1, 0);
                                LCD_Write_String("% of the lamp");
                                button = NULL_KEYPAD;
                                Dimmer_Value = 0; // Variable For Dimmer (Variant lamb)
                                /* Function For Determine The Percentage */
                                for (Uint8 i = 0; i < 3; i++) {
                                        button = NULL_KEYPAD;

                                        while (!(button >= '0' && button <= '9' || button == '*' || button == '#') && eme_check) { // Clicking The Keypad Button For Determine The Percentage
                                                button = Keypad_GetValue();
                                        }

                                        Dimmer_Array[i] = button; // Equaling The Value of Keypad Button to An Array

                                        if (Dimmer_Array[i] >= 48 && Dimmer_Array[i] <= 57) { // This for Choosing The Value from 0 to 9
                                                Dimmer_Value = Dimmer_Value * 10 + (Dimmer_Array[i] - 48); // Taking The value of dimmer and multiply by 10 the anding the value of dimmer array 
                                        } else {
                                                i = 5;
                                        }
                                }
                                // To avoid twitch  of dimmer Lamb 
                                if (Dimmer_Value >= 58 && Dimmer_Value <= 60) {
                                        Dimmer_Value = 55;
                                } else if (Dimmer_Value > 60 && Dimmer_Value <= 62) {
                                        Dimmer_Value = 65;
                                }

                                PWM0_Gen(Dimmer_Value);
                        } else if (button == '6') { // Choosing 6 Will Display The Temperature  
                                // Display temperature
                                LCD_Clear();
                                LCD_Write_String("Temperature:");
                                LCD_Write_Integer(Temp);

                                _delay_ms(1000);
                        }
                }
        }
}