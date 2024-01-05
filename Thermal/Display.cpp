#include "Display.h"

Display::Display() {
  // initialize and clear display
  display->begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display->clearDisplay();
  display->display();
}

void Display::clear () {
  display->clearDisplay();
  display->display();
}

void Display::showThermal (Camera* camera) {
  display->clearDisplay();
  float* frame = camera->getImageData();
  int resHeight = camera->getResolutionHeight();
  int resWidth = camera->getResolutionWidth();

  int warmThreshold = 27;
  int hotThreshold = 29;

  for (uint8_t h=0; h<resHeight; h++) {
    for (uint8_t w=0; w<resWidth; w++) {
      float t = frame[h*resWidth + w];
      if (t >= hotThreshold) {
        display->drawPixel(48 + w, h, WHITE);
      }
      else if (t >= warmThreshold) {
        // 50/50 chance
        if (random(0, 100) % 2 == 0) {
          display->drawPixel(48 + w, h, WHITE);
        }
      }
    }
  }
  display->display();
}


void Display::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}