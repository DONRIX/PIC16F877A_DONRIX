#ifndef DHT11_H
#define DHT11_H

#include <xc.h>

// Define the DHT11 pin
#define DHT11_PIN PORTCbits.RC0

// Function prototypes
void DHT11_Init(void);
void DHT11_Start(void);
char DHT11_Check_Response(void);
char DHT11_Read_Data(void);

#endif
