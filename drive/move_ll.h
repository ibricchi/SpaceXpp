/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#ifndef MOVE_LL_H
#define MOVE_LL_H

#include "Arduino.h"

#define MR 5
#define ML 9
#define DIRR 21
#define DIRL 20

// Defines the pins for the motors and initially sets the motors to move forward
void motorSetup();

// Rover moves forwards
void forwards();

// Rover moves backwards
void backwards();

// Rover rotates clockwise
void clockwise();

// Rover rotates anticlockwise
void anticlockwise();

// Causes the rover to stop moving
void stopMoving();

#endif
