/*
 * Arduino Slave Setup
 */

#include "SPI.h"

char buff[50];

// volatile to prevent wrong compiler optimization (changed unexpectedly inside interrupt routine)
volatile byte idx;
volatile boolean received; // state

void setup() {
   Serial.begin(9600); // for logging
   
   pinMode(MISO, OUTPUT); // Required as SPI.h only supports master mode out of the box
   SPCR |= _BV(SPE);  // turn on SPI in slave mode
   SPI.attachInterrupt(); // turn on interrupt
   
   idx = 0; // buffer empty
   received = false;
}

// SPI interrupt routine 
ISR(SPI_STC_vect) {
  byte c = SPDR; // read byte from SPI Data Register
  Serial.print("Byte read: "); Serial.println(c);
  if (idx < sizeof(buff)) {
    if (c == ':') { // check for end of word delimiter
      received = true;
    } else {
      buff[idx++] = c; // save data in the next index in the array buff
    }
  }
}

void loop() {
  // check if finished reading
  if (received) {
    SPDR = 222; // send data
    Serial.print("Data received: ");
    Serial.println(buff);
    
    received = false;
    idx = 0;
    memset(buff, 0, sizeof(buff));
  }
}
