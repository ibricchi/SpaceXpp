/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

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
