#ifndef OPTICAL_FLOW_H
#define OPTICAL_FLOW_H

#include "SPI.h"
#include "Arduino.h"

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

class OpticalFlow{
  private:
    // Distance moved in each direction, [cm]
    float displacementX;
    float displacementY;

    // Distance moved in each direction, [counts/inch]
    float displacementXAlt;
    float displacementYAlt;

    // Initialising the optical flow sensor
    int mousecam_init();

    // Reseting the optical flow sensor
    void mousecam_reset();

    // Helper function - for writing to registers of the sensor
    void mousecam_write_reg(int reg, int val);

    // Helper function - for writing to registers of the sensor
    int mousecam_read_reg(int reg);

    // For getting movement data , such as displacement
    void mousecam_read_motion(struct MD *p);

  public:
    // Default constructor, sets everything to 0
    OpticalFlow();

     // Initialising the optical flow sensor in the constructor
    bool setup();

    // Finding the displacement in the x- and y- direction
    void read();

    // Getters
    float getDisplacementX();
    float getDisplacementY();

};

#endif
