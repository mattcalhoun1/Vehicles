#include "Display_1306.h"

Display_1306::Display_1306() {
  // initialize and clear display
  display->begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display->clearDisplay();
  display->display();
}

void Display_1306::clear () {
  display->clearDisplay();
  display->display();
}

void Display_1306::repaint () {
  display->display();
}

void Display_1306::showSymbol (int gfxChar, int x, int y) {
  display->setTextColor(SSD1306_WHITE);   // Draw white text
  display->setCursor(x,y);
  display->write(gfxChar);
}

void Display_1306::showText (String text, int x, int y, TextSize size, DisplayColor color) {
  showText(text, x, y, size);
}
void Display_1306::showSymbol (int gfxChar, int x, int y, DisplayColor color) {
  showSymbol (gfxChar, x, y);
}


void Display_1306::showText (String text, int x, int y, TextSize size) {
  logConsole("Showing text: " + text);
  display->setTextSize(size);                
  display->setTextColor(WHITE);   // Draw white text
  display->setCursor(x,y);
  display->println(text);
}

void Display_1306::showThermal (float* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  int hotThreshold = 27;

  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      float t = frame[h*resWidth + w];
      if (t >= hotThreshold) {
        display->drawPixel(xOffset + w, yOffset + h, WHITE);
      }
    }
  }
}

void Display_1306::showInterpolatedThermalRow (float* interpolatedRow, int xOffset, int yOffset) {
  logConsole("interpolation display not implemented in 1306");
}

void Display_1306::showThermal (uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) {
  int hotThreshold = 27;

  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      uint8_t t = frame[h*resWidth + w];
      if (t >= hotThreshold) {
        display->drawPixel(xOffset + w, yOffset + h, WHITE);
      }
    }
  }
}


