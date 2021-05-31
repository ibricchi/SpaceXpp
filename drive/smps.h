#ifndef SMPS_H
#define SMPS_H

#include <Wire.h>
#include <INA219_WE.h>
#include "Arduino.h"

#define PIN_PWM_OUT 6

// This subroutine processes all of the analogue samples, creating the required values for the main loop
void sampling();

// Saturation function - used for limiting current/voltage to avoid damaging the circuit
float saturation(float sat_input, float uplim, float lowlim);

// PWM waveform for SMPS duty cycle
void pwm_modulate(int pinNo, float pwm_input);

// Voltage PID controller
float pidv(float pid_input, float Ts);

// Current PID Controller
float pidi(float pid_input, float Ts);

// Basic setup for the SMPS pins and current sensing chip
void SMPSSetup();

// A function for controlling the SMPS duty cycle to achieve the desired reference voltage
void SMPSControl(float vref, float Ts);

#endif
