#include "Display_1331.h"

Display_1331::Display_1331() {
  // initialize and clear display
  display->begin();
  clear();
 // display->display();
}

void Display_1331::clear () {
  display->fillScreen(BLACK);
 // display->display();
}

void Display_1331::repaint () {
  //display->display();
}

void Display_1331::showText (String text, int x, int y, TextSize size) {
  showText(text, x, y, size, White);
}
void Display_1331::showSymbol (int gfxChar, int x, int y) {
  showSymbol (gfxChar, x, y, White);
}


void Display_1331::showSymbol (int gfxChar, int x, int y, DisplayColor color) {
  display->setTextColor(get1331Color(color));   // Draw white text
  display->setCursor(x,y);
  display->write(gfxChar);
}


void Display_1331::showText (String text, int x, int y, TextSize size, DisplayColor color) {
  logConsole("Showing text: " + text);
  display->setTextSize(size);                
  display->setTextColor(get1331Color(color));
  display->setCursor(x,y);
  display->println(text);
}

void Display_1331::showInterpolatedThermalRow (float* interpolatedRow, int xOffset, int yOffset) {
  for (int w = 0; w < THERMAL_INTERPOLATED_WIDTH; w++) {
      float t = interpolatedRow[w];
      display->drawPixel(xOffset + w, yOffset, get1331Color(getTemperatureColor(t)));
  }
}

void Display_1331::showThermal (float* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      float t = frame[h*resWidth + w];
      display->drawPixel(xOffset + w, yOffset + h, get1331Color(getTemperatureColor(t)));
    }
  }
}

void Display_1331::showThermal (uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      uint8_t t = frame[h*resWidth + w];
      display->drawPixel(xOffset + w, yOffset + h, get1331Color(getTemperatureColor(t)));
    }
  }
}

DisplayColor Display_1331::getTemperatureColor(float temperature) {
  uint8_t temp = round(temperature);
  return getTemperatureColor(temp);
}

DisplayColor Display_1331::getTemperatureColor(uint8_t temperature) {
  if (temperature < 20) {
    return Black;
  }
  else if (temperature < 23) {
    return Blue;
  }
  else if (temperature < 25) {
    return Green;
  }
  else if (temperature < 27) {
    return Cyan;
  }
  else if (temperature < 29) {
    return Magenta;
  }
  else if (temperature < 31) {
    return Yellow;
  }
  else if (temperature < 37) {
    return Red;
  }
  else {
    return White;
  }
}

uint16_t Display_1331::get1331Color(DisplayColor color) {
  switch (color) {
    case Black:
      return BLACK;
    case Blue:
      return BLUE;
    case Red:
      return RED;
    case Green:
      return GREEN;
    case Cyan:
      return CYAN;
    case Magenta:
      return MAGENTA;
    case Yellow:
      return YELLOW;
    default:
      return WHITE;
  }
}