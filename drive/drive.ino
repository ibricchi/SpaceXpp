/*
   Program originally written by Yue Zhu (yue.zhu18@imperial.ac.uk) in July 2020.
   Edited code taken from MS Teams for the Summer Electronics Design Project 2 2021
   Final code written by Benjamin Ramhorst
*/

/*
   NOTES:
    Arduino code for controlling the drive module of the Mars Rover 2021

    SMPS:
        1. Always runs in closed-loop Buck, never Boost
        2. PWM for controlling the SMPS has a frequency of 62.5kHz and a duty cycle range of 2%-98%
        3. Control frequency is set to 2.5kHz
        4. Voltage and current are controlled using a PID controller, to achieve the desired reference voltage

   Movement:
        1. The speed is controlled by controlling the reference (and output) voltage of the SMPS
        2. The rover can move forwards and backwards, as well as rotate clockwise and anticlockwise

   Measurements:
        1. TODO - Expand once optical flow code is added
*/

#include <Wire.h>
#include <INA219_WE.h>
#include "SPI.h"

// Pin definitions for the optical flow sensor
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


// Current sensing chip
INA219_WE ina219;

// Control frequency (integer multiple of switching frequency)
float Ts = 0.0004;

// Reference voltage - used for controlling the speed of the rover
float vref = 0.0;

// Duty cycle required to achieve the desired voltage
float duty_cycle;

// Measurements from the circuit - output voltage and inductor current
float vout, iL;

// Internal values needed for the PID controller
float u0v, u1v, delta_uv, e0v, e1v, e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
float u0i, u1i, delta_ui, e0i, e1i, e2i; // Internal values for the current controller

// PID controller constants for voltage and current
float kpv = 0.05024, kiv = 15.78, kdv = 0;
float kpi = 0.02512, kii = 39.4, kdi = 0;

// Limits
float uv_max = 4, uv_min = 0; // Anti-windup limitation
float ui_max = 1, ui_min = 0; // Anti-windup limitation
float current_limit = 3.0;    // Buck current limit

// Used for looping to control the SMPS
unsigned int loopTrigger;

// Signals for controlling the moverment of the rover
int DIRRstate = HIGH;
int DIRLstate = LOW;

// Distance moved in each direction, [mm]
float totalX = 0.0, totalY = 0.0;

// Distance moved in each direction, [counts/inch]
float totalXAlt = 0.0, totalYAlt = 0.0;

// Purely for debugging - will be removed once control is added
const long f_i = 50000;           //time to move in forward direction

void setup() {
  opticalFlowSetup();
  driveSetup();
  SMPSSetup();
}

void loop() {
  //Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the rover
  if (loopTrigger) {
    sampling();                   // Sample all the measurements
    SMPSControl();                // Control function for duty ref of SMPS
    loopTrigger = 0;              // Reset loop trigger
  }

  opticalFlowRead();
  
  testDrive();

  // Output signals for the motors
  digitalWrite(21, DIRRstate);
  digitalWrite(20, DIRLstate);
}

/*
   Low-level rover functions
*/
void driveSetup() {
  // Direction pins for the motors: Right(21) and Left(20)
  pinMode(21, OUTPUT);
  pinMode(20, OUTPUT);
  digitalWrite(21, HIGH);
  digitalWrite(20, LOW);

  // Right (5) and Left (9) motor & setting their speed to maximum (HIGH)
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
}

// Move forward/backwards
void translation(int dir) {
  if (dir == 0) {
    // move forward
    DIRRstate = HIGH;
    DIRLstate = LOW;
  } else {
    // move backwards
    DIRRstate = LOW;
    DIRLstate = HIGH;
  }
}

// Rotate clockwise/anticlockwise
void rotation(int dir) {
  if (dir == 0) {
    // rotate clockwise
    DIRRstate = HIGH;
    DIRLstate = HIGH;
  } else {
    // rotate anticlockwise
    DIRRstate = LOW;
    DIRLstate = LOW;
  }
}

// Sets the speed of the rover
void setVelocity(float v) {
  // TODO - Epxeriments to find relationship between speed [m/s] and reference voltage [V]
  vref = v;
}

// Causes the rover to stop moving
void stopMoving() {
  vref = 0.0;
}

// Used for testing the motor with incremental changes, will be removed later
void testDrive() {
  unsigned long currentMillis = millis();
  //moving forwards
  if (currentMillis < f_i) {
    setVelocity(3.0);
    translation(0);
  }
  //stop moving
  if (currentMillis >= f_i) {
    stopMoving();
  }
}

/*
  Optical flow setup - NO NEED TO EDIT FURTHER FOR MARS ROVER
*/

// A struct contataining all the necessary attributes of movement
struct MD {
  byte motion;
  char dx, dy;
  byte squal;
  word shutter;
  byte max_pix;
};

// Initialising the optical flow sensor in the setup() function
void opticalFlowSetup() {
  pinMode(PIN_SS, OUTPUT);
  pinMode(PIN_MISO, INPUT);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_SCK, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  Serial.begin(38400);
  if (mousecam_init() == -1) {
    Serial.println("Mouse cam failed to init");
    while (1);
  }
}

// Finding the displacement in the x- and y- direction -> This can be expanded to fit the required needs
void opticalFlowRead() {
  MD md;
  mousecam_read_motion(&md);
  totalXAlt += md.dx;
  totalYAlt += md.dy;
  totalX = 10 * totalXAlt / 157;
  totalY = 10 * totalYAlt / 157;
  Serial.println("Distance_x = " + String(totalX));
  Serial.println("Distance_y = " + String(totalY));
  Serial.print('\n');
}

// Initialising the optical flow sensor
int mousecam_init() {
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
void mousecam_reset() {
  digitalWrite(PIN_MOUSECAM_RESET, HIGH);
  delay(1);
  digitalWrite(PIN_MOUSECAM_RESET, LOW);
  delay(35);
}

// Helper function - for writing to registers of the sensor
void mousecam_write_reg(int reg, int val) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(50);
}

// Helper function - for writing to registers of the sensor
int mousecam_read_reg(int reg) {
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg);
  delayMicroseconds(75);
  int ret = SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS, HIGH);
  delayMicroseconds(1);
  return ret;
}

// For getting movement data , such as displacement
void mousecam_read_motion(struct MD *p) {
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


/*
  SMPS Control - NO NEED TO EDIT FURTHER FOR MARS ROVER
*/

// Basic setup for the SMPS pins and current sensing chip
void SMPSSetup() {
  noInterrupts();             // Disable all interrupts during setup
  analogReference(EXTERNAL);  // Eexternal analogue reference for the ADC

  // TimerA0 initialization for control-loop interrupt.
  TCA0.SINGLE.PER = 999;
  TCA0.SINGLE.CMP1 = 999;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;

  // TimerB0 initialization for PWM output
  pinMode(6, OUTPUT);
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm;
  analogWrite(6, 120);

  // Enable interrupts.
  interrupts();
  // Current sensor
  Wire.begin();
  ina219.init();
  Wire.setClock(700000);
}

// A function for controlling the SMPS duty cycle to achieve the desired reference voltage
void SMPSControl() {
  /*
    The closed loop path has a voltage controller cascaded with a current controller. The voltage controller
    creates a current demand based upon the voltage error. This demand is saturated to give current limiting.
    The current loop then gives a duty cycle demand based upon the error between demanded current and measured
    current.
  */
  float ev = vref - vout;
  float cv = pidv(ev);
  cv = saturation(cv, current_limit, 0);
  float ei = cv - iL;
  duty_cycle = pidi(ei);
  duty_cycle = saturation(duty_cycle, 0.99, 0.01);
  pwm_modulate(duty_cycle);
}

// Timer A CMP1 interrupt. Every 800us the program enters this interrupt. This, clears the incoming interrupt flag and triggers the main loop.
ISR(TCA0_CMP1_vect) {
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm;
  loopTrigger = 1;
}

// This subroutine processes all of the analogue samples, creating the required values for the main loop
void sampling() {
  vout = (float)(analogRead(A0)) * (4.096 / 1023.0);
  iL = (float)(ina219.getCurrent_mA()) / 1000.0;
}

// Saturation function - used for limiting current/voltage to avoid damaging the circuit
float saturation( float sat_input, float uplim, float lowlim) {
  if (sat_input > uplim) sat_input = uplim;
  else if (sat_input < lowlim ) sat_input = lowlim;
  else;
  return sat_input;
}

// PWM waveform for SMPS duty cycle
void pwm_modulate(float pwm_input) {
  analogWrite(6, (int)(255 - pwm_input * 255));
}

// Voltage PID controller
float pidv( float pid_input) {
  float e_integration;
  e0v = pid_input;
  e_integration = e0v;
  if (u1v >= uv_max) {
    e_integration = 0;
  } else if (u1v <= uv_min) {
    e_integration = 0;
  }
  delta_uv = kpv * (e0v - e1v) + kiv * Ts * e_integration + kdv / Ts * (e0v - 2 * e1v + e2v);
  u0v = u1v + delta_uv;  //this time's control output
  saturation(u0v, uv_max, uv_min);
  u1v = u0v;
  e2v = e1v;
  e1v = e0v;
  return u0v;
}

// Current PID Controller
float pidi(float pid_input) {
  float e_integration;
  e0i = pid_input;
  e_integration = e0i;
  if (u1i >= ui_max) {
    e_integration = 0;
  } else if (u1i <= ui_min) {
    e_integration = 0;
  }
  delta_ui = kpi * (e0i - e1i) + kii * Ts * e_integration + kdi / Ts * (e0i - 2 * e1i + e2i);
  u0i = u1i + delta_ui;  //this time's control output
  saturation(u0i, ui_max, ui_min);
  u1i = u0i;
  e2i = e1i;
  e1i = e0i;
  return u0i;
}
