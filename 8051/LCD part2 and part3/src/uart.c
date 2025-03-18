// Author: Lokesh Senthil Kumar
// uart.c file controls UART for RS232


// Include necessary header files
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <mcs51/8051.h>
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <mcs51/8051.h>
#include<string.h>

#include "lcd.h"

// Define constants
#define RX_BUFFER_SIZE 2000
#define TX_BUFFER_SIZE 2000

// Declare global variables
extern volatile char    tenth_of_second;
extern volatile char    seconds_ones_digit;
extern volatile char    seconds_tens_digit;
extern volatile char    minutes_ones_digit;
extern volatile char    minutes_tens_digit;
extern volatile int     update_lcd;
extern volatile uint8_t save_cursor_address;

// Initialize UART
void uart_init()
{
    
    SCON = 0x50;    
    TMOD |= 0x20;   
    TH1 = 0xFD;     
    TL1 = 0xFD;     
    TI = 0;         
    SBUF = 0;       
    TR1 = 1;        
    ES = 1;         
    EA = 1;         
}

// Send a character over UART
int putchar (int c) {
    while (!TI);    
    SBUF = c;       
    TI = 0;         
    return c;
}

int getchar (void)
{
    while (!RI);    
    RI = 0;         
    return SBUF;    
}


void UI(void) 
{
    printf("INTERFACE FOR LCD*\r\n");
    printf("------------------------------------------------\r\n");
    printf("| ---  [A] - Put character to LCD   --------------|\r\n");
    printf("| ---  [B] -  Put string to LCD     --------------|\r\n");
    printf("| ---  [C] -  Goto Address (0xNN)      --------------|\r\n");
    printf("| ---  [D] -  Goto Co-ordinates  --------------|\r\n");
    printf("| ---  [E] -  Stop Time         --------------|\r\n");
    printf("| ---  [F] -  Resume Time       --------------|\r\n");
    printf("| ---  [G] -  Reset time     --------------|\r\n");
    printf("| ---  [H] -  HEX Dump              --------------|\r\n");
    printf("| ---  [I] -  Custom Characters   --------------|\r\n");
    printf("| ---  [J] -  Load CU LOGO              --------------|\r\n");
    printf("| ---  [X] -  Clear LCD          --------------|\r\n");
    printf("| ---  [P] -  BOARD NAME                --------------|\r\n");
    printf("| ---  [L] -  UI                      --------------|\r\n");

    printf("-------------------------------------------------------\r\n");
}