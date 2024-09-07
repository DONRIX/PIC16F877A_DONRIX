#include "rtc.h"
#include <xc.h>
#include "lcd.h"
unsigned char __sec, __min, __hr, __day, __date, __month, __yr, __con;

char decimal_to_bcd(unsigned char value){ // DS1307 STORES STUFF IN BCD
    unsigned char msb, lsb, hex;        //eg say 45
    msb = value / 10;           // 45/10 = 0x04  (quotient))
            
    lsb = value % 10;           // 45%10 = 0x05 (remaineder)
    hex = ((msb << 4) + lsb);       // left shifting 0x04 makes it 0x40 , an then 0x40+0x05
    return hex;             // returns 0x45 which the hex here but to the code its in bcd ( hex only for easy understanding)
}

void bcd_to_ascii(unsigned char value)         // LCD WANTS ASCII
{
    unsigned char bcd;
    bcd = value;
    bcd = bcd & 0xf0;
    bcd = bcd >> 4;
    bcd = bcd | 0x30;
    lcd_data(bcd);
    bcd = value;
    bcd = bcd & 0x0f;
    bcd = bcd | 0x30;
    lcd_data(bcd);
}

void DS1307_write(char _second, char _minute, char _hour, char _day, char _date, char _month, char _year)
{
         /* start bit*/
    SSPCON2bits.SEN = 1;      // initiate start condition
    while(SEN);               // wait for start condition to complete
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    
    /* slave address bits*/
    SSPBUF = DS1307_address;    // send the slave address high and r/w = 0 for write
    while(!SSPIF);            // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    if(SSPCON2bits.ACKSTAT)   
    {  
        SSPCON2bits.PEN = 1;  // initiate stop condition 
        while(PEN);           // wait for stop condition to complete 
        return;               // exit write (no acknowledgment)
    }
    
    SSPBUF = second;    // send the slave address high and r/w = 0 for write
    while(!SSPIF);            // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    if(SSPCON2bits.ACKSTAT)   
    {  
        SSPCON2bits.PEN = 1;  // initiate stop condition 
        while(PEN);           // wait for stop condition to complete 
        return;               // exit write (no acknowledgment)
    }
    
    SSPBUF =  decimal_to_bcd(_second);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
    SSPBUF = decimal_to_bcd( _minute);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
    SSPBUF =  decimal_to_bcd(_hour);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
    SSPBUF = decimal_to_bcd(_day);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
 
    SSPBUF =  decimal_to_bcd(_date);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
    SSPBUF =  decimal_to_bcd(_month);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
    SSPBUF =  decimal_to_bcd(_year);          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
    SSPBUF = 0x00;          // send the data 
    while(!SSPIF);             // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;        // clear SSPIF flag
    
        /*  stop bits */
      SSPCON2bits.PEN = 1;   // initiate stop condition 
      while(PEN);            // wait for stop condition to complete 
}


void DS1307_read(char slave_address,char register_address)
{
          /* start bit*/
    SSPCON2bits.SEN = 1;      // initiate start condition
    while(SEN);               // wait for start condition to complete
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    
    /* slave address bits*/
    SSPBUF = slave_address;    // send the slave address high and r/w = 0 for write
    while(!SSPIF);            // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    if(SSPCON2bits.ACKSTAT)   
    {  
        SSPCON2bits.PEN = 1;  // initiate stop condition 
        while(PEN);           // wait for stop condition to complete 
        return;               // exit write (no acknowledgment)
    }
        /* slave address bits*/
    SSPBUF = register_address;    // send the slave address high and r/w = 0 for write
    while(!SSPIF);            // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    if(SSPCON2bits.ACKSTAT)   
    {  
        SSPCON2bits.PEN = 1;  // initiate stop condition 
        while(PEN);           // wait for stop condition to complete 
        return;               // exit write (no acknowledgment)
    }
          /* Repeated start bit*/
    SSPCON2bits.RSEN = 1;      // initiate start condition
    while(RSEN);               // wait for start condition to complete
    PIR1bits.SSPIF = 0;       // clear SSPIF flag 
    
    /* slave address bits*/
    SSPBUF = (slave_address + 1);    // send the slave address high and r/w = 1 bit
    while(!SSPIF);            // wait for acknowledge SSPIF is set for every 9th clock cycle
    PIR1bits.SSPIF = 0;       // clear SSPIF flag
    if(SSPCON2bits.ACKSTAT)   
    {  
        SSPCON2bits.PEN = 1;  // initiate stop condition 
        while(PEN);           // wait for stop condition to complete 
        return ;               // exit write (no acknowledgment)
    }
    
        /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __sec = SSPBUF;                  // clear SSPIF flag 
 
        
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __min = SSPBUF;                  // clear SSPIF flag 
   
    
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __hr = SSPBUF;                  // clear SSPIF flag 
   
    
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __day = SSPBUF;                  // clear SSPIF flag 

    
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __date = SSPBUF;                  // clear SSPIF flag 
  
    
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __month = SSPBUF;                  // clear SSPIF flag 
  
    
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __yr = SSPBUF;                  // clear SSPIF flag 
 
 
    SSPCON2bits.ACKDT = 0;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);      
    
            /* Receive enable bit*/
    SSPCON2bits.RCEN = 1;                  // initiate receive enable
    while(!SSPSTATbits.BF);               // wait for BUFFER TO BE FULL
    __con = SSPBUF;                  // clear SSPIF flag 
    
    SSPCON2bits.ACKDT = 1;     // Prepare to send NACK
    SSPCON2bits.ACKEN = 1;     // Initiate to send NACK 
    while(ACKEN);              // WAIT  FOR NACK TO COMPLETE
    
        /*  stop bits */
      SSPCON2bits.PEN = 1;   // initiate stop condition 
      while(PEN);            // wait for stop condition to complete 
      
      
    lcd_cmd(0x88);
    bcd_to_ascii(__hr);  
    lcd_data(':'); 
    bcd_to_ascii(__min);  
    lcd_data(':');   
    bcd_to_ascii(__sec);
    
    lcd_cmd(0xC7);
    bcd_to_ascii(__date);  
    lcd_data('/'); 
    
    bcd_to_ascii(__month);  
    lcd_data('/');
    
    bcd_to_ascii(__yr); 
}
