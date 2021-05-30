#include "optical_flow.h"

// Pin definitions for the optical flow sensor (Arduino Uno)
#define PIN_SS        10
#define PIN_MISO      12
#define PIN_MOSI      11
#define PIN_SCK       13
#define PIN_MOUSECAM_RESET     8
#define PIN_MOUSECAM_CS        7
#define ADNS3080_PIXELS_X                 30
#define ADNS3080_PIXELS_Y                 30
#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_REVISION_ID           0x01
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_PIXEL_SUM             0x06
#define ADNS3080_MAXIMUM_PIXEL         0x07
#define ADNS3080_CONFIGURATION_BITS    0x0a
#define ADNS3080_EXTENDED_CONFIG       0x0b
#define ADNS3080_DATA_OUT_LOWER        0x0c
#define ADNS3080_DATA_OUT_UPPER        0x0d
#define ADNS3080_SHUTTER_LOWER         0x0e
#define ADNS3080_SHUTTER_UPPER         0x0f
#define ADNS3080_FRAME_PERIOD_LOWER    0x10
#define ADNS3080_FRAME_PERIOD_UPPER    0x11
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_SROM_ENABLE           0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID               0x1f
#define ADNS3080_OBSERVATION           0x3d
#define ADNS3080_INVERSE_PRODUCT_ID    0x3f
#define ADNS3080_PIXEL_BURST           0x40
#define ADNS3080_MOTION_BURST          0x50
#define ADNS3080_SROM_LOAD             0x60
#define ADNS3080_PRODUCT_ID_VAL        0x17

// A struct contataining all the necessary attributes of movement
struct MD {
  byte motion;
  char dx, dy;
  byte squal;
  word shutter;
  byte max_pix;
};

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
  displacementX = (-1) * displacementX;
  displacementY = (-1) * displacementY;
}

// Get x-position
float OpticalFlow::getDisplacementX() {
  return displacementX;
}

// Get y-position
float OpticalFlow::getDisplacementY() {
  return displacementY;
}
