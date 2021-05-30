#include "move_ll.h"
#include "Arduino.h"

// Defines the pins for the motors and initially sets the motors to move forward
void motorSetup() {
  // Right (5) and Left (9) motor & setting their speed to 0 (stationary initially)
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(9, LOW);

  // Direction pins for the motors: Right(21) and Left(20)
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(21, HIGH);
  digitalWrite(20, LOW);
}

// Rover moves forwards
void forwards() {
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(21, HIGH);
  digitalWrite(20, LOW);
}

// Rover moves backwards
void backwards() {
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(21, LOW);
  digitalWrite(20, HIGH);
}

// Rover rotates clockwise
void clockwise() {
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(21, HIGH);
  digitalWrite(20, HIGH);
}

// Rover rotates anticlockwise
void anticlockwise() {
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(21, LOW);
  digitalWrite(20, LOW);
}
