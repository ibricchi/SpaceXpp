/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#include "move_hl.h"

// Sets up the drive functions
void driveInit() {
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
movement moveForwardForDistance(float d, float initialY, float currentY) {
  movement ans;
  if (currentY < (initialY + d)) {
    forwards();
    ans.e = abs(initialY + d - currentY);
    ans.done = false;
  }else{
    ans.e = 0.0;
    ans.done = true;
  }
  return ans;
}

// Causes the rover to move backward for a distance d [cm] and returns true after moving the given distance
movement moveBackwardForDistance(float d, float initialY, float currentY) {
  movement ans;
  if (currentY > (initialY - d)) {
    backwards();
    ans.e = abs(currentY - initialY + d);
    ans.done = false;
  }else{
    ans.e = 0.0;
    ans.done = true;
  }
  return ans;
}

// Rotates clockwise 90 degrees
movement turnRight(float currentX, float currentY, float initialX, float initialY) {
  float currentHypotenuse = (currentX - initialX) * (currentX - initialX) + (currentY - initialY) * (currentY - initialY);
  float desiredHypotenuse = 2 * distanceFromRoverCentreToSensor * distanceFromRoverCentreToSensor;  // distance is 16.2 [cm]
  movement ans;
  if (currentHypotenuse < desiredHypotenuse) {
    anticlockwise();
    ans.e = abs(desiredHypotenuse - currentHypotenuse);
    ans.done = false;
  } else {
    ans.e = 0.0;
    ans.done = true;
  }
  return ans;
}

// Rotates anticlockwise 90 degrees
movement turnLeft(float currentX, float currentY, float initialX, float initialY) {
  float currentHypotenuse = (currentX - initialX) * (currentX - initialX) + (currentY - initialY) * (currentY - initialY);
  float desiredHypotenuse = 2 * distanceFromRoverCentreToSensor * distanceFromRoverCentreToSensor;
  movement ans;
  if (currentHypotenuse < desiredHypotenuse) {
    clockwise();
    ans.e = abs(desiredHypotenuse - currentHypotenuse);
    ans.done = false;
  } else {
    ans.e = 0.0;
    ans.done = true;
  }
  return ans;
}
