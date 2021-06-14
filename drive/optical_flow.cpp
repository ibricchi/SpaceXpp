/*
  Program written by Benjamin Ramhorst (br319@ic.ac.uk) during May-June 2021
  as part of SpaceX++ team for the Summer term design project at Imperial College London
  SMPS code originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020 and further
  modfied by Benjamin Ramhorst to fit the requirements of the Mars Rover
  Teamplete code for optical flow and movement functions taken from MS Teams for the Summer Electronics Design Project 2 2021
  and further modified by Benjamin Ramhorst to fit the project requirements
  UART code written by Nicholas Pfaff and modified by Benjamin Ramhorst
*/

#include "optical_flow.h"

// Default constructor
OpticalFlow::OpticalFlow() {
  displacementX = 0.0;
  displacementY = 0.0;
  displacementXAlt = 0.0;
  displacementYAlt = 0.0;
}

// Initialising the optical flow sensor
int OpticalFlow::mousecam_init() {
  pinMode(PIN_MOUSECAM_RESET, OUTPUT);
  pinMode(PIN_MOUSECAM_CS, OUTPUT);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  mousecam_reset();
  int pid = mousecam_read_reg(ADNS3080_PRODUCT_ID);
  if (pid != ADNS3080_PRODUCT_ID_VAL) return -1;
  mousecam_write_reg(ADNS3080_CONFIGURATION_BITS, 0x19);
  return 0;
}

// Reseting the optical flow sensor
void OpticalFlow::mousecam_reset() {
  digitalWrite(PIN_MOUSECAM_RESET, HIGH);
  delay(1);
  digitalWrite(PIN_MOUSECAM_RESET, LOW);
  delay(35);
}

// Helper function - for writing to registers of the sensor
void OpticalFlow::mousecam_write_reg(int reg, int val) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(50);
}

// Helper function - for writing to registers of the sensor
int OpticalFlow::mousecam_read_reg(int reg) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg);
  delayMicroseconds(75);
  int ret = SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(1);
  return ret;
}

// For getting movement data, such as displacement
void OpticalFlow::mousecam_read_motion(struct MD *p) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(ADNS3080_MOTION_BURST);
  delayMicroseconds(75);
  p->motion =  SPI.transfer(0xff);
  p->dx =  SPI.transfer(0xff);
  p->dy =  SPI.transfer(0xff);
  p->squal =  SPI.transfer(0xff);
  p->shutter =  SPI.transfer(0xff) << 8;
  p->shutter |=  SPI.transfer(0xff);
  p->max_pix =  SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(5);
}

// Initialising the optical flow sensor in the setup() function
bool OpticalFlow::setup() {
  pinMode(PIN_SS, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_SCK, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  if (mousecam_init() == -1) {
    return false;
  }
  return true;
}

// Finding the displacement in the x- and y- direction
void OpticalFlow::read() {
  MD md;
  mousecam_read_motion(&md);
  // Add incremental changes
  displacementXAlt += md.dx;
  displacementYAlt += md.dy;
  // Convert from counts/inch to cm
  displacementX = displacementXAlt / 157.48;
  displacementY = displacementYAlt / 157.48;
  // Invert to match x-y plane
  // displacementX = (-1) * displacementX;
  // displacementY = (-1) * displacementY;
}

// Get x-position
float OpticalFlow::getDisplacementX() {
  return displacementX;
}

// Get y-position
float OpticalFlow::getDisplacementY() {
  return displacementY;
}
