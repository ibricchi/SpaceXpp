/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#ifndef UART_H
#define UART_H

#include "Arduino.h"

// List of all the possible instructions
enum instructions {doNothing, forwardForTime, backwardForTime, forwardForDistance, backwardForDistance, turnR, turnL, stopAbruptly};

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
