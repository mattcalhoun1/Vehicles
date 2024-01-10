#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "Display.h"
#include "Globals.h"

// 128 x 32 pixel display
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define OLED_ADDR   0x3C

#ifndef DISPLAY_1306_H
#define DISPLAY_1306_H

class Display_1306 : public Display {
  public:
    Display_1306 ();
    virtual void showThermal (float* frame, int resHeight, int resWidth, int xOffset, int yOffset);
    virtual void showThermal (uint8_t* frame, int resHeight, int resWidth, int xOffset, int yOffset);
    virtual void showInterpolatedThermalRow (float* interpolatedRow, int xOffset, int yOffset);
    virtual void showText (String text, int x, int y, TextSize size);
    virtual void showText (String text, int x, int y, TextSize size, DisplayColor color);
    virtual void showSymbol (int gfxChar, int x, int y);
    virtual void showSymbol (int gfxChar, int x, int y, DisplayColor color);
    virtual void clear ();
    virtual void repaint ();

  private:
    Adafruit_SSD1306* display = new  Adafruit_SSD1306(-1);  // -1 = no reset pin, reset pin not used on 4-pin OLED module
};
#endif