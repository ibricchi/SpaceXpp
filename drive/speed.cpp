#include "speed.h"

// Default constructor
SpeedController::SpeedController(){
  counter = 0;
  referenceVoltage = 0.0;
}

// Calculates the voltage from the desired speed, which is calculated from the error - fed into the SMPS controller
float SpeedController::calculateVoltageReference(float e){
  Serial.println("Count " + String(counter));
  if (counter == 9) {
    // Proportional controller
    float referenceSpeed = distanceToSpeedCoefficient * abs(e);
    referenceVoltage = min(speedToVoltageCoefficient * referenceSpeed, maximumVoltage);
    
    // Reset counter
    counter = 0;
  } else {
    // During these states the controller is idle
    counter++;
  }
  return referenceVoltage;
}
