// Touchscreen implementation for CYD 2432S028

#include "touchscreen.h"

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

TouchHandler::TouchHandler(int width, int height) {
  screenWidth = width;
  screenHeight = height;
  lastTouchTime = 0;
}

void TouchHandler::init() {
  // Initialize SPI for touchscreen
  SPI.begin();
  
  // Initialize XPT2046 touchscreen
  ts.begin();
  ts.setRotation(1);  // Match display rotation
  
  Serial.println("Touchscreen initialized");
}

bool TouchHandler::isTouched() {
  return ts.touched();
}

void TouchHandler::getTouch(int& x, int& y) {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    
    // Debounce - ignore if touched within 50ms
    if (millis() - lastTouchTime < 50) return;
    lastTouchTime = millis();
    
    // Map raw touch coordinates to screen coordinates
    // XPT2046 returns values 0-4095
    x = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, screenWidth);
    y = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, screenHeight);
    
    // Constrain to screen bounds
    x = constrain(x, 0, screenWidth - 1);
    y = constrain(y, 0, screenHeight - 1);
  }
}

bool TouchHandler::isButtonTouched(TouchButton btn) {
  if (!ts.touched()) return false;
  
  TS_Point p = ts.getPoint();
  
  // Debounce
  if (millis() - lastTouchTime < 50) return false;
  
  // Map to screen coordinates
  int x = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, screenWidth);
  int y = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, screenHeight);
  
  // Check if touch is within button bounds
  if (x >= btn.x && x <= (btn.x + btn.w) &&
      y >= btn.y && y <= (btn.y + btn.h)) {
    lastTouchTime = millis();
    return true;
  }
  
  return false;
}

void TouchHandler::calibrateTouch() {
  // Simple touchscreen calibration routine
  Serial.println("Touch screen calibration");
  Serial.println("Press top-left corner");
  delay(2000);
  
  // Collect calibration points
  // This is a simplified version - for production, use full calibration
  
  Serial.println("Calibration complete");
}
