#include <Arduino.h>
#include "Globals.h"

#ifndef DISPLAY_H
#define DISPLAY_H
enum TextSize {
  TextSmall = 1,
  TextMedium = 2,
  TextLarge = 3
};

enum DisplayColor {
  Black = 0,
  Blue = 1,
  Red = 2,
  Green = 3,
  Cyan = 4,
  Magenta = 5,
  Yellow = 6,
  White = 7
};

class Display {
  public:
    virtual void showThermal (float* frame, int resHeight, int resWidth, int xOffset, int yOffset) = 0;
    virtual void showThermal (uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset) = 0;
    virtual void showInterpolatedThermalRow (float* interpolatedRow, int xOffset, int yOffset) = 0;
    virtual void showText (String text, int x, int y, TextSize size) = 0;
    virtual void showSymbol (int gfxChar, int x, int y) = 0;
    virtual void showText (String text, int x, int y, TextSize size, DisplayColor color) = 0;
    virtual void showSymbol (int gfxChar, int x, int y, DisplayColor color) = 0;
    virtual void clear () = 0;
    virtual void repaint () = 0;

  protected:
    void logConsole (String msg);
};
#endif