#include <Adafruit_SSD1331.h>
#include <Adafruit_GFX.h>
#include "Display.h"
#include "Globals.h"

#ifndef DISPLAY_1331_H
#define DISPLAY_1331_H

class Display_1331 : public Display {
  public:
    Display_1331 ();
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
    Adafruit_SSD1331* display = new Adafruit_SSD1331(Display_1331_CS, Display_1331_DC, Display_1331_MOSI, Display_1331_SCLK, Display_1331_RST);
    uint16_t get1331Color(DisplayColor color);
    DisplayColor getTemperatureColor(uint8_t temperature);
    DisplayColor getTemperatureColor(float temperature);
};
#endif