#include "ThermalEncoder.h"

ThermalEncoder::ThermalEncoder(int _resolutionHeight, int _resolutionWidth, bool _compress) {
  resolutionHeight = _resolutionHeight;
  resolutionWidth = _resolutionWidth;
  compress = _compress;
}


bool ThermalEncoder::encode (float* image) {
  // Copy the image into the encoded buffer, as uint8_t rounded, for easy transmission
  for (int row = 0; row < resolutionHeight; row++) {
    for (int col = 0; col < resolutionWidth; col++) {
      int position = (row * resolutionWidth) + col;
      encodeBuffer[position] = round(image[position]);
      Serial.print(encodeBuffer[position]);Serial.print(",");
    }
    Serial.println("");
  }
  encodedBufferLength = resolutionHeight * resolutionWidth;
  Serial.println("Encoded Buffer length " + String(encodedBufferLength));
  return true;
}


bool ThermalEncoder::decode (uint8_t* encodedImage) {
  // just copy straight over since we aren't doing compression yet
  for (int row = 0; row < resolutionHeight; row++) {
    for (int col = 0; col < resolutionWidth; col++) {
      int position = (row * resolutionWidth) + col;
      encodeBuffer[position] = encodedImage[position];
    }
  }
  encodedBufferLength = resolutionHeight * resolutionWidth;
  return true;
}

uint8_t* ThermalEncoder::getEncodeDecodeBuffer () {
  return encodeBuffer;
}

float* ThermalEncoder::getInterpolatedBuffer () {
  return interpolationBuffer;
}

float* ThermalEncoder::interpolate (float* image, int interpolatedHeight, int interpolatedWidth) {
  return image;
}

int ThermalEncoder::getEncodedBufferLength () {
  return encodedBufferLength;
}

int ThermalEncoder::getInterpolatedBufferLength () {
  return interpolatedBufferLength;
}
