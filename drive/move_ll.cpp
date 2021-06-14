/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#include "move_ll.h"

// Defines the pins for the motors and initially sets the motors to move forward
void motorSetup() {
  // Right (5) and Left (9) motor & setting their speed to minimum
  pinMode(MR, OUTPUT);
  pinMode(ML, OUTPUT);
  digitalWrite(MR, LOW);
  digitalWrite(ML, LOW);

  // Direction pins for the motors: Right(21) and Left(20)
  pinMode(DIRR, OUTPUT);
  pinMode(DIRL, OUTPUT);
  digitalWrite(DIRR, HIGH);
  digitalWrite(DIRL, LOW);
}

// Rover moves forwards
void forwards() {
  digitalWrite(MR, HIGH);
  digitalWrite(ML, HIGH);
  digitalWrite(DIRR, HIGH);
  digitalWrite(DIRL, LOW);
}

// Rover moves backwards
void backwards() {
  digitalWrite(MR, HIGH);
  digitalWrite(ML, HIGH);
  digitalWrite(DIRR, LOW);
  digitalWrite(DIRL, HIGH);
}

// Rover rotates clockwise
void clockwise() {
  digitalWrite(MR, HIGH);
  digitalWrite(ML, HIGH);
  digitalWrite(DIRR, HIGH);
  digitalWrite(DIRL, HIGH);
}

// Rover rotates anticlockwise
void anticlockwise() {
  digitalWrite(MR, HIGH);
  digitalWrite(ML, HIGH);
  digitalWrite(DIRR, LOW);
  digitalWrite(DIRL, LOW);
}

// Causes the rover to stop moving
void stopMoving() {
  digitalWrite(MR, LOW);
  digitalWrite(ML, LOW);
}
