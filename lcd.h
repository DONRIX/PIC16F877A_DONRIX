#ifndef LCD_H
#define LCD_H

#include <xc.h>
#define _XTAL_FREQ 20000000
#define RS PORTBbits.RB0
#define RW PORTBbits.RB1
#define EN PORTBbits.RB2
void lcd_Clear();
void lcd_Set_Cursor(unsigned char row, unsigned char column);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_Init();
void lcd_char(char data);
void lcd_word(const unsigned char *words);
void delay(unsigned int Delay);

#endif
