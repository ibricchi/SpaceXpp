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

// Control frequency (integer multiple of switching frequency)
float Ts = 0.0004;

// Reference voltage - used for controlling the speed of the rover
float vref = 3.0;

// Signals for controlling the moverment of the rover
int DIRRstate = LOW;
int DIRLstate = LOW;

// Displacement in each direction, [cm]
OpticalFlow opticalFlow;
float totalX = 0.0, totalY = 0.0;

// For velocity control, iterative proportional controller
float kvp = 0.1;   // TO BE TUNED
float currentY = 0.0;
float previousY = 0.0;
float currentTime = 0.0;
float previousTime = 0.0;

// The current instruction to be executed
int currentInstruction = -1;
bool currentInstructionCompleted = false;
bool currentInstructionStarted = false;
long long currentInstructionTime = 0;
float currentInstructionX = 0.0;
float currentInstructionY = 0.0;

// UART constants
const byte numUARTChars = 40;
char receivedUARTChars[numUARTChars];
boolean newUARTDataAvailable = false;

void setup() {
  // Device setup
  Serial.begin(38400);
  SMPSSetup();
  if(!opticalFlow.setup()){
    Serial.println("Failed to initialise optical flow sensor");
  }
  driveSetup();
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  SMPSControl(vref, Ts);

  // Received and decode current instruction
  recvUARTWithStartEndMarkers();
  processNewUARTData();

  // Read the current position of the rover
  opticalFlow.read();
  totalX = opticalFlow.getDisplacementX();
  totalY = opticalFlow.getDisplacementY();
  Serial.println(String(totalX));
  Serial.println(String(totalY));


  // Set velocity to the desired value - commented out until velocity is accurately calculated using time
  // setVelocity(1.0);

  /*
    // Buffer through the instructions in order they arrive
    if (currentInstruction!=-1){
      if (!currentInstructionStarted) {
        currentInstructionStarted = true;
        currentInstructionCompleted = false;
        currentInstructionTime = millis();
        currentInstructionX = totalX;
        currentInstructionY = totalY;
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

    // Output signals for the motors
    digitalWrite(21, DIRRstate);
    digitalWrite(20, DIRLstate);
  */
}



/*
  High-level movement control functions
*/

// Causes the rover to move forward for a time t [ms] and returns true after t [ms] have elapsed - SLIGHTLY BUGGY
boolean moveForwardForTime(float t) {
  if (millis() < (currentInstructionTime + t)) {
    translation(0);
    return false;
  }
  return true;
}

// Causes the rover to move backward for a time t [ms] and returns true after t [ms] have elapsed - SLIGHTLY BUGGY
boolean moveBackwardForTime(float t) {
  if (millis() < (currentInstructionTime + t)) {
    translation(1);
    return false;
  }
  return true;
}

// Causes the rover to move forward for a distance d [cm] and returns true after moving the given distance
boolean moveForwardForDistance(float d) {
  if (totalY < (currentInstructionY + d)) {
    translation(0);
    return false;
  }
  return true;
}

// Causes the rover to move backward for a distance d [cm] and returns true after moving the given distance
boolean moveBackwardForDistance(float d) {
  if (totalY > (currentInstructionY - d)) {
    translation(1);
    return false;
  }
  return true;
}

// Rotates clockwise 90 degrees
boolean clockwise90() {
  float currentHypotenuse = (totalX - currentInstructionX) * (totalX - currentInstructionX) + (totalY - currentInstructionY) * (totalY - currentInstructionY);
  float desiredHypotenuse = 2 * 11.5 * 11.5; // distna from the center of the rover to the optical flow sensor is 11.5 [cm]
  if (currentHypotenuse < desiredHypotenuse) {
    rotation(0);
    return false;
  }
  return true;
}

// Rotates anticlockwise 90 degrees
boolean anticlockwise90() {
  float currentHypotenuse = (totalX - currentInstructionX) * (totalX - currentInstructionX) + (totalY - currentInstructionY) * (totalY - currentInstructionY);
  float desiredHypotenuse = 2 * 11.5 * 11.5; // distna from the center of the rover to the optical flow sensor is 11.5 [cm]
  if (currentHypotenuse < desiredHypotenuse) {
    rotation(1);
    return false;
  }
  return true;
}

// Decides and call the current instruction based on the mapping below
boolean callCurrentInstruction() {
  switch (currentInstruction) {
    case 1:
    //return moveForwardForTime(data[currentInstruction]);
    case 2:
    // return moveBackwardForTime(data[currentInstruction]);
    case 3:
      return moveForwardForDistance(atof(receivedUARTChars));
    case 4:
    // return moveBackwardForDistance(data[currentInstruction]);
    case 5:
      return clockwise90();
    case 6:
      return anticlockwise90();
    default:
      return false;
  }
}



/*
   Low-level rover functions
*/
void driveSetup() {
  // Direction pins for the motors: Right(21) and Left(20)
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(21, HIGH);
  digitalWrite(20, LOW);

  // Right (5) and Left (9) motor & setting their speed to maximum (HIGH)
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
}

// Move forward/backwards
void translation(int dir) {
  if (dir == 0) {
    // move forward
    DIRRstate = HIGH;
    DIRLstate = LOW;
  } else {
    // move backwards
    DIRRstate = LOW;
    DIRLstate = HIGH;
  }
}

// Rotate clockwise/anticlockwise
void rotation(int dir) {
  if (dir == 0) {
    // rotate clockwise
    DIRRstate = HIGH;
    DIRLstate = HIGH;
  } else {
    // rotate anticlockwise
    DIRRstate = LOW;
    DIRLstate = LOW;
  }
}

// Sets the speed of the rover
void setVelocity(float velocityReference) {
  previousY = currentY;
  currentY = totalY;
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
