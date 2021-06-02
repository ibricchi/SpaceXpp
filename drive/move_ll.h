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

#endif
