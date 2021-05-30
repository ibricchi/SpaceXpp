#ifndef UART_H
#define UART_H

#include "Arduino.h"

class UART{
  private:
    // UART constants
    const byte numUARTChars = 40;
    int instruction;
    char* receivedUARTChars;
    bool newUARTDataAvailable;

  public:
    // Default constructor
    UART();

    // Getters
    int getInstruction();
    bool getNewUARTDataAvailable();
    char* getReceivedUARTCharts();

    // Receving new instruction from UART
    void recvUARTWithStartEndMarkers();

    // Decoding the current instruction alongside its paramater
    void processNewUARTData();

    // Ready for next instruction signal => should be sent after finished with current drive instruction
    void nextInstructionReady();
};

#endif
