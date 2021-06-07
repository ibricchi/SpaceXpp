/*
   Program originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020.
   Edited code taken from MS Teams for the Summer Electronics Design Project 2 2021
   Final code written by Benjamin Ramhorst and Nicholas Pfaff (UART code)
*/

#include "smps.h"
#include "move_hl.h"
#include "move_ll.h"
#include "uart.h"
#include "optical_flow.h"

// Control frequency (integer multiple of switching frequency, 62.5kHz)
float Ts = 0.0008;

// Reference voltage - used for controlling the speed of the rover; likely to be removed once speed controller is fully functional
float vref = 3.0;

// Displacement in each direction, [cm]
OpticalFlow opticalFlow;
float displacementX = 0.0, displacementY = 0.0;

// UART
UART uart;

// The current instruction to be executed
instructions currentInstruction = doNothing;
float receivedUARTChars = 0.0;
bool currentInstructionCompleted = false;
bool currentInstructionStarted = false;
unsigned long currentInstructionTime = 0;
float currentInstructionX = 0.0;
float currentInstructionY = 0.0;

// For velocity control, iterative proportional controller - to be fully implemented
float kvp = 0.1;   // TO BE TUNED
float currentY = 0.0;
float previousY = 0.0;
float currentTime = 0.0;
float previousTime = 0.0;

void setup() {
  Serial.begin(38400);
  SMPSSetup();
  driveInit();
  if (!opticalFlow.setup()) {
    Serial.println("Failed to initialise optical flow sensor");
  }
  uart.setup();
  uart.nextInstructionReady();
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  SMPSControl(vref, Ts);

  // Received and decode current instruction
  uart.recvUARTWithStartEndMarkers();
  uart.processNewUARTData();
  currentInstruction = uart.getInstruction();
  receivedUARTChars = uart.getReceivedUARTCharts();

  // Read the current position of the rover
  opticalFlow.read();
  displacementX = opticalFlow.getDisplacementX();
  displacementY = opticalFlow.getDisplacementY();

  // Set velocity to the desired value - commented out until velocity is accurately calculated using time
  // setVelocity(1.0);

  // Buffer through the instructions in order they arrive
  if (currentInstruction != doNothing && currentInstruction != stopAbruptly) {
    if (!currentInstructionStarted) {
      startInstruction();
    } else {
      if (!currentInstructionCompleted) {
        currentInstructionCompleted = callCurrentInstruction();
      } else {
        completeInstruction();
      }
    }
  } else if (currentInstruction == stopAbruptly) {
    currentInstructionStarted = false;
    uart.setInstruction(doNothing);
    stopMoving();

    // Needed to allow the ESP to be flashed again
    delay(100);

    // Return distance moved to ESP
    Serial1.print(String(abs(displacementY - currentInstructionY)) + "S");
    
    // Delay and wait for next instruction
    delay(2000);
    uart.nextInstructionReady();
  } else {
    uart.setInstruction(doNothing);
    stopMoving();
  }

  // Control circuit frequency (SMPS)
  delayMicroseconds(800);
}

// Decides which and calls the given instruction based on the mapping below
boolean callCurrentInstruction() {
  switch (currentInstruction) {
    case forwardForTime:
      return false;
      //return moveForwardForTime(10000, currentInstructionTime); IMPLEMENTED, NOT USED
    case backwardForTime:
      return false;
     //return moveBackwardForTime(10000, currentInstructionTime); IMPLEMENTED, NOT USED
    case forwardForDistance:
      return moveForwardForDistance(receivedUARTChars, currentInstructionY, displacementY);
    case backwardForDistance:
      return moveBackwardForDistance(receivedUARTChars, currentInstructionY, displacementY);
    case turnR:
      return turnRight(displacementX, displacementY, currentInstructionX, currentInstructionY);
    case turnL:
      return turnLeft(displacementX, displacementY, currentInstructionX, currentInstructionY);
    default:
      return false;
  }
}

// Starts the current instruction
void startInstruction() {
  currentInstructionStarted = true;
  currentInstructionCompleted = false;
  currentInstructionTime = millis();
  currentInstructionX = displacementX;
  currentInstructionY = displacementY;
}

// Completes the current instruction and allows starting the next instruction
void completeInstruction() {
  currentInstructionStarted = false;
  uart.setInstruction(doNothing);
  uart.nextInstructionReady();
}

/*
   Velocity control functions - to be implemented
*/
/*
  // Sets the speed of the rover
  void setVelocity(float velocityReference) {
  previousY = currentY;
  currentY = displacementY;
  previousTime = currentTime;
  currentTime = millis();
  float velocityCurrent = (currentY - previousY) / (currentTime - previousTime);
  float velocityError = velocityReference - velocityCurrent;
  vref += kvp * velocityError;
  }
*/
