// Author: Lokesh Senthil Kumar
// led.c file controls the flow of LCD functions

#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "at89c51ed2.h"

#include "uart.h"

// Pin definitions for the LCD
#define RS P1_2
#define RW P1_3

// LCD memory addresses for each row
#define LCD_ROW_0_ADDR 0x00
#define LCD_ROW_1_ADDR 0x40
#define LCD_ROW_2_ADDR 0x10
#define LCD_ROW_3_ADDR 0x50

// The number of cycles required to wait for one millisecond
#define one_MS       82

// The address used to read LCD commands
#define LCD_COMMAND_READ_ADDRESS    (uint8_t *)0xF000

// The LCD read pointer
volatile uint8_t __at(LCD_COMMAND_READ_ADDRESS) lcd_ptr;

// Global variables used to display time on the LCD
extern volatile char tenth_of_second;
extern volatile char seconds_ones_digit;
extern volatile char seconds_tens_digit;
extern volatile char minutes_ones_digit;
extern volatile char minutes_tens_digit;

// The cursor address saved for later use
uint8_t save_cursor_address = 0;

// The string pointer
char *string;

// Function to implement a delay for a specified number of milliseconds
void delay(int millisec)
{
   
    for(int i = 0; i < millisec * one_MS; i++){};
}


// Wait for the LCD to become not busy
void BUSY_WAIT(void)
{
    
    const uint8_t BF_MASK = 0x80;

    // Address of the LCD command register
    const uint16_t COMMAND_LOC = 0xF000;

    // Set control pins for reading command
    RS = 0;
    RW = 1;

    // Read status register until busy flag is cleared
    while ((* (volatile uint8_t *) COMMAND_LOC) & BF_MASK){
        
        delay(10);
    }
}

// Set initial values of global variables used to display time on the LCD
void INIT_TIME(void)
{
  
    tenth_of_second     = '0';
    seconds_ones_digit  = '0';
    seconds_tens_digit  = '0';
    minutes_ones_digit  = '0';
    minutes_tens_digit  = '0';
}

void init_lcd(void){
    // Set RS and RW pins to 0
    RS = 0;
    RW = 0;
    // Delay for 160 ms
    delay(160);

    // Send 0x30 to the LCD
    lcd_ptr = 0x30;
    delay(170);
    lcd_ptr = 0x30;
    delay(150);

    lcd_ptr = 0x30;
    BUSY_WAIT();
    // Set RS and RW pins to 0
    RS = 0;
    RW = 0;
    lcd_ptr = 0x38;
    // Wait until the LCD is not busy
    BUSY_WAIT();

    // Set RS and RW pins to 0
    RS = 0;
    RW = 0;
    lcd_ptr = 0x0F;
   
    BUSY_WAIT();

    // Set RS and RW pins to 0
    RS = 0;
    RW = 0;
    lcd_ptr = 0x01;
    
    BUSY_WAIT();

    // Set RS and RW pins to 0
    RS = 0;
    RW = 0;
    // Send 0x06 to the LCD
    lcd_ptr = 0x06;
    
    BUSY_WAIT();

    // Call INIT_TIME function to initialize time variables
    INIT_TIME();
}

// Function to get the cursor address
uint8_t get_cursor_address(){
    RS = 0; // Set RS pin low
    RW = 1; // Set RW pin high

    uint8_t address = lcd_ptr & (~0x80);
    return address; // Return the address
}

// Function to move the cursor to the given address
void lcdgotoaddr(unsigned char address){
    RS = 0; // Set RS pin low
    RW = 0; // Set RW pin low

    address = address | 0x80; // Set the MSB of address to 1
    lcd_ptr = address; // Assign the address to lcd_ptr
    BUSY_WAIT(); // Wait for the LCD to be not busy
}

// Function to move the cursor to the given row and column
void lcdgotoxy(unsigned char row, unsigned char column) {
    unsigned char addr;  // Declare address variable
    switch (row) {          // Select address based on the row number
        case '0':
            addr = (0x00);
            break;
        case '1':
            addr = (0x40);
            break;
        case '2':
            addr = (0x10);
            break;
        case '3':
            addr = (0x50);
            break;
        default:
            addr = (0x00);
            break;
    }
    
    if (column >= 'A' && column <= 'F') {
        addr |= (column - '7');
    }
    else if (column >= '0' && column <= '9') {
        addr |= (column - '0');
    }
    else {
        addr |= ('8' - 48);
    }
    lcdgotoaddr(addr);   // Call lcdgotoaddr function with the calculated address
    BUSY_WAIT();          // Wait for the LCD to be not busy
}


void lcdputch(char cc){
    unsigned char address = get_cursor_address(); // get the current cursor address
    RS=1;       
    RW=0;       
    lcd_ptr=cc;     // character to the LCD data bus
    BUSY_WAIT();  // wait LCD is not busy

    // check cursor position and move cursor to next line if needed
    switch(address) {
        case 0x0F:
            lcdgotoxy('1', '0');
            break;
        case 0x1F:
            lcdgotoxy('0', '0');
            break;
        case 0x4F:
            lcdgotoxy('2', '0');
            break;
        case 0x5F:
            lcdgotoxy('0', '0');
            break;
    }
}

void lcdputstr(char *ss){
    int i=0;
    while(ss[i]!='\0'){     // loop until end of string
        lcdputch(ss[i]);    
        i++;
    }
}
void handler_lcdclear(void){
    RS=0;               // set RS pin to low
    RW=0;               // set RW pin to low
    lcd_ptr=0x01;          
    BUSY_WAIT();          // wait until LCD is ready
    lcdgotoaddr(0x00);      // move cursor to the beginning
    lcdputstr("       ");   // write 7 spaces to clear the first line
    lcdgotoaddr(0x00);      // move cursor back to the beginning
    printf("\n\rLCD Cleared!!\r\n"); // LCD has been cleared
}

void handler_wr_c_lcd(void)
{
    char LCD_int;
    printf("\n\rEnter Character for LCD !!\r\n"); // print a message to ask the user to enter a character
    LCD_int = getchar();  // get the input character from the user
    __critical
    {
        lcdputch(LCD_int); // write the input character to the LCD
    }
    printf("\n\rEntered Char = %c\n\r\n\r",LCD_int); // print the entered character
    
}

void handler_wr_str_lcd(void)
{
    printf(" \n\rEnter String for LCD !!\r\n"); // print a message to ask the user to enter a string
    int k = 1;
    int i = 0;
    char ch = 0;

    while(k){
        ch=getchar();       //  until the enter key is pressed
        if(ch=='\r'){
            k=0;            // exit the loop when the enter key is pressed
        }
        else{
            *(string+i)=ch; // store each characte
            i++;
        }
    }
    *(string+i)='\0';       // add null character
    __critical
    {
        lcdputstr(string);  // write the string to the LCD
    }
    printf("Entered String = %s\n\r\n\r",string); // print the entered string
    
}

// This function is used to handle the user input to move the cursor on the LCD to the specified coordinates
void handler_lcdgotoxy(void)
{
    // user to input the x-coordinate
    char x_coordinate_ch;
    printf(" \n\rEnter X-Co-ordinate for LCD !!\r\n");
    x_coordinate_ch  = toupper(getchar());

    // print the user input for x-coordinate
    printf("X-Cordinate = %c\n\r",x_coordinate_ch);

    // user to input the y-coordinate
    printf(" \n\rEnter Y-Co-ordinate for LCD !!\r\n");
    char y_coordinate_ch  = toupper(getchar());

    // user input for y-coordinate
    printf("Y-Cordinate = %c\n\r",y_coordinate_ch);

    // if the user input is not a valid coordinate
    if (x_coordinate_ch >= '0' && x_coordinate_ch <= '3'){
    } else if (y_coordinate_ch >= '0' && y_coordinate_ch <= 'F') {
    } else {
        printf("Invalid coordinate!!\n\r");
        return;
    }

    // move the cursor to the specified coordinates on the LCD
    __critical {
        lcdgotoxy(x_coordinate_ch, y_coordinate_ch);
    }

    // print the message indicating the cursor movement completed
    printf(" \n\rCursor Movement Completed!!\r\n");
}

long int hex_to_int(char *hex_str) {
    long int result = 0;  
    int i;            

    // Check if the string starts with "0x" or "0X" and skip the prefix
    if (hex_str[0] == '0' && (hex_str[1] == 'x' || hex_str[1] == 'X')) {
        i = 2;  // Start processing from the third character
    }

    // Loop through each character of the string until the null terminator is encountered
    while (hex_str[i] != '\0') {
        char c = hex_str[i];
        result *= 16;  

        // Check if the character is a digit (0-9)
        if (c >= '0' && c <= '9') {
            result += c - '0';  
        }
        // Check if the character is an uppercase letter (A-F)
        else if (c >= 'A' && c <= 'F') {
            result += c - 'A' + 10;  
        }
        // Check if the character is a lowercase letter (a-f)
        else if (c >= 'a' && c <= 'f') {
            result += c - 'a' + 10;  // Convert ASCII letter to numeric value (10-15)
        }
        // If the character is not valid, return an error (optional)
        else {
            return -1;  // Return -1 to indicate an invalid hexadecimal string
        }

        i++;  // Move to the next character
    }

    return result;  // Return the final converted value
}

// This function handles the command to go to a specific address on the LCD
void handler_lcdgotoaddress(void)
{
    char str[50];
    printf(" \n\rEnter address for LCD !!\r\n");

    // Read in the address string from the user
    int i = 0;
    char c = getchar();
    putchar(c);
    while (c != '\r' && i < 49) { // Read characters until enter is pressed or limit is reached
        str[i++] = c;
        c = getchar();
    }
    str[i] = '\0';

   
    printf("Address Entered as %s\n\r",str);

    // Convert the address string to a long integer using base 16
    long int num = hex_to_int(str);

    if (!((num >= 0x00 && num <= 0x0F) ||   
          (num >= 0x40 && num <= 0x4F) ||   
          (num >= 0x10 && num <= 0x1F) ||  
          (num >= 0x50 && num <= 0x5F))) 
    {  
        printf("\n\rInvalid address for a LCD.\n\r");
        return;
    }
    // Go to the specified address on the LCD
    __critical
    {
        lcdgotoaddr((char)num);
    }
    return;
}

// This function handles the command to stop the clock
void handler_stop_time(void)
{
    printf(" \n\rTime Paused !!\r\n");

    // Set the timer control register bit 4 to 0 to stop the timer
    TCON &=~(0x10);
}

// This function handles the command to resume the clock
void handler_resume_time(void)
{
    //resume clock
    printf(" \n\rTime Resumed !!\r\n");
    TCON |=0x10;
}

// Function to reset the timer
void handler_reset_time(void)
{
    printf(" \n\rTime Reset !!\r\n");

    tenth_of_second     = '0';
    seconds_ones_digit  = '0';
    seconds_tens_digit  = '0';
    minutes_ones_digit  = '0';
    minutes_tens_digit  = '0';
}

// Function to read from a specific address in the LCD
uint8_t read_lcd_address(uint8_t is_ddram, uint8_t address) {
    // Set address to read from
    if (is_ddram == 1) {
        lcdgotoaddr(address);
    } else {
        RS = 0;
        RW = 0;
        lcd_ptr = address;
        BUSY_WAIT();
    }

    // Set LCD to read mode
    RS = 1;
    RW = 1;

    // Return the value stored at the specified address
    return lcd_ptr;
}
void handler_lcd_hexdump(void)
{
    __critical{
        save_cursor_address=get_cursor_address();       // Save the current cursor address
        printf("\n\rPrinting Hexdump of DDRAM\n\r");

        // Loop through DDRAM addresses and print hex values
        for(uint16_t k=0x00;k<=0x5F;k++){
            if(k%16 == 0){                              // Print new line and address every 16 bytes
                printf("\n\r0x%02x: ",k);
            }
            printf("0x%02x ",read_lcd_address(1,k));    // Read and print the value at the current address
        }

        printf("\n\r\n\rPrinting Hexdump of CGRAM\n\r");

        // Loop through CGRAM addresses and print hex values
        for(uint16_t k=0x40;k<=0x7F;k++){
            if(k%16 == 0){                              // Print new line and address every 16 bytes
                printf("\n\r0x%02x: ",k);
            }
            printf("0x%02x ",read_lcd_address(0,k));    // Read and print the value at the current address
        }

        printf("\n\r");
        lcdgotoaddr(save_cursor_address);               // Restore the original cursor position

    }
}

unsigned char get_hex_value(void) {
    unsigned char digit1, digit2, hex_value;
    printf("\n\rEnter a hexadecimal value between (00 to 1F) or (40 to 58): ");

    while (1) {                         //loop until a valid input is entered
        digit1 = putchar(getchar());    //get first digit from input and display it
        digit2 = putchar(getchar());    //get second digit from input and display it

        //check if both digits are valid hexadecimal digits (0-9 or A-F)
        if (digit1 >= '0' && digit1 <= '9' && digit2 >= '0' && digit2 <= '9') {
            hex_value = ((digit1 - '0') << 4) | (digit2 - '0'); //convert the digits to a hexadecimal value
            break; //break the loop
        }
        //check if the first digit is valid and the second digit is a valid hexadecimal digit (A-F)
        else if ((digit1 == '0' || digit1 == '1' || digit1 == '4' || digit1 == '5') && (digit2 >= 'A' && digit2 <= 'F')) {
            hex_value = ((digit1 - '0') << 4) | (digit2 - '7'); //convert the digits to a hexadecimal value
            break; //break the loop
        }
        else {
            printf("\n\rInvalid input. ");      //display error message if input is invalid
        }
        printf("Please enter a valid input: "); //prompt user to enter a valid input
    }
    return hex_value;                           //return the hexadecimal value
}
void create_custom_char(unsigned char code, unsigned char rows[]) {
    // Define some variables
    unsigned char six_bit = 0x40;
    unsigned char code_val = code - '0';
    unsigned char third_bit = code_val << 3;
    unsigned char char_val = six_bit | third_bit;
    int i =0;

    // Print a message indicating which character has been created
    printf("\n\rThe custom character  0x%x been created.", char_val);

   
    while (i < 8) {
        // Set the RS and RW pins low
        RS = 0;
        RW = 0;
        // Set the LCD pointer to the appropriate memory location
        lcd_ptr = char_val + i;
        
        printf("\n\rLCD_ptr = %x", char_val + i);
        // Wait for the LCD to become available
        BUSY_WAIT();
       
        RS = 1;
        RW = 0;
        lcd_ptr = rows[i];
        BUSY_WAIT();
        // Increment the row counter
        i++;
    }
}

void handler_custom_char(void) {

    uint8_t j;
    // Get current cursor address and save it in a variable
    unsigned int addr = get_cursor_address();

    printf("Enter the code for the custom character: ");
    // Get the code for the custom character from the user
    unsigned char code = getchar();

    // Print the code of the custom character entered by the user
    printf("%c\n\r", code);

    // Initialize an array to store the values for the custom character rows
    unsigned char rows[8];

    
    while (j < 8) { // Loop through each row of the custom character and get its value from the user
        printf("\n\rEnter the value for the row %d: ", j);
        rows[j] = get_hex_value();
        j++;
    }

    __critical { // Enter a critical section to prevent interruption
        // Call the function to create the custom character on the LCD
        create_custom_char(code, rows);

        // Call the handler function for the LCD gotoxy command
        handler_lcdgotoxy();

        // Display the custom character on the LCD screen
        lcdputch(code - '0');

        // Move the cursor to the original position before the custom character was created
        lcdgotoaddr(addr);
    }

}

void handle_cu_custom_char(void)
{
    save_cursor_address = get_cursor_address();     // Get current cursor address and save it in a variable

    // Create custom character 1
    unsigned char ccode1 = '1';
    unsigned char row_vals1[8] = {0x00, 0x00, 0x0F, 0x08, 0x08, 0x09, 0x09, 0x09};
    __critical {
        create_custom_char(ccode1, row_vals1);      // Call function to create custom character on the LCD
        lcdgotoxy('1', '4');                        // Move cursor to position to display custom character 1
        lcdputch(ccode1 - '0');                     // Display custom character 1 on the LCD screen
    }

    // Create custom character 2
    unsigned char ccode2 = '2';
    unsigned char row_vals2[8] = {0x00, 0x00, 0x18, 0x00, 0x00, 0x02, 0x02, 0x02};
    __critical {
        create_custom_char(ccode2, row_vals2);      // Call function to create custom character on the LCD
        lcdgotoxy('1', '5');                        // Move cursor to position to display custom character 2
        lcdputch(ccode2 - '0');                     // Display custom character 2 on the LCD screen
        lcdgotoaddr(save_cursor_address);           // Move the cursor back to the original position
    }

    // Create custom character 3
    unsigned char ccode3 = '3';
    unsigned char row_vals3[8] = {0x09, 0x09, 0x09, 0x0F, 0x01, 0x01, 0x00, 0x00};
    __critical {
        create_custom_char(ccode3, row_vals3);      // Call function to create custom character on the LCD
        lcdgotoxy('2', '4');                        // Move cursor to position to display custom character 3
        lcdputch(ccode3 - '0');                     // Display custom character 3 on the LCD screen
        lcdgotoaddr(save_cursor_address);           // Move the cursor back to the original position
    }

    // Create custom character 4
    unsigned char ccode4 = '4';
    unsigned char row_vals4[8] = {0x02, 0x02, 0x02, 0x1A, 0x02, 0x1E, 0x00, 0x00};
    __critical {
        create_custom_char(ccode4, row_vals4);      // Call function to create custom character on the LCD
        lcdgotoxy('2', '5');                        // Move cursor to position to display custom character 4
        lcdputch(ccode4 - '0');                     // Display custom character 4 on the LCD screen
        lcdgotoaddr(save_cursor_address);           // Move the cursor back to the original position
    }

}



void print_board_name(void)
{

    char * str;
    str = "8051 DEV BOARD";

    // critical section to prevent interruption while writing to LCD
    __critical
    {
        // move cursor to beginning of first line on LCD and print the string
        lcdgotoaddr(0x00);
        lcdputstr(str);

        // move cursor to beginning of first line on LCD (in case string is longer than display width)
        lcdgotoaddr(0x00);
    }
    return;
}



