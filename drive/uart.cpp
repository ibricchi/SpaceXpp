#include "uart.h"
#include "Arduino.h"

// Constructor
UART::UART(){

  instruction = -1;
  newUARTDataAvailable = false;
  if(!Serial.available()) Serial.begin(9600);
}

// Getters
int UART::getInstruction(){ return instruction; }
bool UART::getNewUARTDataAvailable(){ return newUARTDataAvailable; }
char* UART::getReceivedUARTCharts(){ return receivedUARTChars; }

// Receving new instruction from UART
void UART::recvUARTWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte idx = 0;
  const char startMarker = '<';
  const char endMarker = '>';
  char received;

  while (Serial.available() && !newUARTDataAvailable) {
    received = Serial.read();

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
        instruction = 3;
        break;
      case 'R':
        instruction = 5;
        break;
      case 'L':
        instruction = 6;
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
  Serial.print("R");
}
