// Author: Lokesh Senthil Kumar
// main.c files controls the flow Bit banging SPI


#include "at89c51ed2.h"
#include "mcs51reg.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

/* DAC Control Pins */
#define sck P1_6        // SPI Clock
#define sdi P1_7        // SPI Data Input
#define cs_bar P1_3     // Chip Select

/* DAC Configuration Masks */
#define A_mask 0xF000              // DAC Channel A mask
#define active_mask 0x3000         // Active configuration mask
#define Gain_increase_mask 0xFFFF  // Mask for gain increase
#define Gain_decrease_mask 0x7FFF  // Mask for gain decrease

//data points for the wave
__xdata uint8_t static sine_wave[160] = {
    128, 131, 134, 137, 140, 144, 147, 150, 153, 156, 159, 162, 165, 168, 171, 174,
    177, 180, 182, 185, 188, 191, 194, 196, 199, 201, 204, 206, 209, 211, 214, 216,
    217, 215, 212, 210, 208, 205, 203, 200, 197, 195, 192, 189, 187, 184, 181, 178,
    175, 172, 169, 167, 164, 160, 157, 154, 151, 148, 145, 142, 139, 136, 133, 130,
    126, 123, 120, 117, 114, 111, 108, 105, 102,  99,  96,  92,  89,  87,  84,  81,
     78,  75,  72,  69,  67,  64,  61,  59,  56,  53,  51,  48,  46,  44,  41,  39,
     37,  35,  33,  31,  29,  27,  25,  23,  21,  20,  18,  16,  15,  14,  12,  11,
     12,  13,  14,  16,  17,  19,  20,  22,  24,  26,  28,  30,  32,  34,  36,  38,
     40,  42,  45,  47,  50,  52,  55,  57,  60,  62,  65,  68,  71,  74,  76,  79,
     82,  85,  88,  91,  94,  97, 100, 103, 106, 109, 112, 116, 119, 122, 125, 128,
};

/* GLOBAL Variables */
__xdata uint16_t counter = 0;
uint8_t gain = 1;  // Default gain setting

/* SPI Bit-Banging Functions */

/**
 *Generates a single clock cycle on the SPI clock line.
 */
void spi_clock_tick(void) {
    sck = 1; // Clock high
    sck = 0; // Clock low
}

/**
 * @brief Send a 16-bit word using SPI bit-banging.
 * 
 * @param data_word The 16-bit data word to be transmitted.
 */
void spi_write_word(uint16_t data_word) {
    

    sck = 0;         // Ensure clock starts low
    cs_bar = 0;      // Enable chip select

    for (uint8_t i = 0; i < 16; i++) {
        sdi = (data_word & 0x8000) ? 1 : 0; // Send MSB first
        spi_clock_tick();                   // Generate clock pulse
        data_word <<= 1;                    // Shift data left for next bit
    }

    cs_bar = 1;      // Disable chip select
    sck = 0;
}

/**
 * @brief Updates the DAC output for Channel A.
 */
void dac_update_output(void) {
    uint16_t command_word = (sine_wave[counter] << 4) | A_mask | active_mask;

    // Apply gain
    if (gain == 2) {
        command_word &= Gain_increase_mask;
    } else {
        command_word &= Gain_decrease_mask;
    }

    spi_write_word(command_word); // Send command to DAC

    counter = (counter + 1) % 160; // Increment counter for sine wave
}

/* Gain Control Functions */
void dac_increase_voltage(void) {
    if (gain < 2) {
        gain++;
    }
}

void dac_decrease_voltage(void) {
    if (gain > 1) {
        gain--;
    }
}

/* Timer 0 Interrupt Handler */
void wave_interrupt_handler(void) __interrupt(1) {
    TF0 = 0;       // Clear Timer 0 overflow flag
    TL0 = 0x00;    // Reload Timer 0
    TH0 = 0xFC;    // Reload Timer 0
    dac_update_output(); // Update DAC output
}

/* Timer Initialization */
void waves_init(void) {
    IEN0 |= 0x82;  // Enable Timer 0 interrupt
    TMOD |= 0x01;  // Timer 0 Mode 1
    TMOD &= 0xF1;  // Clear other Timer 1 configuration bits
    TL0 = 0x00;    // Set low byte reload value
    TH0 = 0xFC;    // Set high byte reload value for ~1 ms interval
    TR0 = 1;       // Start Timer 0
}

/* Main Function */
void main(void) {
    __xdata uint8_t key_pressed;

    initialize_UART(); // Initialize UART
    waves_init();      // Initialize Timer for waveform updates

    printf("\n\rWelcome to DAC wave generator");
    printf("\n\rCommands: \n\r'+'-> Increase the Voltage, \n\r'-'-> Decrease the Voltage, \n\r'?'-> help");

    while (1) {
        key_pressed = getchar();
        switch (key_pressed) {
            case '+':
                dac_increase_voltage();
                printf("\n\rVoltage Increased\n\r");
                break;
            case '-':
                dac_decrease_voltage();
                printf("\n\rVoltage Decreased\n\r");
                break;
            case '?':
                printf("\n\rCommands: \n\r'+'-> Increase Voltage,\n\r '-'-> Decrease Voltage,\n\r '?'-> Display Menu");
                break;
            default:
                printf("\n\rInvalid Command");
                break;
        }
    }
}
