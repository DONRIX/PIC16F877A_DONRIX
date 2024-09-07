#include "dht11.h"
#include <xc.h>

#define _XTAL_FREQ 20000000  // Define the crystal frequency

void DHT11_Init(void) {
    TRISC0 = 1;  // Set RC0 as input
}

void DHT11_Start(void) {
    TRISC0 = 0;  // Set RC0 as output
    DHT11_PIN = 0;  // Pull the pin low
    __delay_ms(18);  // Wait for 18 ms
    DHT11_PIN = 1;  // Pull the pin high
    __delay_us(20);  // Wait for 20-40 us
    TRISC0 = 1;  // Set RC0 as input
}

char DHT11_Check_Response(void) {
    __delay_us(40);
    if (!DHT11_PIN) {
        __delay_us(80);
        if (DHT11_PIN) {
            __delay_us(40);
            return 1;
        }
    }
    return 0;
}

char DHT11_Read_Data(void) {
    char i, data = 0;
    for (i = 0; i < 8; i++) {
        while (!DHT11_PIN);  // Wait until pin goes high
        __delay_us(30);  // Wait for data bit start
        
        if (DHT11_PIN)  // If pin is still high after 30 us, it?s a '1'
            data = ((data << 1) | 1);
        else  // If pin is low, it?s a '0'
            data = (data << 1);
        
        while (DHT11_PIN);  // Wait until pin goes low
    }
    return data;
}
