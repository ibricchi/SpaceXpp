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
