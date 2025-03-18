


#ifndef _driver_H_
#define _driver_H_

#include <stdint.h>
#include "uart.h"
#include "at89c51ed2.h"
#include <mcs51reg.h>

#define I2C_SDA_PIN P1_5
#define I2C_SCL_PIN P1_4

#define I2C_MSB_MASK               (0x80)
#define IDENTIFIER_MASK        (0xA0)
#define IDENTIFIER_MASK_2      (0xAF)
#define I2C_READ_MASK               (0x01)
#define WRITE_MASK              (0xFE)
#define I2C_LSB_HIGH_MASK           (0x01)
#define I2C_LSB_LOW_MASK            (0xFE)


void PULSE(void);
void PCF8574A_write(uint8_t data);
uint8_t PCF8574A_read(void);
void I2C_SEND_ACK(void);
void    i2c_sda(uint8_t value);



void    I2C_SCL(uint8_t value);



void    i2c_start(void);



void    i2c_stop(void);



void  i2c_check_ack(void);


void I2C_BYTE_W(uint8_t data_byte);



uint8_t I2C_BYTE_R(void);

void I2C_EEPROM_WRITE(uint16_t address,uint8_t data_byte);


uint8_t I2C_EEPROM_READ(uint16_t address);


void I2C_RESET(void);


#endif // _driver_H_

