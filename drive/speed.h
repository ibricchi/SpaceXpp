/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#ifndef SPEED_H
#define SPEED_H

#include "Arduino.h"

class SpeedController{
  private:
    // Ensures frequency seperation of voltage; speed is only calculated every 10 loops
    int counter;

    // Reference voltage
    float referenceVoltage;

    // Controller constants; determined experimentally
    const float distanceToSpeedCoefficient = 1.5;
    const float speedToVoltageCoefficient = 0.25;
    const float maximumVoltage = 4.0;

  public:
    // Default constructor
    SpeedController();

    // Calculates the voltage from the desired speed - fed into the SMPS controller
    float calculateVoltageReference(float speed);
  };

#endif
