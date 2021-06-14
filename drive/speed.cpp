/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#include "speed.h"

// Default constructor
SpeedController::SpeedController(){
  counter = 0;
  referenceVoltage = 0.0;
}

// Calculates the voltage from the desired speed, which is calculated from the error - fed into the SMPS controller
float SpeedController::calculateVoltageReference(float e){
  Serial.println("Count " + String(counter));
  if (counter == 9) {
    // Proportional controller
    float referenceSpeed = distanceToSpeedCoefficient * abs(e);
    referenceVoltage = min(speedToVoltageCoefficient * referenceSpeed, maximumVoltage);
    
    // Reset counter
    counter = 0;
  } else {
    // During these states the controller is idle
    counter++;
  }
  return referenceVoltage;
}
