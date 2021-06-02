#ifndef UART_H
#define UART_H

#include "Arduino.h"

// List of all the possible instructions
enum instructions {doNothing, forwardForTime, backwardForTime, forwardForDistance, backwardForDistance, turnR, turnL};

class UART{
  private:
    // UART constants
    static const byte numUARTChars = 40;
    instructions instruction;
    char receivedUARTChars[numUARTChars];
    bool newUARTDataAvailable;


  public:
    // Default constructor
    UART();

    void setup();

    // Getters and setters
    instructions getInstruction();
    bool getNewUARTDataAvailable();
    float getReceivedUARTCharts();
    void setInstruction(instructions i);

    // Receving new instruction from UART
    void recvUARTWithStartEndMarkers();

    // Decoding the current instruction alongside its paramater
    void processNewUARTData();

    // Ready for next instruction signal => should be sent after finished with current drive instruction
    void nextInstructionReady();
};

#endif
