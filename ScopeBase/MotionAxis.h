#ifndef MOTION_AXIS_H
#define MOTION_AXIS_H
/**
 * This is just an interface to some device that can control motion along an axis
 */
enum AxisSpeed {
  AXIS_SPEED_SLOW,
  AXIS_SPEED_MEDIUM,
  AXIS_SPEED_FAST
};


class MotionAxis {
  public:
    virtual void goToPosition (float position, AxisSpeed speed) = 0;
    virtual void move (float amount, AxisSpeed speed) = 0;
    virtual void calibrate () = 0;
    virtual float getPosition () = 0;
    virtual void center() = 0;
};
#endif