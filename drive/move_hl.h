#ifndef MOVE_HL_H
#define MOVE_HL_H

// Sets up the drive functions
void driveInit();

// Causes the rover to move forward for a time t [ms] and returns true after t [ms] have elapsed
bool moveForwardForTime(unsigned long t, unsigned long currentInstructionTime);

// Causes the rover to move backward for a time t [ms] and returns true after t [ms] have elapsed
bool moveBackwardForTime(unsigned long t, unsigned long currentInstructionTime);

// Causes the rover to move forward for a distance d [cm] and returns true after moving the given distance
bool moveForwardForDistance(float d, float currentInstructionY, float totalY);

// Causes the rover to move backward for a distance d [cm] and returns true after moving the given distance
bool moveBackwardForDistance(float d, float currentInstructionY, float totalY);

// Rotates clockwise 90 degrees
bool turnRight(float totalX, float totalY, float currentInstructionX, float currentInstructionY);

// Rotates anticlockwise 90 degrees
bool turnLeft(float totalX, float totalY, float currentInstructionX, float currentInstructionY);

#endif
