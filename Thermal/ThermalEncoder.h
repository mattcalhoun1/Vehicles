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
    float* getInterpolatedRow (float* image, int projectedRow);
    float* getInterpolatedRow (uint8_t* image, int projectedRow);

  private:
    int resolutionHeight;
    int resolutionWidth;
    bool compress;
    float interpolationBuffer[THERMAL_INTERPOLATE_BUFFER_SIZE];
    uint8_t encodeBuffer[THERMAL_ENCODE_BUFFER_SIZE];
    int interpolatedBufferLength = 0;
    int encodedBufferLength = 0;

    void primeInterpolationBuffer (float* image, int projectedRow);
    void primeInterpolationBuffer (uint8_t* image, int projectedRow);
    void setInterpolatedBufferValue (int row, int col, float value);
    float getInterpolatedBufferValue (int row, int col);
    float getIntBufferPerimiterAverageSquare (int row, int col);
    float getIntBufferPerimiterAverageDiamond (int row, int col);
    void logInterpolationBuffer ();
    void completePrimedInterpolationBuffer ();
};

#endif