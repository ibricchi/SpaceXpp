#ifndef MOVE_HL_H
#define MOVE_HL_H

#include "move_ll.h"
#include "Arduino.h"

struct movement{
  float e;
  bool done;  
};

// Distance from the center of motion (wheels) to the optical flow sensor is 16.2 [cm]
const float distanceFromRoverCentreToSensor = 16.2;

// Sets up the drive functions
void driveInit();

// Causes the rover to move forward for a time t [ms] and returns true after t [ms] have elapsed
bool moveForwardForTime(unsigned long t, unsigned long currentInstructionTime);

// Causes the rover to move backward for a time t [ms] and returns true after t [ms] have elapsed
bool moveBackwardForTime(unsigned long t, unsigned long currentInstructionTime);

// Causes the rover to move forward for a distance d [cm] and returns true after moving the given distance
movement moveForwardForDistance(float d, float initialY, float currentY);

// Causes the rover to move backward for a distance d [cm] and returns true after moving the given distance
movement moveBackwardForDistance(float d, float initialY, float currentY);

// Rotates clockwise 90 degrees
movement turnRight(float currentX, float currentY, float initialX, float initialY);

// Rotates anticlockwise 90 degrees
movement turnLeft(float currentX, float currentY, float initialX, float initialY);

#endif
