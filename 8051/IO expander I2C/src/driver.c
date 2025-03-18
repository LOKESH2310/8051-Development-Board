/******************************************************************************
 * File: driver.c
 * Author: Lokesh Senthil Kumar
 * Date: 15-11-2024
 * 
 * Description:
 * This file contains the implementation of I2C communication functions for 
 * interfacing with an EEPROM. It includes functions to manage the I2C lines 
 * (SDA and SCL), send and receive bytes, handle acknowledgments, and perform 
 * EEPROM-specific read and write operations.
 *
 * Key Functions:
 * - Low-level I2C control (start, stop, pulse, etc.)
 * - Byte-level data transfer (write and read)
 * - EEPROM read and write operations
 * - I2C bus reset
 *
 * Dependencies:
 * - Standard I/O library for debugging
 * - Hardware-specific macros for I2C line control
 *
 *****************************************************************************/

#include <stdio.h>
#include "driver.h"

/**
 * @brief Generates a clock pulse on the I2C clock line (SCL).
 */
void PULSE(void) {
    I2C_SCL(1);
    I2C_SCL(0);
    return;
}

/**
 * @brief Sets the I2C data line (SDA) to the specified value.
 
 * @param VAL Value to set (1 for high, 0 for low).
 */
void i2c_sda(uint8_t VAL) {
    I2C_SDA_PIN = (VAL == 1) ? 1 : 0;
}

/**
 * @brief Sets the I2C clock line (SCL) to the specified value.
 *
 * @param VAL Value to set (1 for high, 0 for low).
 */
void I2C_SCL(uint8_t VAL) {
    I2C_SCL_PIN = (VAL == 1) ? 1 : 0;
}

/**
 * @brief Waits for acknowledgment (ACK) from the slave device.
 */
void i2c_check_ack(void) {
    PULSE();
    while (I2C_SDA_PIN);
}

/**
 * @brief Sends a "no acknowledgment" (NACK) signal.
 */
void I2C_NO_ACK(void) {
    I2C_SCL(0);
    i2c_sda(1);
    I2C_SCL(1);
    i2c_sda(0);
}

/**
 * @brief Sends an I2C start condition.
 */
void i2c_start(void) {
    i2c_sda(1);
    I2C_SCL(1);
    i2c_sda(0);
    I2C_SCL(0);
}

/**
 * @brief Sends an I2C stop condition.
 */
void i2c_stop(void) {
    i2c_sda(0);
    I2C_SCL(1);
    i2c_sda(1);
    I2C_SCL(0);
}

/**
 * @brief Writes a byte of data to the I2C bus.
 *
 * @param DATA The byte to write.
 */
void I2C_BYTE_W(uint8_t DATA) {
    I2C_SCL(0);
    for (uint8_t i = 0; i < 8; i++) {
        i2c_sda((DATA & I2C_MSB_MASK) ? 1 : 0);
        PULSE();
        DATA = DATA << 1;
    }
    i2c_sda(0);
    return;
}

/**
 * @brief Reads a byte of data from the I2C bus.
 *
 * @return The byte read from the bus.
 */
uint8_t I2C_BYTE_R(void) {
    uint8_t byte_value = 0;

    // Prepare SDA line for input.
    i2c_sda(1);

    for (uint8_t bit_position = 0; bit_position < 8; bit_position++) {
        I2C_SCL(1); // Set SCL high to read the bit.
        byte_value = (byte_value << 1) | (I2C_SDA_PIN & 0x01); // Shift left and read SDA bit directly.
        I2C_SCL(0); // Set SCL low for the next bit.
    }

    return byte_value;
}

//write sequence for i/o expander
void PCF8574A_write(uint8_t data) {
    uint8_t address = 0x70; // Slave address for PCF8574A in write mode (0111000 followed by 0)
    
    i2c_start();            // Send START condition
    I2C_BYTE_W(address);    // Send slave address with write bit (0x38)
    i2c_check_ack();        // Wait for ACK from the slave
    I2C_BYTE_W(data);       // Write the byte to the expander
    i2c_check_ack();        // Wait for ACK from the slave
    i2c_stop();             // Send STOP condition
}

//ack sequence for the data received
void I2C_SEND_ACK(void) {
    I2C_SCL(0);       // Ensure SCL is low before starting ACK sequence
    i2c_sda(0);       // Pull SDA line low (ACK signal)
    PULSE();          // Generate a clock pulse on SCL
    i2c_sda(1);       // Release SDA line (prepare for next communication)
}


//read sequence for to get the data
uint8_t PCF8574A_read(void) {
    uint8_t address = 0x71; // Slave address for PCF8574A in read mode (0111000 followed by 1)
    uint8_t data;

    i2c_start();            // Send START condition
    I2C_BYTE_W(address);    // Send slave address with read bit (0x39)
    i2c_check_ack();        // Wait for ACK from the slave
    data = I2C_BYTE_R();    // Read the byte from the expander
    I2C_SEND_ACK();
    i2c_stop();             // Send STOP condition

    return data;            // Return the read byte
}



/**
 * @brief Writes data to a specific address in the EEPROM.
 *
 * @param ADD The memory address to write to (16-bit).
 * @param DATA The data byte to write.
 */
void I2C_EEPROM_WRITE(uint16_t ADD, uint8_t DATA) {
    uint8_t ADD_LSB = (uint8_t)ADD;
    uint8_t ADD_MSB = (uint8_t)(ADD >> 7);
    ADD_MSB |= IDENTIFIER_MASK;
    ADD_MSB &= WRITE_MASK;

    i2c_start();
    I2C_BYTE_W(ADD_MSB);
    i2c_check_ack();
    I2C_BYTE_W(ADD_LSB);
    i2c_check_ack();
    I2C_BYTE_W(DATA);
    i2c_check_ack();
    i2c_stop();

    return;
}

/**
 * @brief Reads data from a specific address in the EEPROM.
 *
 * @param ADD The memory address to read from (16-bit).
 * @return The data byte read from the specified address.
 */
uint8_t I2C_EEPROM_READ(uint16_t ADD) {
    uint8_t ADD_LSB = (uint8_t)ADD;
    uint8_t ADD_MSB = (uint8_t)(ADD >> 7);
    ADD_MSB |= IDENTIFIER_MASK;
    ADD_MSB &= WRITE_MASK;

    i2c_start();
    I2C_BYTE_W(ADD_MSB);
    i2c_check_ack();
    I2C_BYTE_W(ADD_LSB);
    i2c_check_ack();
    ADD_MSB |= I2C_READ_MASK;
    i2c_start();
    I2C_BYTE_W(ADD_MSB);
    i2c_check_ack();
    uint8_t DATA = I2C_BYTE_R();
    I2C_NO_ACK();
    i2c_stop();

    return DATA;
}

/**
 * @brief Resets the I2C bus to ensure proper communication.
 */
void I2C_RESET() {
    i2c_start();
    for (uint8_t i = 0; i < 9; i++) {
        i2c_sda(1);
        PULSE();
    }
    i2c_start();
    i2c_stop();

    return;
}
