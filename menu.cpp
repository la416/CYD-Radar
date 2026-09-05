// Menu system implementation for CYD Radar

#include "menu.h"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define CONFIG_START_ADDR 0

RadarMenu::RadarMenu(TFT_eSPI* displayPtr) {
  tft = displayPtr;
  currentState = MENU_MAIN;
  selectedOption = 0;
  lastButtonCheck = 0;
  buttonUpState = HIGH;
  buttonDownState = HIGH;
}

void RadarMenu::init() {
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  
  EEPROM.begin(EEPROM_SIZE);
  loadConfig();
  
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE);
  tft->setTextDatum(MC_DATUM);
  tft->setTextSize(2);
  tft->drawString("CYD RADAR", tft->width()/2, tft->height()/2 - 20);
  tft->setTextSize(1);
  tft->drawString("Press UP to configure", tft->width()/2, tft->height()/2 + 20);
  delay(2000);
}

void RadarMenu::update() {
  if (millis() - lastButtonCheck < 50) return;  // Debounce
  lastButtonCheck = millis();
  
  handleButtonInput();
  draw();
}

void RadarMenu::handleButtonInput() {
  int upPressed = !digitalRead(BUTTON_UP);    // LOW when pressed
  int downPressed = !digitalRead(BUTTON_DOWN); // LOW when pressed
  
  // UP button: move selection up
  if (upPressed && buttonUpState == LOW) {
    selectedOption--;
    if (selectedOption < 0) selectedOption = 0;
  }
  buttonUpState = upPressed ? LOW : HIGH;
  
  // DOWN button: move selection down or enter menu
  if (downPressed && buttonDownState == LOW) {
    selectedOption++;
  }
  buttonDownState = downPressed ? LOW : HIGH;
}

void RadarMenu::draw() {
  switch (currentState) {
    case MENU_MAIN:
      displayMainMenu();
      break;
    case MENU_WIFI:
      displayWiFiMenu();
      break;
    case MENU_LOCATION:
      displayLocationMenu();
      break;
    case MENU_DISPLAY:
      displayDisplayMenu();
      break;
    case MENU_RADAR:
      displayRadarMenu();
      break;
    case MENU_RUNNING:
      // Running radar - minimal UI
      break;
  }
}

void RadarMenu::drawMenuHeader(const char* title) {
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_YELLOW);
  tft->setTextDatum(TL_DATUM);
  tft->setTextSize(2);
  tft->drawString(title, 5, 5);
  tft->setTextSize(1);
  tft->setTextColor(TFT_WHITE);
}

void RadarMenu::drawMenuItem(int y, const char* label, const char* value, bool selected) {
  uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
  uint16_t textColor = selected ? TFT_WHITE : TFT_CYAN;
  
  tft->fillRect(0, y, tft->width(), 25, bgColor);
  tft->setTextColor(textColor);
  tft->setTextDatum(TL_DATUM);
  tft->drawString(label, 10, y + 5);
  
  tft->setTextDatum(TR_DATUM);
  tft->drawString(value, tft->width() - 10, y + 5);
}

void RadarMenu::displayMainMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("MAIN MENU");
  
  const char* menuItems[] = {
    "WiFi Settings",
    "Location Setup",
    "Display Config",
    "Radar Settings",
    "Start Radar"
  };
  
  const int maxItems = 5;
  if (selectedOption >= maxItems) selectedOption = maxItems - 1;
  
  for (int i = 0; i < maxItems; i++) {
    int y = 40 + (i * 35);
    drawMenuItem(y, menuItems[i], "", i == selectedOption);
    
    // Handle selection
    if (i == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
      delay(200);
      switch (i) {
        case 0: currentState = MENU_WIFI; selectedOption = 0; break;
        case 1: currentState = MENU_LOCATION; selectedOption = 0; break;
        case 2: currentState = MENU_DISPLAY; selectedOption = 0; break;
        case 3: currentState = MENU_RADAR; selectedOption = 0; break;
        case 4: currentState = MENU_RUNNING; saveConfig(); break;
      }
    }
  }
  
  tft->setTextColor(TFT_GREEN);
  tft->setTextDatum(BL_DATUM);
  tft->setTextSize(1);
  tft->drawString("UP/DOWN to select", 5, tft->height() - 5);
}

void RadarMenu::displayWiFiMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("WiFi Settings");
  
  const char* menuItems[] = { "SSID", "Password", "Back" };
  const int maxItems = 3;
  if (selectedOption >= maxItems) selectedOption = maxItems - 1;
  
  char ssidDisplay[20];
  snprintf(ssidDisplay, sizeof(ssidDisplay), "%.15s", config.ssid);
  
  char passDisplay[20];
  snprintf(passDisplay, sizeof(passDisplay), "%s", strlen(config.password) > 0 ? "***" : "Not set");
  
  drawMenuItem(40, menuItems[0], ssidDisplay, 0 == selectedOption);
  drawMenuItem(75, menuItems[1], passDisplay, 1 == selectedOption);
  drawMenuItem(110, menuItems[2], "", 2 == selectedOption);
  
  if (2 == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
    delay(200);
    currentState = MENU_MAIN;
    selectedOption = 0;
  }
}

void RadarMenu::displayLocationMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("Location Setup");
  
  char latStr[20], lonStr[20], radiusStr[20];
  snprintf(latStr, sizeof(latStr), "%.4f", config.latitude);
  snprintf(lonStr, sizeof(lonStr), "%.4f", config.longitude);
  snprintf(radiusStr, sizeof(radiusStr), "%.1f deg", config.searchRadius);
  
  drawMenuItem(40, "Latitude", latStr, 0 == selectedOption);
  drawMenuItem(75, "Longitude", lonStr, 1 == selectedOption);
  drawMenuItem(110, "Search Radius", radiusStr, 2 == selectedOption);
  
  // Simple increment/decrement with buttons
  if (0 == selectedOption && digitalRead(BUTTON_UP) == LOW) {
    config.latitude += 0.01;
    delay(100);
  }
  if (0 == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
    config.latitude -= 0.01;
    delay(100);
  }
  
  if (2 == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
    delay(200);
    currentState = MENU_MAIN;
    selectedOption = 1;
  }
}

void RadarMenu::displayDisplayMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("Display Config");
  
  char radarStr[20], updateStr[20];
  snprintf(radarStr, sizeof(radarStr), "%d px", config.radarRadius);
  snprintf(updateStr, sizeof(updateStr), "%d ms", config.updateInterval);
  
  drawMenuItem(40, "Radar Radius", radarStr, 0 == selectedOption);
  drawMenuItem(75, "Update Rate", updateStr, 1 == selectedOption);
  drawMenuItem(110, "Back", "", 2 == selectedOption);
  
  if (2 == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
    delay(200);
    currentState = MENU_MAIN;
    selectedOption = 2;
  }
}

void RadarMenu::displayRadarMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("Radar Settings");
  
  const char* debugStr = config.debugMode ? "ON" : "OFF";
  
  drawMenuItem(40, "Debug Mode", debugStr, 0 == selectedOption);
  drawMenuItem(75, "Info:", "Press DOWN to see", 1 == selectedOption);
  drawMenuItem(110, "Back", "", 2 == selectedOption);
  
  if (0 == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
    config.debugMode = !config.debugMode;
    delay(200);
  }
  
  if (2 == selectedOption && digitalRead(BUTTON_DOWN) == LOW) {
    delay(200);
    currentState = MENU_MAIN;
    selectedOption = 3;
  }
}

void RadarMenu::saveConfig() {
  // Write to EEPROM
  EEPROM.writeString(CONFIG_START_ADDR, config.ssid);
  EEPROM.writeString(CONFIG_START_ADDR + 32, config.password);
  EEPROM.writeFloat(CONFIG_START_ADDR + 64, config.latitude);
  EEPROM.writeFloat(CONFIG_START_ADDR + 68, config.longitude);
  EEPROM.writeFloat(CONFIG_START_ADDR + 72, config.searchRadius);
  EEPROM.writeInt(CONFIG_START_ADDR + 76, config.updateInterval);
  EEPROM.writeInt(CONFIG_START_ADDR + 80, config.radarRadius);
  EEPROM.writeBool(CONFIG_START_ADDR + 84, config.debugMode);
  EEPROM.commit();
}

void RadarMenu::loadConfig() {
  // Load from EEPROM with defaults
  String ssidStr = EEPROM.readString(CONFIG_START_ADDR);
  String passStr = EEPROM.readString(CONFIG_START_ADDR + 32);
  
  strcpy(config.ssid, ssidStr.c_str());
  strcpy(config.password, passStr.c_str());
  
  config.latitude = EEPROM.readFloat(CONFIG_START_ADDR + 64);
  config.longitude = EEPROM.readFloat(CONFIG_START_ADDR + 68);
  config.searchRadius = EEPROM.readFloat(CONFIG_START_ADDR + 72);
  config.updateInterval = EEPROM.readInt(CONFIG_START_ADDR + 76);
  config.radarRadius = EEPROM.readInt(CONFIG_START_ADDR + 80);
  config.debugMode = EEPROM.readBool(CONFIG_START_ADDR + 84);
  
  // Apply defaults if not set
  if (strlen(config.ssid) == 0) strcpy(config.ssid, "YourSSID");
  if (config.latitude == 0) config.latitude = 51.5074;
  if (config.longitude == 0) config.longitude = -0.1278;
  if (config.searchRadius == 0) config.searchRadius = 1.5;
  if (config.updateInterval == 0) config.updateInterval = 100;
  if (config.radarRadius == 0) config.radarRadius = 80;
}

Config RadarMenu::getConfig() {
  return config;
}
