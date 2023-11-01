#include "Display.h"
#include <math.h>
#include <Arduino.h>

void Display::init() {
  lcd = new LiquidCrystal_I2C(LCD_ADDRESS, 20, 4);
  lcd->init();

  lineNumber[grid_coordinates] = 0;
  startingPosition[grid_coordinates] = 0;
  length[grid_coordinates] = 15;
  lastChange[grid_coordinates] = 0;
  padLeft[grid_coordinates] = false;
  clearText(grid_coordinates);
  maxAge[grid_coordinates] = -1;

  lineNumber[grid_heading] = 0;
  startingPosition[grid_heading] = 15;
  length[grid_heading] = 5;
  lastChange[grid_heading] = 0;
  padLeft[grid_heading] = true;
  clearText(grid_heading);
  maxAge[grid_heading] = -1;

  lineNumber[grid_status] = 1;
  startingPosition[grid_status] = 0;
  length[grid_status] = 10;
  lastChange[grid_status] = 0;
  padLeft[grid_status] = false;
  clearText(grid_status);
  maxAge[grid_status] = -1;

  lineNumber[grid_command] = 2;
  startingPosition[grid_command] = 0;
  length[grid_command] = 10;
  lastChange[grid_command] = 0;
  padLeft[grid_command] = false;
  clearText(grid_command);
  maxAge[grid_command] = -1;

  lineNumber[grid_mode] = 3;
  startingPosition[grid_mode] = 0;
  length[grid_mode] = 10;
  lastChange[grid_mode] = 0;
  padLeft[grid_mode] = false;
  clearText(grid_mode);
  maxAge[grid_command] = -1;

  lineNumber[grid_obstruction_1] = 1;
  startingPosition[grid_obstruction_1] = 10;
  length[grid_obstruction_1] = 10;
  lastChange[grid_obstruction_1] = 0;
  padLeft[grid_obstruction_1] = true;
  clearText(grid_obstruction_1);
  maxAge[grid_obstruction_1] = 5000;

  lineNumber[grid_obstruction_2] = 2;
  startingPosition[grid_obstruction_2] = 10;
  length[grid_obstruction_2] = 10;
  lastChange[grid_obstruction_2] = 0;
  padLeft[grid_obstruction_2] = true;
  clearText(grid_obstruction_2);
  maxAge[grid_obstruction_2] = 5000;

  lineNumber[grid_obstruction_3] = 3;
  startingPosition[grid_obstruction_3] = 10;
  length[grid_obstruction_3] = 10;
  lastChange[grid_obstruction_3] = 0;
  padLeft[grid_obstruction_3] = true;
  clearText(grid_obstruction_3);
  maxAge[grid_obstruction_3] = 5000;
}

void Display::fitText(String message, short gridId) {
  if (message.length() >= length[gridId]) {
    for (int i = 0; i < length[gridId]; i++) {
      text[gridId][i] = message.charAt(i);
    }
  }
  else {
    int currPosition = 0;
    if (padLeft[gridId]) {
      // add spaces until the characters are all filled
      for (currPosition = 0; currPosition < length[gridId] - message.length(); currPosition++) {
        text[gridId][currPosition] = ' ';
      }
    }

    for (int m = 0; m < message.length(); m++) {
      text[gridId][currPosition++] = message[m];
    }

    // pad any characters left on the right
    while (currPosition < length[gridId]) {
      text[gridId][currPosition++] = ' ';
    }
  }
}

void Display::refreshDisplay() {
  lcd->backlight();

  // purge any really old things?
  unsigned long now = millis();
  for (int i = grid_coordinates; i <= grid_heading; i++) {
    if (maxAge[i] > 0 && now - lastChange[i] > maxAge[i]) {
      clearText(i);
    }
    lcd->setCursor(startingPosition[i], lineNumber[i]);
    lcd->print(text[i]);
  }
}

void Display::showObstruction(String location, int distance) {
  // bump any older obstructions lower

  if (lastChange[grid_obstruction_2] > 0) {
    lastChange[grid_obstruction_3] = lastChange[grid_obstruction_2];
    copyText(grid_obstruction_2, grid_obstruction_3);
  }
  if (lastChange[grid_obstruction_1] > 0) {
    lastChange[grid_obstruction_2] = lastChange[grid_obstruction_1];
    copyText(grid_obstruction_1, grid_obstruction_2);
  }

  String sDist = String(distance);

  // Figure out if we need to trim down the location text
  int trimChars = 0;
  while ((location.length() + sDist.length() + 1 - trimChars) > length[grid_obstruction_1]) {
    trimChars++;
  }

  // fit the new text
  if (trimChars == 0) {
    fitText(location + ":" + sDist, grid_obstruction_1);
  }
  else {
    fitText(location.substring(0, location.length() - trimChars) + ":" + sDist, grid_obstruction_1);
  }
  lastChange[grid_obstruction_1] = millis();

  refreshDisplay();
}

void Display::clearText (short gridField) {
  lastChange[gridField] = 0;
  for (int i = 0; i < length[gridField]; i++) {
    text[gridField][i] = ' ';
  }
}

void Display::copyText (short sourceField, short targetField) {
  for (int i = 0; i < length[targetField]; i++) {
    text[targetField][i] = text[sourceField][i];
  }
}

void Display::showStatus(String status) {
  fitText(status, grid_status);
  refreshDisplay();
}

void Display::showPosition(double x, double y, double heading) {
  fitText("(" + String(int(round(x))) + ", " + String(int(round(y))) + ")", grid_coordinates);
  fitText(String(int(round(heading))) + (char)223, grid_heading);
  refreshDisplay();
}

void Display::showCommand(String command) {
  fitText(command, grid_command);
  refreshDisplay();
}

void Display::showMode(String mode) {
  fitText(mode, grid_mode);
  refreshDisplay();
}

void Display::clearDisplay() {
  lcd->clear();
}