/*
   Program originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020.
   Edited code taken from MS Teams for the Summer Electronics Design Project 2 2021
   Final code written by Benjamin Ramhorst and Nicholas Pfaff (UART code)
*/

#include "smps.h"
#include "optical_flow.h"
#include "move_hl.h"
#include "uart.h"

// Control frequency (integer multiple of switching frequency, 62.5kHz)
float Ts = 0.0004;

// Reference voltage - used for controlling the speed of the rover; likely to be removed once speed controller is fully functional
float vref = 4.0;

// Displacement in each direction, [cm]
OpticalFlow opticalFlow;
float displacementX = 0.0, displacementY = 0.0;

// UART
UART uart;

// The current instruction to be executed
instructions currentInstruction = doNothing;
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
  Serial.begin(9600);
  SMPSSetup();
  driveInit();
  if (!opticalFlow.setup()) {
    Serial.println("Failed to initialise optical flow sensor");
  }
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  SMPSControl(vref, Ts);

  // Received and decode current instruction
  uart.recvUARTWithStartEndMarkers();
  uart.processNewUARTData();
  currentInstruction = uart.getInstruction();

  // Read the current position of the rover
  opticalFlow.read();
  displacementX = opticalFlow.getDisplacementX();
  displacementY = opticalFlow.getDisplacementY();
  Serial.println("Displacement x-direction: " + String(displacementX));
  Serial.println("Displacement y-driection: " + String(displacementY));


  // Set velocity to the desired value - commented out until velocity is accurately calculated using time
  // setVelocity(1.0);

  // Buffer through the instructions in order they arrive
  if (currentInstruction != doNothing) {
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
      }
    }
  } else {
    stopMoving();
  }

  // Control circuit frequency (SMPS)
  delayMicroseconds((unsigned long)Ts * 1000000.0);
}

// Decides and calls the current instruction based on the mapping below - these numbers are purely for testing; will be replaced with UART data
boolean callCurrentInstruction() {
  switch (currentInstruction) {
    case forwardForTime:
      return moveForwardForTime(10000, currentInstructionTime);
    case backwardForTime:
      return moveBackwardForTime(10000, currentInstructionTime);
    case forwardForDistance:
      return moveForwardForDistance(20, currentInstructionY, displacementY);
    case backwardForDistance:
      return moveBackwardForDistance(20, currentInstructionY, displacementY);
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

// Causes the rover to stop moving
void stopMoving() {
  digitalWrite(5, LOW);
  digitalWrite(9, LOW);
  vref = 0.0;
}
