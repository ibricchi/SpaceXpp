/*
   Program originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020.
   Edited code taken from MS Teams for the Summer Electronics Design Project 2 2021
   Final code written by Benjamin Ramhorst and Nicholas Pfaff (UART code)
*/

#include "smps.h"
#include "move_hl.h"
#include "uart.h"
#include "optical_flow.h"

// Control frequency (integer multiple of switching frequency, 62.5kHz)
float Ts = 0.0008;

// Reference voltage - used for controlling the speed of the rover; likely to be removed once speed controller is fully functional
float vref = 0.0;

// Displacement in each direction, [cm]
OpticalFlow opticalFlow;
float displacementX = 0.0, displacementY = 0.0;

// UART
UART uart;

// The current instruction to be executed
instructions currentInstruction = doNothing;
float receivedUARTChars;
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
      currentInstructionStarted = true;
      currentInstructionCompleted = false;
      currentInstructionTime = millis();
      currentInstructionX = displacementX;
      currentInstructionY = displacementY;
    } else {
      // This has room for improving efficiency; could cut down 2(?) cycles
      if (!currentInstructionCompleted) {
        currentInstructionCompleted = callCurrentInstruction();
      } else {
        currentInstructionStarted = false;
        currentInstruction = doNothing;
        uart.nextInstructionReady();
        uart.setInstruction(doNothing);
      }
    }
  } else if (currentInstruction == stopAbruptly) {
    stopMoving();
        
    // Needed to allow the ESP to be flashed again
    delay(100);

    uart.nextInstructionReady();
    // TODO - Add sending the distance moved to the ESP32 since the last instruction
  } else {
    stopMoving();
  }

  // Control circuit frequency (SMPS)
  delayMicroseconds(800);
}

// Decides and calls the current instruction based on the mapping below - these numbers are purely for testing; will be replaced with UART data
boolean callCurrentInstruction() {
  switch (currentInstruction) {
    case forwardForTime:
      return false;
    //return moveForwardForTime(10000, currentInstructionTime);
    case backwardForTime:
      return false;
    //return moveBackwardForTime(10000, currentInstructionTime);
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
  }*/

// Causes the rover to stop moving
void stopMoving() {
  //digitalWrite(5, LOW);
  //digitalWrite(9, LOW);
  vref = 0.0;
}
