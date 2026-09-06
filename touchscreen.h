// Touchscreen handler for CYD 2432S028
// Uses XPT2046 touch controller

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <SPI.h>
#include <XPT2046_Touchscreen.h>

// Touch controller pins for CYD 2432S028
#define TOUCH_CS 33   // GPIO33 - Chip Select
#define TOUCH_IRQ 36  // GPIO36 - Interrupt

// Display calibration values (may need adjustment)
#define TOUCH_MIN_X 150
#define TOUCH_MAX_X 3800
#define TOUCH_MIN_Y 150
#define TOUCH_MAX_Y 3800

// Button areas for menu navigation
struct TouchButton {
  int16_t x, y, w, h;
  const char* label;
  int id;
};

class TouchHandler {
  private:
    XPT2046_Touchscreen ts;  // Changed from pointer to object
    int screenWidth;
    int screenHeight;
    uint32_t lastTouchTime;
    
  public:
    TouchHandler(int width, int height);
    void init();
    bool isTouched();
    void getTouch(int& x, int& y);
    bool isButtonTouched(TouchButton btn);
    void calibrateTouch();
};

#endif // TOUCHSCREEN_H
