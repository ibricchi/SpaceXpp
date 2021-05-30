/*
   Program originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020.
   Edited code taken from MS Teams for the Summer Electronics Design Project 2 2021
   Final code written by Benjamin Ramhorst
*/

/*
   NOTES:
    Arduino code for controlling the drive module of the Mars Rover 2021

    SMPS:
        1. Always runs in closed-loop Buck, never Boost
        2. PWM for controlling the SMPS has a frequency of 62.5kHz and a duty cycle range of 2%-98%
        3. Control frequency is set to 2.5kHz
        4. Voltage and current are controlled using a PID controller, to achieve the desired reference voltage

   Movement:
        1. The speed is controlled by controlling the reference (and output) voltage of the SMPS
        2. The rover can move forwards and backwards, as well as rotate clockwise and anticlockwise

   Measurements:
        1. TODO - Expand once optical flow code is added

   UART:
        1. TODO - Expand once code is fully added
*/

#include "smps.h"
#include "optical_flow.h"
#include "move_hl.h"

// Control frequency (integer multiple of switching frequency)
float Ts = 0.0004;

// Reference voltage - used for controlling the speed of the rover
float vref = 3.0;

// Displacement in each direction, [cm]
OpticalFlow opticalFlow;
float displacementX = 0.0, displacementY = 0.0;

// For velocity control, iterative proportional controller
float kvp = 0.1;   // TO BE TUNED
float currentY = 0.0;
float previousY = 0.0;
float currentTime = 0.0;
float previousTime = 0.0;

// The current instruction to be executed
int currentInstruction = 1;
bool currentInstructionCompleted = false;
bool currentInstructionStarted = false;
unsigned long currentInstructionTime = 0;
float currentInstructionX = 0.0;
float currentInstructionY = 0.0;

// UART constants
const byte numUARTChars = 40;
char receivedUARTChars[numUARTChars];
boolean newUARTDataAvailable = false;

void setup() {
  Serial.begin(38400);
  SMPSSetup();
  driveInit();
  if(!opticalFlow.setup()){
    Serial.println("Failed to initialise optical flow sensor");
  }
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  SMPSControl(vref, Ts);

  // Received and decode current instruction
  // recvUARTWithStartEndMarkers();
  // processNewUARTData();

  // Read the current position of the rover
  opticalFlow.read();
  displacementX = opticalFlow.getDisplacementX();
  displacementY = opticalFlow.getDisplacementY();
  Serial.println("Displacement x-direction: " + String(displacementX));
  Serial.println("Displacement y-driection: " + String(displacementY));


  // Set velocity to the desired value - commented out until velocity is accurately calculated using time
  // setVelocity(1.0);


    // Buffer through the instructions in order they arrive
    if (currentInstruction!=-1){
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
          currentInstruction = -1;
          nextInstructionReady();
        }
      }
    } else {
      stopMoving();
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



/*
  UART - Reading  the current instruction
*/

// Decides and call the current instruction based on the mapping below
boolean callCurrentInstruction() {
  switch (currentInstruction) {
    case 1:
    return moveForwardForTime(10000, currentInstructionTime);
    case 2:
    return moveBackwardForTime(10000, currentInstructionTime);
    case 3:
      return moveForwardForDistance(20, currentInstructionY, displacementY);
    case 4:
      return moveBackwardForDistance(20, currentInstructionY, displacementY);
    case 5:
      return turnRight(displacementX, displacementY, currentInstructionX, currentInstructionY);
    case 6:
      return turnLeft(displacementX, displacementY, currentInstructionX, currentInstructionY);
    default:
      return false;
  }
}

// Receving new instruction from UART
void recvUARTWithStartEndMarkers() {
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
void processNewUARTData() {
  if (newUARTDataAvailable) {
    // Read and pop instruction key
    const char instructionKey = receivedUARTChars[strlen(receivedUARTChars) - 1];
    receivedUARTChars[strlen(receivedUARTChars) - 1] = '\0';

    switch (instructionKey) {
      case 'F':
        currentInstruction = 3;
        break;
      case 'R':
        currentInstruction = 5;
        break;
      case 'L':
        currentInstruction = 6;
        break;
      default:
        Serial.print("INVALID INSTRUCTION KEY\n");
        break;
    }

    newUARTDataAvailable = false;
  }
}

// Ready for next instruction signal => should be send after finished with current drive instruction
void nextInstructionReady() {
  Serial.print("R");
}
