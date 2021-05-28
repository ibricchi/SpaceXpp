#ifndef OPTICAL_FLOW_H
#define OPTICAL_FLOW_H

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
