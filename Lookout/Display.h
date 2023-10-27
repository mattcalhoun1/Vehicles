#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27

#ifndef DISPLAY_H
#define DISPLAY_H

enum DisplayGrid {
  grid_coordinates = 0, // gets an entire line
  grid_status = 1, // gets an entire line
  grid_command = 2, // gets first half of line 2
  grid_mode = 3, // gets first half of line 3
  grid_obstruction_1 = 4, //get second half of line 2
  grid_obstruction_2 = 5, // gets second half of line 3
  grid_obstruction_3 = 6, // gets second half of line 3
  grid_heading = 7
};

class Display {
  public:
    void init ();
    void showObstruction (String location, int distance);
    void showStatus (String status);
    void showPosition (double x, double y, double heading);
    void showCommand (String command);
    void showMode (String mode);
    void clearDisplay ();
    void refreshDisplay ();
  private:
    LiquidCrystal_I2C* lcd;
    char text[8][20]; // the texts, up to 20 chars each
    short lineNumber[8];
    short startingPosition[8];
    short length[8];
    unsigned long lastChange[8];
    long maxAge[8];
    bool padLeft[8];
    void fitText(String message, short gridId);
    void clearText (short gridField);
    void copyText (short sourceField, short targetField);
};

#endif DISPLAY_H
