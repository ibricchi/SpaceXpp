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
        3. Control frequency is set to 1.25kHz
        4. Voltage and current are controlled using a PID controller, to achieve the desired reference voltage

   Movement:
        1. The speed is controlled by controlling the reference (and output) voltage of the SMPS
        2. The rover can move forwards and backwards, as well as rotate clockwise and anticlockwise

   Measurements:
        1. TODO - Expand once optical flow code is added
*/

#include <Wire.h>
#include <INA219_WE.h>

INA219_WE ina219;

float duty_mes; // Duty Cycles
float vb, iL, dutyref, current_mA; // Measurement Variables
unsigned int sensorValue0, sensorValue1, sensorValue2, sensorValue3; // ADC sample values declaration
float ev = 0, cv = 0, ei = 0, oc = 0; //internal signals
float Ts = 0.0008; //1.25 kHz control frequency. It's better to design the control period as integral multiple of switching period.
float kpv = 0.05024, kiv = 15.78, kdv = 0; // voltage pid.
float u0v, u1v, delta_uv, e0v, e1v, e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
float kpi = 0.02512, kii = 39.4, kdi = 0; // current pid.
float u0i, u1i, delta_ui, e0i, e1i, e2i; // Internal values for the current controller
float uv_max = 4, uv_min = 0; //anti-windup limitation
float ui_max = 1, ui_min = 0; //anti-windup limitation
float current_limit = 3.0;

// Used for setting the speed of the rover
float vref = 1.5;

unsigned int loopTrigger;
unsigned int com_count = 0; // a variables to count the interrupts. Used for program debugging.


//************************** Motor Constants **************************//
unsigned long previousMillis = 0; //initializing time counter
const long f_i = 10000;           //time to move in forward direction, please calculate the precision and conversion factor
const long r_i = 20000;           //time to rotate clockwise
const long b_i = 30000;           //time to move backwards
const long l_i = 40000;           //time to move anticlockwise
const long s_i = 50000;
int DIRRstate = LOW;              //initializing direction states
int DIRLstate = HIGH;

int DIRL = 20;                    //defining left direction pin
int DIRR = 21;                    //defining right direction pin

int pwmr = 5;                     //pin to control right wheel speed using pwm
int pwml = 9;                     //pin to control left wheel speed using pwm
//*******************************************************************//


void setup() {

  //************************** Motor Pins Defining **************************//
  pinMode(DIRR, OUTPUT);
  pinMode(DIRL, OUTPUT);
  pinMode(pwmr, OUTPUT);
  pinMode(pwml, OUTPUT);
  digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
  digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
  //*******************************************************************//

  SMPSSetup();
}

void loop() {
  /*
     Used for controlling the duty cycle of the SMPS to achieve the required reference voltage, which, in turn, controls the speed of the motors
  */
  if (loopTrigger) {
    sampling();                   // Sample all the measurements
    SMPSControl();                // Control function for duty ref of SMPS
    loopTrigger = 0;              // Reset loop trigger
  }

  /*
     Used for testing the motor with incremental changes, will be removed later
  */
  unsigned long currentMillis = millis();
  //moving forwards
  if (currentMillis < f_i) {
    setVelocity(4.0);
    translation(0);
  }
  //rotating clockwise
  if (currentMillis > f_i && currentMillis < r_i) {
    rotation(0);
  }
  //moving backwards
  if (currentMillis > r_i && currentMillis < b_i) {
    setVelocity(1.5);
    translation(1);
  }
  //rotating anticlockwise
  if (currentMillis > b_i && currentMillis < l_i) {
    rotation(1);
  }
  //stop moving
  if (currentMillis > l_i) {
    stopMoving();
  }

  // Output signals for the motors
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate);
}

/*
   Rover movement functions
*/

// A function that causes the rover to move forward/backwards
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

// A function that causes the rover to rotate clockwise/anticlockwise
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
  vref = 0;
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
  ev = vref - vb;
  cv = pidv(ev);
  cv = saturation(cv, current_limit, 0);
  ei = cv - iL;
  duty_mes = pidi(ei);
  duty_mes = saturation(duty_mes, 0.99, 0.01);
  pwm_modulate(duty_mes);

}

// Timer A CMP1 interrupt. Every 800us the program enters this interrupt. This, clears the incoming interrupt flag and triggers the main loop.
ISR(TCA0_CMP1_vect) {
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
  loopTrigger = 1;
}

// This subroutine processes all of the analogue samples, creating the required values for the main loop
void sampling() {
  sensorValue0 = analogRead(A0); // output voltage at termin 
  sensorValue2 = vref * (1023.0 / 4.096); //analogRead(A2); // Vref
  current_mA = ina219.getCurrent_mA(); // IL from current snesing chip

  /*
    Process the values so they are a bit more usable/readable
    The analogRead process gives a value between 0 and 1023
    representing a voltage between 0 and the analogue reference which is 4.096V
  */
  vb = sensorValue0 * (4.096 / 1023.0);
  //vref = sensorValue2 * (4.096 / 1023.0);


  
    // The inductor current is in mA from the sensor so we need to convert to amps.
    iL = current_mA / 1000.0;
    dutyref = sensorValue2 * (1.0 / 1023.0);
 
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
