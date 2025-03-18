// Author: Lokesh Senthil Kumar
// uart.h file declares the uart function


#ifndef _UART_H_
#define _UART_H_



/**
 * @brief   Initializes the UART timer.
 * @details This function initializes the UART timer with a baud rate of 9600.
 */
void uart_init();

/**
 * @brief   Reads a character from the UART console.
 * @details This function reads a character from the UART console.
 * @return  The character read from the UART console.
 */
int getchar(void);

/**
 * @brief   Writes a character to the UART console.
 * @details This function writes a character to the UART console.
 * @param   c The character to write to the UART console.
 * @return  The character written to the UART console.
 */
int putchar(int c);


void UI(void);


#endif // _UART_H_
