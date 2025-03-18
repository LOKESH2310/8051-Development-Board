/******************************************************************************
 * File: main.c
 * Author: Lokesh Senthil Kumar
 * Date: 15-11-2024
 * 
 * Description:
 * This file implements a user interface for controlling an EEPROM via an I2C 
 * interface on the 8051 microcontroller. It supports various commands to read, 
 * write, dump, and reset the EEPROM.
 *
 * The program uses UART for communication, allowing users to input commands 
 * which are processed by the COMMAND_CONTROL function. The I2C functions are 
 * initialized during startup.
 *
 * Dependencies:
 * - UART initialization and communication (uart.h)
 * - EEPROM control functions (driver.h, process_command.h)
 *
 * Command List:
 * - 'W' or 'w': Write data to EEPROM.
 * - 'R' or 'r': Read data from EEPROM.
 * - 'D' or 'd': Hex dump of EEPROM.
 * - 'S' or 's': Reset EEPROM.
 *
 *****************************************************************************/

/* Include standard libraries and hardware-specific headers */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <mcs51/8051.h>
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include "driver.h"
#include "uart.h"
#include "process_command.h"

/**
 * @brief Processes the user input command and calls the respective EEPROM control functions.
 *
 * @param COM The command character entered by the user.
 *            Supported Commands:
 *              - 'W' or 'w': Write data to EEPROM.
 *              - 'R' or 'r': Read data from EEPROM.
 *              - 'D' or 'd': Hex dump of EEPROM.
 *              - 'S' or 's': Reset EEPROM.
 *            Any other character prompts an error message and displays the command menu.
 */
void COMMAND_CONTROL(char COM);



//external interrupt configuration for to trigger the change the i/o expander
void initialize_interrupt(void) {
    IT0 = 1;      		        /* Interrupt0 on falling edge */
	EX0 = 1;      		        /* Enable External interrupt0 */
	IP = 0x01;		            /* Set highest priority for Ext. interrupt0 */
}


//external interrupt handler
void external_interrupt0_ISR(void) __interrupt (0) {
    uint8_t data;
    printf("\r\nISR works\r\n");
    // Read current state from PCF8574A
    PCF8574A_write(0xFF);
    data = PCF8574A_read();

    
    if (data & 0x01) {          // Check if the first bit is 1
        data &= ~(1 << 1);      // Clear the second bit
    } else {
        data |= (1 << 1);       // Set the second bit
    }

    // Write modified data back to PCF8574A
    PCF8574A_write(data);
}


/**
 * @brief Entry point of the program. Initializes peripherals and provides a 
 *        command-line interface for EEPROM operations.
 *
 * This function:
 * 1. Initializes UART and I2C interfaces.
 * 2. Displays a menu with supported commands.
 * 3. Enters an infinite loop to process user commands.
 *
 * @return int This function does not return as it runs an infinite loop.
 */
int main(void) {
    initialize_UART(); // Initialize UART for communication
    
    initialize_interrupt(); // Initialize interrupt for /INT0
    
    // Configure PCF8574A: Set all pins as inputs initially
    PCF8574A_write(0xFF);
    printf("\r\n ---- PCF8574A Interrupt is done ----\r\n");

    char COMMAND;

    // Display command menu
    printf("\r\n ------------------------------\r\n");
    printf("\r\n ---- EEPROM I2C Interface ----\r\n");
    printf("\r\n ------------------------------\r\n");
    printf("\r\n COMMANDS:\r\n");
    printf("\r\n W - Write Data to EEPROM\r\n");
    printf("\r\n R - Read Data from EEPROM\r\n");
    printf("\r\n D - Hex Dump of EEPROM\r\n");
    printf("\r\n S - Reset EEPROM\r\n");
    printf("\r\n ------------------------------\r\n");

    // Command processing loop
    while (1) {
        printf("\r\n ENTER THE COMMAND: \r\n");
        COMMAND = getchar();          // Read user input
        COMMAND_CONTROL(COMMAND);     // Process the command
    }
}

// Command handling function implementation
void COMMAND_CONTROL(char COM) {
    switch (COM) {
        case 'W':
        case 'w':
            EEPROM_WRITE();  // Call EEPROM write function
            break;
        case 'R':
        case 'r':
            EEPROM_READ();   // Call EEPROM read function
            break;
        case 'D':
        case 'd':
            EEPROM_DUMP();   // Call EEPROM hex dump function
            break;
        case 'S':
        case 's':
            I2C_RESET();     // Reset the EEPROM
            printf("\r\n DONE EEPROM Reset\r\n");
            printf("\r\n Please give the valid command!\r\n");
            printf("\r\n ------------------------------\r\n");
            printf("\r\n ---- EEPROM I2C Interface ----\r\n");
            printf("\r\n ------------------------------\r\n");
            printf("\r\n COMMANDS:\r\n");
            printf("\r\n W - Write Data to EEPROM\r\n");
            printf("\r\n R - Read Data from EEPROM\r\n");
            printf("\r\n D - Hex Dump of EEPROM\r\n");
            printf("\r\n S - Reset EEPROM\r\n");
            printf("\r\n ------------------------------\r\n");
            break;
        default:
            // Invalid command handling
            printf("\r\n Please give the valid command!\r\n");
            printf("\r\n ------------------------------\r\n");
            printf("\r\n ---- EEPROM I2C Interface ----\r\n");
            printf("\r\n ------------------------------\r\n");
            printf("\r\n COMMANDS:\r\n");
            printf("\r\n W - Write Data to EEPROM\r\n");
            printf("\r\n R - Read Data from EEPROM\r\n");
            printf("\r\n D - Hex Dump of EEPROM\r\n");
            printf("\r\n S - Reset EEPROM\r\n");
            printf("\r\n ------------------------------\r\n");
            break;
    }
}
