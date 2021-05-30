#ifndef MOVE_LL_H
#define MOVE_LL_H

#include "Arduino.h"

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
