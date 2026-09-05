// Menu system for CYD Radar - allows configuration via display buttons
// Supports navigation and adjustment of key settings

#ifndef MENU_H
#define MENU_H

#include <TFT_eSPI.h>

// Button pins for CYD 2432S028
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
    MenuState currentState;
    int selectedOption;
    Config config;
    int buttonUpState, buttonDownState;
    unsigned long lastButtonCheck;
    
  public:
    RadarMenu(TFT_eSPI* displayPtr);
    void init();
    void update();
    void draw();
    void handleButtonInput();
    void displayMainMenu();
    void displayWiFiMenu();
    void displayLocationMenu();
    void displayDisplayMenu();
    void displayRadarMenu();
    void drawMenuHeader(const char* title);
    void drawMenuItem(int y, const char* label, const char* value, bool selected);
    void saveConfig();
    void loadConfig();
    Config getConfig();
};

#endif // MENU_H
