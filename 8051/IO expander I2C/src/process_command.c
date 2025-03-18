#include <stdio.h>
#include <mcs51/8051.h>
#include <at89c51ed2.h>
#include <mcs51reg.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "process_command.h"
#include "uart.h"
#include "driver.h"

#define HEX_BASE        (16)
#define DIVIDE_BY_16    (16)
#define ASCII_MAX       (255)
#define ADDR_MAX        (2047)
#define ASCII_SPACE     (32)


void EEPROM_WRITE(void)
{
    uint16_t data_read = 0;
    uint16_t addr_read = 0;

    printf_tiny(" \r\nWriting to EEPROM !!\r\n");

    printf_tiny(" \r\nEnter Data to put into EEPROM\r\n");
    data_read = INPUT_PARSING(HEX_BASE);

   
    if(data_read >= 0 && data_read <= ASCII_MAX){}
    else{
        printf("\r\nInvalid Data Range!! \r\nData has to be between 0x00 to 0xFF \r\n");
        return;
    }

    printf("\r\n");

    printf_tiny("\r\nEnter Address to put into EEPROM\r\n");
    addr_read = INPUT_PARSING(HEX_BASE );
    
    if(addr_read >= 0 && addr_read <= ADDR_MAX){}
    else{
        printf("\r\nInvalid Address Range!!\r\nAddress has to be between 0x000 to 0x7FF\r\n");
        return;
    }
    printf("\r\n");

    I2C_EEPROM_WRITE(addr_read,data_read);
    printf_tiny("\r\nFinished writting to EEPROM !!\r\n");
}


void EEPROM_READ(void)
{
    __xdata uint16_t addr_read = 0;
    __xdata uint8_t byte_read1 = 0;
    printf_tiny("\r\nReading from EEPROM !!\r\n");

    printf_tiny("\r\nEnter Address to put into EEPROM\r\n");
    addr_read = INPUT_PARSING(HEX_BASE );
    if(addr_read >= 0 && addr_read <= ADDR_MAX){}
    else{
        printf("\r\nInvalid Adress Range!!\r\n Address has to be between 0x000 to 0x7FF\r\n");
        return;
    }
    printf("\r\n");

    byte_read1 = I2C_EEPROM_READ(addr_read);
    printf_tiny("\r\nReading from EEPROM Completed!!\r\n");
    printf("\r\nData = %x present at Location = 0%x \r\n",byte_read1,addr_read);

}

void EEPROM_DUMP(void)
{
    __xdata uint16_t start_addr = 0;
    printf_tiny("\r\nEnter Start Address for HEX Dump\r\n");
    start_addr = INPUT_PARSING(HEX_BASE);

    if(start_addr >= 0 && start_addr <= ADDR_MAX){}
    else{
        printf("\r\nInvalid Start Address Range!!\r\n Address has to be between 0x000 to 0x7FF\r\n");
        return;
    }

    __xdata uint16_t end_addr = 0;

    printf_tiny("\r\nEnter End Address for HEX Dump\r\n");
    end_addr = INPUT_PARSING(HEX_BASE);

    if(end_addr >= 0 && end_addr <= ADDR_MAX){}
    else{
        printf("\r\nInvalid End Address Range!!\r\n Address has to be between 0x000 to 0x7FF\r\n");
        return;
    }

    printf_tiny("\r\nI2C EEPROM DUMP!!\r\n");

    __xdata uint8_t count = 0, i = 0, temp_storage = 0, data_byte = 0;
    while (start_addr <= end_addr) {
        if (count % DIVIDE_BY_16 == 0) {
            putchar('\n');
            putchar('\r');
            print_hex_number(start_addr, 3);
            putchar(':');
        }
        putchar(ASCII_SPACE);
        data_byte = I2C_EEPROM_READ(start_addr);
        print_hex_number(data_byte, 2);

        start_addr++;
        count++;
    }
    printf("\r\n");
}



