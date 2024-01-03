#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "Globals.h"
#include "Camera.h"

#define OLED_ADDR   0x3C

// 128 x 32 pixel display
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#ifndef DISPLAY_H
#define DISPLAY_H
class Display {
  public:
    Display ();
    void showThermal (Camera* camera);
    void clear ();

  private:
    void logConsole (String msg);
    Adafruit_SSD1306* display = new  Adafruit_SSD1306(-1);  // -1 = no reset pin, reset pin not used on 4-pin OLED module    
};
#endif