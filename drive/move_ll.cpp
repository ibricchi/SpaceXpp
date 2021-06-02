#include "move_ll.h"

// Defines the pins for the motors and initially sets the motors to move forward
void motorSetup() {
  // Right (5) and Left (9) motor & setting their speed to maximum
  pinMode(MR, OUTPUT);
  pinMode(ML, OUTPUT);
  digitalWrite(MR, HIGH);
  digitalWrite(ML, HIGH);

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
