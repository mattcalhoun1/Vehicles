#ifndef MOTOR_INFO_H

#define MOTOR_INFO_H
struct MotorInfo {
  int EncoderPinOut;
  int OutputPin1;
  int OutputPin2;
  int EncoderAIn;
  float GearRatio;
  float TrackLength;
  float DriveWheelDiameter;
  float PulsesPerRevolution;
  int OutputMin;
  int OutputMax;
  int WheelRow; // which row the wheel is in, 0 or 1 in the case of tank
  int WheelSide; // which side (0 = L, 1 = R)
};

#endif