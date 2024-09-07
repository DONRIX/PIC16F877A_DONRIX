#include <xc.h>
#include "rtc.h"
#include "lcd.h"
#include "dht11.h"

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

#define _XTAL_FREQ 20000000  // Define the crystal frequency
#define BUTTON_TOGGLE RB3    // Define the button on RB3
#define EEPROM_ADDRESS 0x00  // Define EEPROM address to store setup flag

// EEPROM Functions
void EEPROM_Write(unsigned char address, unsigned char data) {
    EEADR = address;  // Set EEPROM address
    EEDATA = data;    // Set data to write
    EECON1bits.WREN = 1;  // Enable EEPROM writes
    INTCONbits.GIE = 0;   // Disable global interrupts
    EECON2 = 0x55;        // Required sequence
    EECON2 = 0xAA;        // Required sequence
    EECON1bits.WR = 1;    // Start write
    INTCONbits.GIE = 1;   // Enable global interrupts
    while (EECON1bits.WR); // Wait for write to complete
    EECON1bits.WREN = 0;  // Disable EEPROM writes
}

unsigned char EEPROM_Read(unsigned char address) {
    EEADR = address;   // Set EEPROM address
    EECON1bits.RD = 1; // Start read
    while (EECON1bits.RD); // Wait for read to complete
    return EEDATA;     // Return read data
}

void main(void) {
    char temperature_integral, temperature_decimal;
    char humidity_integral, humidity_decimal;
    char checksum;
    char toggle_state = 0;
    char prev_button_state = 1; // Assume button is not pressed initially
    char rtc_setup_done;

    SSPADD = 49;
    SSPCON = 0x28;

    // Setup TRIS and PORT registers
    TRISC = 0xFF;  // Set PORTC as input for DHT11 and I2C
    TRISD = 0x00;  // Set PORTD as output for LCD
    PORTD = 0x00;  // Clear PORTD
    TRISB3 = 1;    // Set RB3 as input for the button
    TRISBbits.TRISB0 = 0x00;
    TRISBbits.TRISB1 = 0x00;
    TRISBbits.TRISB2 = 0x00;
    PORTB = 0x00;  // Clear PORTB

    // Initialize LCD
    lcd_Init();

    // Read EEPROM to check if RTC has been set up before
    rtc_setup_done = EEPROM_Read(EEPROM_ADDRESS);

//    if (rtc_setup_done != 1) {
//        // Initialize RTC (DS1307) only if not set up before
//        DS1307_write(3, 44, 21, 3, 3, 9, 24);  // Set initial time and date (hh, mm, ss, dow, dd, mm, yy)
//        __delay_ms(300);  // Small delay to ensure RTC is set
//
//        // Store a flag in EEPROM to indicate that RTC has been set up
//        EEPROM_Write(EEPROM_ADDRESS, 1);
//    }

    // Initialize DHT11
    DHT11_Init();

    while (1) {
        __delay_ms(20);  // Small delay to stabilize communication

        // Check if the button is pressed
        if (BUTTON_TOGGLE == 0 && prev_button_state == 1) {  // Button is active low and was previously not pressed
            __delay_ms(50);  // Debounce delay
            if (BUTTON_TOGGLE == 0) {
                toggle_state = !toggle_state;
                lcd_Clear();  // Toggle the display state
                __delay_ms(100);  // Prevent bouncing
            }
        }
        prev_button_state = BUTTON_TOGGLE;  // Update the previous button state

        if (toggle_state == 0) {
            // Display time and date from RTC
            lcd_cmd(0x80);  // Move cursor to the first line
            lcd_word("CLOCK:");
            lcd_cmd(0xC0);  // Move cursor to the second line
            lcd_word("DATE:");
            DS1307_read(DS1307_address, 0);  // Read time and date from RTC
            
            // Assuming you have functions to display time and date
            // Example:
            // lcd_display_time();
            // lcd_display_date();
        } else {
            // Display temperature and humidity from DHT11
            DHT11_Start();  // Start DHT11 sensor communication
            if (DHT11_Check_Response()) {  // Check for response from DHT11
                humidity_integral = DHT11_Read_Data();  // Read integral part of humidity
                humidity_decimal = DHT11_Read_Data();  // Read decimal part of humidity
                temperature_integral = DHT11_Read_Data();  // Read integral part of temperature
                temperature_decimal = DHT11_Read_Data();  // Read decimal part of temperature
                //checksum = DHT11_Read_Data();  // Read checksum

                // Verify checksum
                //if ((humidity_integral + humidity_decimal + temperature_integral + temperature_decimal) == checksum) {
                    // Display temperature and humidity on LCD
                    lcd_cmd(0x80);  // Move cursor to the first line
                    lcd_word("Temp: ");
                    lcd_char((temperature_integral / 10) + '0');  // Display tens place of temperature
                    lcd_char((temperature_integral % 10) + '0');  // Display ones place of temperature
//                    lcd_char('.');
//                    lcd_char((temperature_decimal / 10) + '0');  // Display decimal place of temperature
//                    lcd_char((temperature_decimal % 10) + '0');  // Display decimal place of temperature
                    lcd_word("C");

                    lcd_cmd(0xC0);  // Move cursor to the second line
                    lcd_word("Humidity: ");
                    lcd_char((humidity_integral / 10) + '0');  // Display tens place of humidity
                    lcd_char((humidity_integral % 10) + '0');  // Display ones place of humidity
//                    lcd_char('.');
//                    lcd_char((humidity_decimal / 10) + '0');  // Display decimal place of humidity
//                    lcd_char((humidity_decimal % 10) + '0');  // Display decimal place of humidity
                    lcd_word("%");
//                } else {
//                    // Handle checksum error
//                    lcd_cmd(0x80);
//                    lcd_word("Checksum Error");
//                }
            } else {
                // Handle response error
                lcd_cmd(0x80);
                lcd_word("No Response");
            }

            __delay_ms(2000);  // Wait before next reading
        }
    }
}
