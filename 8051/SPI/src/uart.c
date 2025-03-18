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

int gety(void){
    if(RI){
        RI = 0;			// Clear the RI flag
	return SBUF;  	// Return the Character
    }
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
    IEN0|=0x80;
    IPL0 |= 0x10;
    TMOD |= 0x20; //TIMER 1, MODE 2
    SCON |= 0x50; //8 BIT, 1 STOP , REN ENABLED
    TCON |= 0x40; 	//START TIMER1
    TH1 = 0xFD;
    TI = 1;
}







