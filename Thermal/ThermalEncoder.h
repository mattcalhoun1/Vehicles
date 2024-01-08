#include "Globals.h"
#include <Arduino.h>

#ifndef THERMALENCODER_H
#define THERMALENCODER_H

class ThermalEncoder {
  public:
    ThermalEncoder(int _resolutionHeigth, int _resolutionWidth, bool _compress);
    bool encode (float* image);
    bool decode (uint8_t* encodedImage);
    uint8_t* getEncodeDecodeBuffer ();
    int getEncodedBufferLength ();
    float* interpolate (float* image, int interpolatedHeight, int interpolatedWidth);
    int getInterpolatedBufferLength ();
    float* getInterpolatedBuffer ();

  private:
    int resolutionHeight;
    int resolutionWidth;
    bool compress;
    float interpolationBuffer[THERMAL_INTERPOLATE_BUFFER_SIZE];
    uint8_t encodeBuffer[THERMAL_ENCODE_BUFFER_SIZE];
    int interpolatedBufferLength = 0;
    int encodedBufferLength = 0;
};

#endif