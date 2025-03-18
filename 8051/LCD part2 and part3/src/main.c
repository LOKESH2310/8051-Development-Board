// Author: Lokesh Senthil Kumar
// main.c file controls the flow of LCD program

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <string.h>
#include <mcs51/8051.h>

#include "uart.h"
#include "lcd.h"

// Flag variable to update LCD display
volatile int update_lcd = 0;
// volatile int user_input_required = 0;



// Global variables to store different digits of the time
volatile char tenth_of_second    ='0';
volatile char seconds_ones_digit ='0';
volatile char seconds_tens_digit ='0';
volatile char minutes_ones_digit ='0';
volatile char minutes_tens_digit ='0';

void timer0_ISR() __interrupt(1) { // Define Timer 0 interrupt service routine
//printf("ISR");
    EA = 0;     // Disable interrupts

    TF0 = 0;    // Clear Timer 0 interrupt flag

    static int counter_02s = 0; // Initialize a static variable
    volatile uint8_t save_cursor_addr = get_cursor_address(); // Get the current cursor address

    if (counter_02s == 2) { // means 0.2 seconds have passed
        P1_1 = P1_1 ^ 1;    // Toggle pin P1_1
        update_lcd  = 1;    // Set the update_lcd flag
        counter_02s = 0;    // Reset the counter_02s
        tenth_of_second++;  // Increment the tenth of a second
    }

    TL0 = 0xfd;     // Set the low value of Timer 0 to 0xfd
    TH0 = 0x4b;     // Set the high value of Timer 0 to 0x4b
    counter_02s++;  // Increment the counter_02s

    if (tenth_of_second == ':') { // If the tenth of a second counter means 1 second has passed
        tenth_of_second = '0'; // Reset the tenth
        seconds_ones_digit++; // Increment the first digit

        if (seconds_ones_digit == ':') { // If the first digit of the seconds counter means 10 seconds have passed
            seconds_ones_digit = '0'; // Reset the first digit
            seconds_tens_digit++; // Increment the second digit

            if (seconds_tens_digit == '6') { // If the second digit of the seconds counter  means 1 minute has passed
                seconds_tens_digit = '0'; // Reset the second digit
                minutes_ones_digit++; // Increment the first digit

                if (minutes_ones_digit == ':') { // If the first digit of the minutes counter means 10 minutes have passed
                    minutes_ones_digit = '0'; // Reset the first digit
                    minutes_tens_digit++; // Increment the second digit

                    if (minutes_tens_digit == '6') { // If the second digit of the minutes counter  means 1 hour has passed
                        minutes_tens_digit = '0'; // Reset the second digit
                    }
                }
            }
        }
    }
    lcdgotoaddr(save_cursor_addr); // Move the cursor to the previous cursor address
    EA=1; // Enable interrupts
}


void init_timer(void){
    TCON=TCON&(~0x30);   // Clearing the last 2 bits in TCON register
    TMOD |= 0x01;        // Setting the Timer 0 mode to 16-bit mode
    TL0 = 0xfd;          // Setting the initial value of Timer 0 low byte
    TH0 = 0x4b;          // Setting the initial value of Timer 0 high byte
    IE   |=0x82;         // Enabling Timer 0 interrupt
    PT0 = 1;             // Setting the Timer 0 interrupt priority to high
    TR0 = 1;             // Starting Timer 0
}


void main(void)
{
    uart_init();        // Initialize UART for serial communication
    init_lcd();         // Initialize LCD
    init_timer();       // Initialize Timer for timing functionality
    UI();         // Print the UI (User Interface) on the LCD

    while(1)
    {
            if(update_lcd) // If the LCD needs to be updated
            {
                //printf("lkok");
                volatile uint8_t save_cursor_address = get_cursor_address(); // Save the current cursor address
                lcdgotoaddr(0x59);              // Move the cursor to the specific location
                //printf("lkok2222222222222222222222222222222");
                lcdputch(minutes_tens_digit);           // Display the tens digit
                lcdputch(minutes_ones_digit);           // Display the ones digit
                lcdputch(':');                  // Display a colon on the LCD
                lcdputch(seconds_tens_digit);           // Display the tens digit
                lcdputch(seconds_ones_digit);           // Display the ones digit
                lcdputch('.');                  // Display a period
                lcdputch(tenth_of_second);     // Display the tenths digit
                update_lcd = 0;                 // Reset the flag
                lcdgotoaddr(save_cursor_address); // Restore the cursor address
            }

            /* Fetching Characters */
            if(RI)
            {
                char char_detected;
                char_detected = SBUF;           // Store the character in a variable
                RI = 0;
                putchar(char_detected);        
                printf("\n\r ");                 // Move to the next line on the serial terminal
                switch(char_detected)           // Perform a certain action based on the received character
                {
                case 'L':                       
                    __critical                  
                    {
                        UI();             
                    }
                    break;

                case 'A': 
                    handler_wr_c_lcd();         // Hanwriting a single character to the LCD
                    break;

                case 'B': 
                    handler_wr_str_lcd();       // writing a string to the LCD
                    break;

                case 'C': 
                    handler_lcdgotoaddress();   // moving the cursor to a specific address on the LCD
                    break;

                case 'D':
                    handler_lcdgotoxy();        // moving the cursor to a specific row and column on the LCD
                    break;

                case 'X': 
                    handler_lcdclear();         //clearing the LCD
                    break;

                case 'E':
                    handler_stop_time();        // function to stop the timer
                    break;

                case 'F': 
                    handler_resume_time();      // function to resume the timer
                    break;

                case 'G':
                    handler_reset_time();       // function to reset the timer
                    break;

                case 'H': 
                    handler_lcd_hexdump();      // function to dump the contents of the LCD
                    break;

                case 'I': 
                    handler_custom_char();      // function to create custom characters for the LCD
                    break;

                case 'J': 
                    handle_cu_custom_char();    // function to create a custom "CU" character for the LCD
                    break;

                case 'P': 
                    print_board_name();    
                    break;

                default:  
                    printf("Invalid Character!! Please Enter the valid character\n\r");
                    break;
                }
            }
    }
}
