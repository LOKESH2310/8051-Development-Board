/**
 * @file usart.c
 * @brief Implementation of UART communication functions for the 8051 microcontroller.
 *
 * This file provides functions to initialize the UART, send/receive characters, and perform
 * hexadecimal and integer conversions. It also includes a robust input parsing function.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <mcs51/8051.h>
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <mcs51/8051.h>
#include"driver.h"


#define MAX_DIGITS 20       // Maximum number of digits in user input
#define CARRIAGE_RETURN 13  // ASCII code for carriage return
#define BACKSPACE 8         // ASCII code for backspace
#define SPACE 32            // ASCII code for space


/**
 * @brief Receives a single character via UART.
 * 
 * Waits until a character is received, clears the receive interrupt flag, 
 * and returns the received character.
 * 
 * @return int The received character.
 */
int getchar (void)
{
    while (!RI); //Wait till the Character is received
	RI = 0;			// Clear the RI flag
	return SBUF;  	// Return the Character
}


/**
 * @brief Transmits a single character via UART.
 * 
 * Waits until the transmitter is ready, writes the character to the UART buffer,
 * and clears the transmit interrupt flag.
 * 
 * @param c The character to transmit.
 * @return int The transmitted character.
 */
int putchar (int c)
{
    while(!TI); //Wait till the Transmitter is ready
    SBUF = c; //write character to SBUF
    TI = 0; //Clear the TI flag
    return c;
}


/**
 * @brief Initializes the UART for 9600 baud communication.
 * 
 * Configures Timer 1 in Mode 2 (8-bit auto-reload) and UART in Mode 1 (8-bit UART).
 * Enables interrupts and prepares for data transmission.
 */
void initialize_UART(void)
{
    TMOD |= 0x20;   // Set Timer 1 mode to 8-bit auto-reload (Mode 2)
    SCON = 0x50;    // Set UART to Mode 1 (8-bit UART), REN enabled
    TH1 = 0xFD;     // Load TH1 for 9600 baud rate (for 11.0592 MHz clock)
    TR1 = 1;        // Start Timer 1
    ES = 1;         // Enable UART interrupt
    EA = 1;         // Enable global interrupt
    TI = 1;         // Set TI to indicate ready for transmission
}



/**
 * @brief Converts an integer (0-15) to its hexadecimal character representation.
 * 
 * @param num The integer to convert.
 * @return char The hexadecimal character, or '0' for invalid input.
 */
char int_to_char(int num) 
{
    const char hex_digits[] = "0123456789ABCDEF"; // Hexadecimal digits
    if (num >= 0 && num <= 15) {
        return hex_digits[num];
    }
    return '0';
}


/**
 * @brief Converts a character (hex digit) to its integer representation.
 * 
 * @param ch The hexadecimal character to convert.
 * @return uint8_t The integer representation of the character, or 0 for invalid input.
 */
uint8_t CONVERT_CHAR_INT(uint8_t ch)
{
    const char *hex_digits = "0123456789abcdefABCDEF"; // Hexadecimal digits
    const char *pos = strchr(hex_digits, ch); // Get position of character in hexadecimal digits
    if (pos != NULL) {
        return (uint8_t)(pos - hex_digits);
    }
    return 0;
}


/**
 * @brief Parses user input as a number in a given base.
 * 
 * Accepts characters until a carriage return is received. Supports hexadecimal input
 * and handles backspace for corrections. Converts the input string to an integer.
 * 
 * @param base The base (e.g., 10 for decimal, 16 for hexadecimal) of the input.
 * @return uint16_t The parsed number.
 */
uint16_t INPUT_PARSING(uint8_t base)
{
    uint16_t number = 0;
    uint16_t digits[MAX_DIGITS] = {0};
    uint16_t digit_count = 0;
    uint16_t i = 0;
    uint16_t current_char = 0;

    while (current_char != CARRIAGE_RETURN) {   // Loop until the input is complete.
        current_char = getchar();               // Read the next character.

        if (((current_char >= '0') && (current_char <= '9')) || ((current_char >= 'a') && (current_char <= 'f')) ||
            ((current_char >= 'A') && (current_char <= 'F'))){
            putchar(current_char);                              // Echo the character back to the user.
            digits[digit_count++] = CONVERT_CHAR_INT(current_char);
        } else if (current_char == BACKSPACE){
            if (digit_count > 0) {
                putchar(BACKSPACE); // Move the cursor back one position.
                putchar(SPACE);     // Print a space to overwrite the previous digit.
                putchar(BACKSPACE); // Move the cursor back one position again.
                digit_count--;      // Decrement the digit count.
            }
        }
    }

    for(i = 0; i < digit_count; i++) // Iterate over the digits.
    {
        number *= base;             // Multiply the current value of number by the base.
        number += digits[i];        // Add the current digit to number.
    }
    return number;
}

/**
 * @brief Prints a hexadecimal number with a specified width.
 * 
 * Converts the number to hexadecimal format and prints it with leading zeros
 * if necessary to ensure the specified width.
 * 
 * @param num The number to print.
 * @param width The minimum width of the output.
 */
void print_hex_number(uint32_t num, uint8_t width) {
    char ascii_buffer[10]; // Buffer for storing the ASCII representation.
    int8_t digit_count = 0;

    // Calculate the hex digits and store in buffer.
    do {
        ascii_buffer[digit_count++] = int_to_char(num & 0xF); // Extract lower 4 bits and convert.
        num >>= 4;  // Shift the number right by 4 bits (equivalent to dividing by 16).
    } while (num > 0);

    // Ensure the output has at least 'width' characters (leading zeros if needed).
    while (digit_count < width) {
        putchar('0');
        width--;
    }

    // Print the digits in reverse order (from most significant to least significant).
    while (digit_count > 0) {
        putchar(ascii_buffer[--digit_count]);
    }
}


