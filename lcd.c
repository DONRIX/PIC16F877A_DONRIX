#include "lcd.h"
#include <xc.h>

#define _XTAL_FREQ 20000000  // Define the crystal frequency

void delay(unsigned int Delay)
{
while(Delay--);
}

void lcd_Init() {
    lcd_cmd(0x38);  // 8-bit mode, 2 line, 5x7 matrix
    lcd_cmd(0x06);  // Increment cursor (shift cursor to right)
    lcd_cmd(0x0C);  // Display on, cursor off
    lcd_cmd(0x01);  // Clear display screen
    lcd_cmd(0x80);  // Force cursor to beginning (1st line)
}

void lcd_cmd(unsigned char cmd) {
    PORTD = cmd;    // Send command to data bus
    EN = 1;         // Enable high
    RS = 0;         // Register Select to command
    RW = 0;         // Write operation
    delay(500);     // Delay to allow the command to process
    EN = 0;         // Enable low
    __delay_ms(2);  // Additional delay for command processing
}

void lcd_data(unsigned char data) {
    PORTD = data;   // Send data to data bus
    EN = 1;         // Enable high
    RS = 1;         // Register Select to data
    RW = 0;         // Write operation
    delay(500);     // Delay to allow the data to process
    EN = 0;         // Enable low
    __delay_ms(2);  // Additional delay for data processing
}

void lcd_word(const unsigned char *words) {
    while (*words) {
        lcd_data(*words++);  // Send each character to lcd_data()
    }
}
void lcd_char(char data) {
    lcd_data(data);  // Call lcd_data function to send a single character
}
void lcd_Clear() {
    lcd_cmd(0x01);  // Clear display
    __delay_ms(2);  // Wait for the command to complete
}

void lcd_Set_Cursor(unsigned char row, unsigned char column) {
    unsigned char position;
    switch(row) {
        case 1:
            position = 0x80 + (column - 1);
            break;
        case 2:
            position = 0xC0 + (column - 1);
            break;
        default:
            position = 0x80 + (column - 1);
            break;
    }
    lcd_cmd(position);  // Set cursor position
}
