// Updated menu system with touchscreen support

#ifndef MENU_H
#define MENU_H

#include <TFT_eSPI.h>
#include "touchscreen.h"

// Button pins for CYD 2432S028 (hardware buttons - optional)
#define BUTTON_UP 0      // GPIO0
#define BUTTON_DOWN 35   // GPIO35

// Menu states
enum MenuState {
  MENU_MAIN,
  MENU_WIFI,
  MENU_LOCATION,
  MENU_DISPLAY,
  MENU_RADAR,
  MENU_RUNNING
};

// Configuration structure (persistent settings)
struct Config {
  char ssid[32];
  char password[32];
  float latitude;
  float longitude;
  float searchRadius;
  int updateInterval;
  int radarRadius;
  bool debugMode;
};

class RadarMenu {
  private:
    TFT_eSPI* tft;
    TouchHandler* touch;
    MenuState currentState;
    int selectedOption;
    Config config;
    int buttonUpState, buttonDownState;
    unsigned long lastButtonCheck;
    
    // Touch button definitions
    TouchButton upButton;
    TouchButton downButton;
    TouchButton selectButton;
    TouchButton backButton;
    
  public:
    RadarMenu(TFT_eSPI* displayPtr, TouchHandler* touchPtr);
    void init();
    void update();
    void draw();
    void handleButtonInput();
    void handleTouchInput();
    void displayMainMenu();
    void displayWiFiMenu();
    void displayLocationMenu();
    void displayDisplayMenu();
    void displayRadarMenu();
    void drawMenuHeader(const char* title);
    void drawMenuItem(int y, const char* label, const char* value, bool selected);
    void drawNavigationButtons();
    void saveConfig();
    void loadConfig();
    Config getConfig();
};

#endif // MENU_H
