#include <Arduino.h>
#include "Display.h"

void Display::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}