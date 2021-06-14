/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

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
void pwm_modulate(float pwm_input);

// Voltage PID controller
float pidv(float pid_input, float Ts);

// Current PID Controller
float pidi(float pid_input, float Ts);

// Basic setup for the SMPS pins and current sensing chip
void SMPSSetup();

// A function for controlling the SMPS duty cycle to achieve the desired reference voltage
void SMPSControl(float vref);

#endif
