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
#include "SPI.h"

// Pin definitions for the optical flow sensor
#define PIN_SS        10
#define PIN_MISO      12
#define PIN_MOSI      11
#define PIN_SCK       13
#define PIN_MOUSECAM_RESET     8
#define PIN_MOUSECAM_CS        7
#define ADNS3080_PIXELS_X                 30
#define ADNS3080_PIXELS_Y                 30
#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_REVISION_ID           0x01
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_PIXEL_SUM             0x06
#define ADNS3080_MAXIMUM_PIXEL         0x07
#define ADNS3080_CONFIGURATION_BITS    0x0a
#define ADNS3080_EXTENDED_CONFIG       0x0b
#define ADNS3080_DATA_OUT_LOWER        0x0c
#define ADNS3080_DATA_OUT_UPPER        0x0d
#define ADNS3080_SHUTTER_LOWER         0x0e
#define ADNS3080_SHUTTER_UPPER         0x0f
#define ADNS3080_FRAME_PERIOD_LOWER    0x10
#define ADNS3080_FRAME_PERIOD_UPPER    0x11
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_SROM_ENABLE           0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID               0x1f
#define ADNS3080_OBSERVATION           0x3d
#define ADNS3080_INVERSE_PRODUCT_ID    0x3f
#define ADNS3080_PIXEL_BURST           0x40
#define ADNS3080_MOTION_BURST          0x50
#define ADNS3080_SROM_LOAD             0x60
#define ADNS3080_PRODUCT_ID_VAL        0x17

// Control frequency (integer multiple of switching frequency)
float Ts = 0.0004;

// Reference voltage - used for controlling the speed of the rover
float vref = 3.0;

// Signals for controlling the moverment of the rover
int DIRRstate = HIGH;
int DIRLstate = LOW;

// Distance moved in each direction, [cm]
float totalX = 0.0, totalY = 0.0;

// Distance moved in each direction, [counts/inch]
float totalXAlt = 0.0, totalYAlt = 0.0;

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
  SMPSSetup();
  opticalFlowSetup();
  driveSetup();
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  SMPSControl(vref, Ts);

  // Received and decode current instruction
  recvUARTWithStartEndMarkers();
  processNewUARTData();

  // Read the current position of the rover
  opticalFlowRead();

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



/*
  Optical flow setup - NO NEED TO EDIT FURTHER FOR MARS ROVER
*/

// A struct contataining all the necessary attributes of movement
struct MD {
  byte motion;
  char dx, dy;
  byte squal;
  word shutter;
  byte max_pix;
};

// Initialising the optical flow sensor in the setup() function
void opticalFlowSetup() {
  pinMode(PIN_SS, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_SCK, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  Serial.begin(38400);
  if (mousecam_init() == -1) {
    Serial.println("Mouse cam failed to init");
    while (1);
  }
}

// Finding the displacement in the x- and y- direction -> This can be expanded to fit the required needs
void opticalFlowRead() {
  MD md;
  mousecam_read_motion(&md);
  // Add incremental changes
  totalXAlt += md.dx;
  totalYAlt += md.dy;
  // Convert from counts/inch to cm
  totalX = totalXAlt / 157.48;
  totalY = totalYAlt / 157.48;
  // Inver to match x-y plane
  totalX = (-1) * totalX;
  totalY = (-1) * totalY;
  Serial.println("Distance_x = " + String(totalX));
  Serial.println("Distance_y = " + String(totalY));
  Serial.print('\n');
}

// Initialising the optical flow sensor
int mousecam_init() {
  pinMode(PIN_MOUSECAM_RESET, OUTPUT);
  pinMode(PIN_MOUSECAM_CS, OUTPUT);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  mousecam_reset();
  int pid = mousecam_read_reg(ADNS3080_PRODUCT_ID);
  if (pid != ADNS3080_PRODUCT_ID_VAL) return -1;
  mousecam_write_reg(ADNS3080_CONFIGURATION_BITS, 0x19);
  return 0;
}

// Reseting the optical flow sensor
void mousecam_reset() {
  digitalWrite(PIN_MOUSECAM_RESET, HIGH);
  delay(1);
  digitalWrite(PIN_MOUSECAM_RESET, LOW);
  delay(35);
}

// Helper function - for writing to registers of the sensor
void mousecam_write_reg(int reg, int val) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(50);
}

// Helper function - for writing to registers of the sensor
int mousecam_read_reg(int reg) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg);
  delayMicroseconds(75);
  int ret = SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(1);
  return ret;
}

// For getting movement data , such as displacement
void mousecam_read_motion(struct MD *p) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(ADNS3080_MOTION_BURST);
  delayMicroseconds(75);
  p->motion =  SPI.transfer(0xff);
  p->dx =  SPI.transfer(0xff);
  p->dy =  SPI.transfer(0xff);
  p->squal =  SPI.transfer(0xff);
  p->shutter =  SPI.transfer(0xff) << 8;
  p->shutter |=  SPI.transfer(0xff);
  p->max_pix =  SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(5);
}
