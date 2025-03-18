// Author: Lokesh Senthil Kumar
// main.c files controls the flow of DAC using SPI



#include "at89c51ed2.h"
#include "mcs51reg.h"
#include <stdint.h>
#include <stdio.h>
#include "uart.h"

/* DAC Control Pins */
#define cs_bar P1_3     // Chip Select
#define A_mask 0xF000              // DAC Channel A mask
#define active_mask 0x3000         // Active configuration mask
#define Gain_increase_mask 0xFFFF  // Mask for gain increase
#define Gain_decrease_mask 0x7FFF  // Mask for gain decrease

/* Wave Data  */
__xdata uint8_t static sine_wave[128] = {
    128, 131, 134, 137, 140, 144, 147, 150, 153, 156, 159, 162, 165, 168, 171, 174,
    177, 180, 182, 185, 188, 191, 194, 196, 199, 201, 204, 206, 209, 211, 214, 216,
    217, 215, 212, 210, 208, 205, 203, 200, 197, 195, 192, 189, 187, 184, 181, 178,
    175, 172, 169, 167, 164, 160, 157, 154, 151, 148, 145, 142, 139, 136, 133, 130,
    126, 123, 120, 117, 114, 111, 108, 105, 102,  99,  96,  92,  89,  87,  84,  81,
     78,  75,  72,  69,  67,  64,  61,  59,  56,  53,  51,  48,  46,  44,  41,  39,
     40,  42,  45,  47,  50,  52,  55,  57,  60,  62,  65,  68,  71,  74,  76,  79,
     82,  85,  88,  91,  94,  97, 100, 103, 106, 109, 112, 116, 119, 122, 125, 128,
};



/* Global Variables */
__xdata uint16_t counter = 0;
uint8_t gain = 1;  // Default gain setting

//SPI configuration initialization
void spi_init(void) {
    SPCON |= 0x10;   // Master mode
    P1_1=1;
    SPCON |= 0x82;   // Fclk Periph / 128
    SPCON &= ~0x08;  // CPOL = 0
    SPCON |= 0x04;   // CPHA = 1
    SPCON |= 0x40;   // Enable SPI
    SPCON |= 0x20;
    
}

/**
 * @brief Send a 16-bit command word to the DAC via SPI.
 * 
 * @param command_word The 16-bit data to send.
 */
void spi_send_word(uint16_t command_word) {
    uint8_t high_byte = (command_word >> 8) & 0xFF;
    uint8_t low_byte = command_word & 0xFF;

    // Send high byte
    SPDAT = high_byte;
    while (!(SPSTA & 0x80));  // Wait for transmission to complete
    SPSTA &= ~0x80;           // Clear interrupt flag

    // Send low byte
    SPDAT = low_byte;
    while (!(SPSTA & 0x80));  // Wait for transmission to complete
    SPSTA &= ~0x80;           // Clear interrupt flag
}

/**
 * Update the DAC output for Channel A.
 */
void dac_update_output(void) {
   
    uint16_t command_word = (sine_wave[counter] << 4) | A_mask | active_mask;

    // Apply gain
    if (gain == 2) {
        command_word &= Gain_increase_mask;
    } else {
        command_word &= Gain_decrease_mask;
    }
    
    
    cs_bar = 0;               // Select the DAC
    spi_send_word(command_word);  // Send the command
    cs_bar = 1;               // Deselect the DAC

    // Increment waveform counter
    counter = (counter + 1) % 128;
    //printf("count %d", counter);
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

//interrupt handler for the timer 0
void wave_interrupt_handler(void) __interrupt(1)
{
    TF0 = 0;
    TL0 = 0x00;
    TH0 = 0xFC;
    dac_update_output();

}

//initialized the interrupt for generating the wave signal
void waves_init(void)
{
    IEN0 |= 0x82;
    TMOD |= 0x01;
    TMOD &= 0xF1;
    TL0 = 0x00;
    TH0 = 0xFC;
    TR0 = 1;
    return;
}

/* Main Function */
void main(void) {
    __xdata uint8_t key_pressed;

    initialize_UART();  // Initialize UART for user input
    spi_init();         // Initialize SPI module
    waves_init();

    printf("\n\rWelcome to DAC Wave generator");
    printf("\n\rCommands: \n\r'+'-> Increase the Voltage, \n\r'-'-> Decrease the Voltage, \n\r'?'-> HELP");

    

    while (1) {
        //get the character to increase or decrese the voltage
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
                printf("\n\rCommands: \n\r'+'-> Increase the Voltage, \n\r'-'-> Decrease the Voltage, \n\r'?'-> HELP");
                break;
            default:
                printf("\n\rInvalid Command");
                break;
        }
    }
}