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
