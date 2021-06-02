#include "move_hl.h"

// Sets up the drive functions
void driveInit(){
  motorSetup();
}

// Causes the rover to move forward for a time t [ms] and returns true after t [ms] have elapsed
bool moveForwardForTime(unsigned long t, unsigned long currentInstructionTime) {
  if (millis() < (currentInstructionTime + t)) {
    forwards();
    return false;
  }
  return true;
}

// Causes the rover to move backward for a time t [ms] and returns true after t [ms] have elapsed
bool moveBackwardForTime(unsigned long t, unsigned long currentInstructionTime) {
  if (millis() < (currentInstructionTime + t)) {
    backwards();
    return false;
  }
  return true;
}

// Causes the rover to move forward for a distance d [cm] and returns true after moving the given distance
bool moveForwardForDistance(float d, float currentInstructionY, float totalY) {
  if (totalY < (currentInstructionY + d)) {
    forwards();
    return false;
  }
  return true;
}

// Causes the rover to move backward for a distance d [cm] and returns true after moving the given distance
bool moveBackwardForDistance(float d, float currentInstructionY, float totalY) {
  if (totalY > (currentInstructionY - d)) {
    backwards();
    return false;
  }
  return true;
}

// Rotates clockwise 90 degrees
bool turnRight(float totalX, float totalY, float currentInstructionX, float currentInstructionY) {
  const float distanceFromRoverCentreToSensor = 11.5;   // distnce from the center of the rover to the optical flow sensor is 11.5 [cm]
  float currentHypotenuse = (totalX - currentInstructionX) * (totalX - currentInstructionX) + (totalY - currentInstructionY) * (totalY - currentInstructionY);
  float desiredHypotenuse = 2 * distanceFromRoverCentreToSensor * distanceFromRoverCentreToSensor;
  if (currentHypotenuse < desiredHypotenuse) {
    anticlockwise();
    return false;
  }
  return true;
}

// Rotates anticlockwise 90 degrees
bool turnLeft(float totalX, float totalY, float currentInstructionX, float currentInstructionY) {
  const float distanceFromRoverCentreToSensor = 11.5;   // distnce from the center of the rover to the optical flow sensor is 11.5 [cm]
  float currentHypotenuse = (totalX - currentInstructionX) * (totalX - currentInstructionX) + (totalY - currentInstructionY) * (totalY - currentInstructionY);
  float desiredHypotenuse = 2 * distanceFromRoverCentreToSensor * distanceFromRoverCentreToSensor;
  if (currentHypotenuse < desiredHypotenuse) {
    clockwise();
    return false;
  }
  return true;
}
