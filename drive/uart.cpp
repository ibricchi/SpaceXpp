/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#include "uart.h"

// Constructor
UART::UART(){
  instruction = doNothing;
  newUARTDataAvailable = false;
}

void UART::setup(){
  if(!Serial1.available()) Serial1.begin(115200);
}

// Getters and setters
instructions UART::getInstruction(){ return instruction; }
bool UART::getNewUARTDataAvailable(){ return newUARTDataAvailable; }
float UART::getReceivedUARTCharts(){ return atof(receivedUARTChars); }
void UART::setInstruction(instructions i){ instruction = i; }

// Receving new instruction from UART
void UART::recvUARTWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte idx = 0;
  const char startMarker = '<';
  const char endMarker = '>';
  char received;

  while (Serial1.available() && !newUARTDataAvailable) {
    received = Serial1.read();

    if (recvInProgress) {
      if (received != endMarker) {
        receivedUARTChars[idx++] = received;

        if (idx >= numUARTChars) {
          idx = numUARTChars - 1;
          Serial.print("ERROR: UART character buffer not large enough\n");
        }
      } else {
        receivedUARTChars[idx] = '\0'; // terminate the string
        recvInProgress = false;
        idx = 0;
        newUARTDataAvailable = true;
      }
    } else if (received == startMarker) {
      recvInProgress = true;
    }
  }
}

// Decoding the current instruction alongside its paramater
void UART::processNewUARTData() {
  if (newUARTDataAvailable) {
    // Read and pop instruction key
    const char instructionKey = receivedUARTChars[strlen(receivedUARTChars) - 1];
    receivedUARTChars[strlen(receivedUARTChars) - 1] = '\0';

    switch (instructionKey) {
      case 'F':
        instruction = forwardForDistance;
        break;
      case 'B':
        instruction = backwardForDistance;
        break;
      case 'R':
        instruction = turnR;
        break;
      case 'L':
        instruction = turnL;
        break;
      case 'S':
        instruction = stopAbruptly;
        break;
      default:
        Serial.print("INVALID INSTRUCTION KEY\n");
        break;
    }
    newUARTDataAvailable = false;
  }
}

// Ready for next instruction signal => should be send after finished with current drive instruction
void UART::nextInstructionReady() {
  Serial1.print("R");
}
