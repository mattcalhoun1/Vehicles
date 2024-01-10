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

int ThermalEncoder::getEncodedBufferLength () {
  return encodedBufferLength;
}

// identical to uint8 version, just needed different pointer type
// maybe can refactor these down somehow
void ThermalEncoder::primeInterpolationBuffer (float* image, int projectedRow) {
  // if the projected row is an even row, we already have half the pixels
  // if it's odd, we have no rows of the pixel

  // -1 means unknown or not yet calculated
  //  first copy in all the data we know
  int intBufferRow = 0;
  int intBufferCol = 0;
  for (int currProjRow = projectedRow - 1; currProjRow <= projectedRow + 1; currProjRow++) {
    for (int currProjCol = 0; currProjCol < THERMAL_INTERPOLATED_WIDTH; currProjCol++) {
      // if this is an even (projected) row, copy the data straight over
      if (currProjRow % 2 == 0 && currProjRow < THERMAL_INTERPOLATED_HEIGHT) {
        if (currProjCol % 2 == 0) {
          setInterpolatedBufferValue(intBufferRow, intBufferCol, image[(resolutionWidth * (currProjRow / 2)) + (currProjCol / 2)]);
        }
        else {
          setInterpolatedBufferValue(intBufferRow, intBufferCol, -1);
        }
      }
      else {
          setInterpolatedBufferValue(intBufferRow, intBufferCol, -1);
      }
      intBufferCol++;
    }
    //Serial.println("");
    intBufferCol = 0;
    intBufferRow++;
  }
}

// identical to float version, just needed different pointer type
// maybe can refactor these down somehow
void ThermalEncoder::primeInterpolationBuffer (uint8_t* image, int projectedRow) {
  // if the projected row is an even row, we already have half the pixels
  // if it's odd, we have no rows of the pixel

  // -1 means unknown or not yet calculated
  //  first copy in all the data we know
  int intBufferRow = 0;
  int intBufferCol = 0;
  for (int currProjRow = projectedRow - 1; currProjRow <= projectedRow + 1; currProjRow++) {
    for (int currProjCol = 0; currProjCol < THERMAL_INTERPOLATED_WIDTH; currProjCol++) {
      // if this is an even (projected) row, copy the data straight over
      if (currProjRow % 2 == 0 && currProjRow < THERMAL_INTERPOLATED_HEIGHT) {
        if (currProjCol % 2 == 0) {
          setInterpolatedBufferValue(intBufferRow, intBufferCol, image[(resolutionWidth * (currProjRow / 2)) + (currProjCol / 2)]);
        }
        else {
          setInterpolatedBufferValue(intBufferRow, intBufferCol, -1);
        }
      }
      else {
          setInterpolatedBufferValue(intBufferRow, intBufferCol, -1);
      }
      intBufferCol++;
    }
    //Serial.println("");
    intBufferCol = 0;
    intBufferRow++;
  }
}

float* ThermalEncoder::getInterpolatedRow (uint8_t* image, int projectedRow) {
  primeInterpolationBuffer(image, projectedRow);

  completePrimedInterpolationBuffer();


  // the middle row is the one requested
  return interpolationBuffer + THERMAL_INTERPOLATED_WIDTH;
}

float* ThermalEncoder::getInterpolatedRow (float* image, int projectedRow) {
  // Copy the known values into the interpolation projection
  // -1 means unknown or not yet calculated
  primeInterpolationBuffer(image, projectedRow);

  completePrimedInterpolationBuffer();

  // the middle row is the one requested
  return interpolationBuffer + THERMAL_INTERPOLATED_WIDTH;
}

void ThermalEncoder::completePrimedInterpolationBuffer () {

  // first pass on primed buffer,
  // for each row that is fully empty (indicated by -1 in col zero, based on how priming was implemented)
  int intBufferRow = 0;
  int intBufferCol = 1;
  for (int currProjRow = 0; currProjRow < 3; currProjRow++) {
    if (getInterpolatedBufferValue(intBufferRow, 0) == -1) {
      // start with second pixel, where we have the most surrounding information
      for (int currProjCol = 1; currProjCol < THERMAL_INTERPOLATED_WIDTH; currProjCol+=2) {
        // get the avrage of surrounding pixels (excluding -1s) at the 4 corners (like a square)
        setInterpolatedBufferValue(intBufferRow, intBufferCol, getIntBufferPerimiterAverageSquare(intBufferRow, intBufferCol));
        intBufferCol+=2;
      }
    }
    intBufferCol = 1;
    intBufferRow++;
  }

  // 3rd pass, fill in all missing values in middle row, which is the only one we return
  intBufferRow = 1;
  //intBufferCol = 0;

  for (int currProjCol = 0; currProjCol < THERMAL_INTERPOLATED_WIDTH; currProjCol++) {
    if (getInterpolatedBufferValue(intBufferRow, currProjCol) == -1) {
      setInterpolatedBufferValue(intBufferRow, currProjCol, getIntBufferPerimiterAverageDiamond(intBufferRow, currProjCol));
    }
  }
}

void ThermalEncoder::logInterpolationBuffer () {
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < THERMAL_INTERPOLATED_WIDTH; c++) {
      Serial.print(String(getInterpolatedBufferValue(r, c)));Serial.print(",");
    }
    Serial.println("");
  }
}

float ThermalEncoder::getIntBufferPerimiterAverageSquare (int row, int col) {
  int numVals = 0;
  float total = 0;
  for (int r = row-1; r <= row+1; r++) {
    for (int c = col-1; c <= col+1; c++) {
      if (r != row && c != col) {
        float val = getInterpolatedBufferValue(r, c);
        if (val != -1) {
          total += val;
          numVals ++;
        }
      }
    }
  }

  if (numVals > 0) {
    return total / numVals;
  }
  return -1;
}

float ThermalEncoder::getIntBufferPerimiterAverageDiamond (int row, int col) {
  int numVals = 0;
  float total = 0;
  for (int r = row-1; r <= row+1; r++) {
    for (int c = col-1; c <= col+1; c++) {
      if (r == row || c == col) {
        float val = getInterpolatedBufferValue(r, c);
        if (val != -1) {
          total += val;
          numVals ++;
        }
      }
    }
  }



  if (numVals > 0) {
    return total / numVals;
  }
  return -1;
}

void ThermalEncoder::setInterpolatedBufferValue (int row, int col, float value) {
  interpolationBuffer[(row * THERMAL_INTERPOLATED_WIDTH) + col] = value;
}

float ThermalEncoder::getInterpolatedBufferValue (int row, int col) {
  // if the requested value is out of bounds, return -1 so it doesn't get counted
  if(row < 0 || col < 0 || row >= 3 || col >= THERMAL_INTERPOLATED_WIDTH) {
    return -1;
  }
  return interpolationBuffer[(row * THERMAL_INTERPOLATED_WIDTH) + col];
}