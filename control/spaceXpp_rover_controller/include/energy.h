/*
    Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

#ifndef ENERGY_H
#define ENERGY_H

#define ENERGY_INSTRUCTION_DELIMITER ":"

// UART data encoding
typedef struct EnergyEncoding{
    const char* stateOfCharge;
    const char* stateOfHealth;
    const char* cellError;
} EnergyEncoding;

#endif
