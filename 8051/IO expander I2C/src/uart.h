#ifndef _UART_H_
#define _UART_H_

int getchar (void);

int putchar (int c);

void initialize_UART(void);


uint8_t CONVERT_CHAR_INT(uint8_t ch);


char int_to_char(int num);


uint16_t INPUT_PARSING(uint8_t number_base);


void print_hex_number(uint32_t num, uint8_t width);

#endif // _UART_H_
