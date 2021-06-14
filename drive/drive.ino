/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#include "smps.h"
#include "move_hl.h"
#include "move_ll.h"
#include "uart.h"
#include "optical_flow.h"
#include "speed.h"

// Control frequency (integer multiple of switching frequency, 62.5kHz)
static const float Tus = 800;  // [us]

// Displacement in each direction, [cm]
OpticalFlow opticalFlow;
float displacementX = 0.0, displacementY = 0.0;

// UART
UART uart;

// Speed and voltage control
SpeedController sc;

// The current instruction to be executed
instructions currentInstruction = doNothing;
float receivedUARTChars = 0.0;
movement data;
bool currentInstructionCompleted = false;
bool currentInstructionStarted = false;
unsigned long currentInstructionTime = 0;
float currentInstructionX = 0.0;
float currentInstructionY = 0.0;

void setup() {
  Serial.begin(38400);
  SMPSSetup();
  driveInit();
  if (!opticalFlow.setup()) {
    Serial.println("Failed to initialise optical flow sensor");
  }
  uart.setup();
  uart.nextInstructionReady();
  data.e = 0.0;
  data.done = true;
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  SMPSControl(sc.calculateVoltageReference(data.e));

  // Received and decode current instruction
  uart.recvUARTWithStartEndMarkers();
  uart.processNewUARTData();
  currentInstruction = uart.getInstruction();
  receivedUARTChars = uart.getReceivedUARTCharts();

  // Read the current position of the rover
  opticalFlow.read();
  displacementX = opticalFlow.getDisplacementX();
  displacementY = opticalFlow.getDisplacementY();

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
    // No error, so no need to control speed
    data.done = true;
    data.e = 0.0;

    // Reset the current instruction
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
    // No error, so no need to control speed
    data.done = true;
    data.e = 0.0;
    uart.setInstruction(doNothing);
    stopMoving();
  }

  // Control circuit frequency (SMPS)
  delayMicroseconds(Tus);
}

// Decides which and calls the given instruction based on the mapping below
boolean callCurrentInstruction() {
  switch (currentInstruction) {
    case forwardForTime:
      return false;
      //return moveForwardForTime(10000, currentInstructionTime);  IMPLEMENTED, NOT USED
    case backwardForTime:
      return false;
    //return moveBackwardForTime(10000, currentInstructionTime); IMPLEMENTED, NOT USED
    case forwardForDistance: 
      data = moveForwardForDistance(receivedUARTChars, currentInstructionY, displacementY);
      return data.done;
    case backwardForDistance:
      data = moveBackwardForDistance(receivedUARTChars, currentInstructionY, displacementY);
      return data.done;
    case turnR:
      data = turnRight(displacementX, displacementY, currentInstructionX, currentInstructionY);
      return data.done;
    case turnL:
      data = turnLeft(displacementX, displacementY, currentInstructionX, currentInstructionY);
      return data.done;
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
