#ifndef MOVE_LL_H
#define MOVE_LL_H

// Defines the pins for the motors and initially sets the motors to move forward
void motorSetup();

// These functions return two values, one for output signal for right motor, other for the left motor

// Rover moves forwards
void forwards();

// Rover moves backwards
void backwards();

// Rover rotates clockwise
void clockwise();

// Rover rotates anticlockwise
void anticlockwise();

#endif
