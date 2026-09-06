// Menu system implementation with touchscreen support
#include "menu.h"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define CONFIG_START_ADDR 0

RadarMenu::RadarMenu(TFT_eSPI* displayPtr, TouchHandler* touchPtr) {
  tft = displayPtr;
  touch = touchPtr;
  currentState = MENU_MAIN;
  selectedOption = 0;
  lastButtonCheck = 0;
  buttonUpState = HIGH;
  buttonDownState = HIGH;
  
  // Define touch button areas
  upButton = {10, 10, 50, 40, "UP", 1};
  downButton = {10, 190, 50, 40, "DOWN", 2};
  selectButton = {260, 190, 50, 40, "SEL", 3};
  backButton = {260, 10, 50, 40, "BACK", 4};
}

void RadarMenu::init() {
  // Initialize hardware buttons
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  
  // Initialize touchscreen
  touch->init();
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  loadConfig();
  
  // Display splash screen
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_CYAN, TFT_BLACK);
  tft->setTextDatum(MC_DATUM);
  tft->setTextSize(3);
  tft->drawString("CYD RADAR", tft->width()/2, tft->height()/2 - 30);
  tft->setTextSize(1);
  tft->setTextColor(TFT_YELLOW);
  tft->drawString("Press UP or TAP UP button to configure", tft->width()/2, tft->height()/2 + 20);
  tft->drawString("Touch and button control enabled", tft->width()/2, tft->height()/2 + 40);
  delay(3000);
}

void RadarMenu::update() {
  if (millis() - lastButtonCheck < 50) return;
  lastButtonCheck = millis();
  
  handleButtonInput();
  handleTouchInput();
  draw();
}

void RadarMenu::handleButtonInput() {
  // Read hardware buttons (GPIO0 and GPIO35)
  int upPressed = !digitalRead(BUTTON_UP);
  int downPressed = !digitalRead(BUTTON_DOWN);
  
  // UP button: move selection up
  if (upPressed && buttonUpState == HIGH) {
    buttonUpState = LOW;
    selectedOption = max(0, selectedOption - 1);
  }
  if (!upPressed) buttonUpState = HIGH;
  
  // DOWN button: select/move down
  if (downPressed && buttonDownState == HIGH) {
    buttonDownState = LOW;
    selectedOption = min(4, selectedOption + 1);
  }
  if (!downPressed) buttonDownState = HIGH;
}

void RadarMenu::handleTouchInput() {
  if (!touch->isTouched()) return;
  
  int touchX, touchY;
  touch->getTouch(touchX, touchY);
  
  // UP navigation
  if (touchX >= upButton.x && touchX <= upButton.x + upButton.w &&
      touchY >= upButton.y && touchY <= upButton.y + upButton.h) {
    selectedOption = max(0, selectedOption - 1);
    delay(200);
    return;
  }
  
  // DOWN navigation
  if (touchX >= downButton.x && touchX <= downButton.x + downButton.w &&
      touchY >= downButton.y && touchY <= downButton.y + downButton.h) {
    selectedOption = min(4, selectedOption + 1);
    delay(200);
    return;
  }
  
  // SELECT button
  if (touchX >= selectButton.x && touchX <= selectButton.x + selectButton.w &&
      touchY >= selectButton.y && touchY <= selectButton.y + selectButton.h) {
    if (currentState == MENU_MAIN) {
      currentState = (MenuState)(MENU_WIFI + selectedOption);
      selectedOption = 0;
    }
    delay(200);
    return;
  }
  
  // BACK button
  if (touchX >= backButton.x && touchX <= backButton.x + backButton.w &&
      touchY >= backButton.y && touchY <= backButton.y + backButton.h) {
    currentState = MENU_MAIN;
    selectedOption = 0;
    delay(200);
    return;
  }
  
  // Touch menu items directly
  if (currentState == MENU_MAIN) {
    for (int i = 0; i < 5; i++) {
      int itemY = 60 + i * 30;
      if (touchY >= itemY && touchY <= itemY + 25) {
        selectedOption = i;
        delay(100);
        currentState = (MenuState)(MENU_WIFI + i);
        selectedOption = 0;
        delay(200);
        return;
      }
    }
  }
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
    default:
      break;
  }
  
  drawNavigationButtons();
}

void RadarMenu::drawMenuHeader(const char* title) {
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_CYAN, TFT_BLACK);
  tft->setTextDatum(TL_DATUM);
  tft->setTextSize(2);
  tft->drawString(title, 70, 10);
  tft->drawFastHLine(0, 40, 320, TFT_CYAN);
}

void RadarMenu::drawMenuItem(int y, const char* label, const char* value, bool selected) {
  uint16_t bgColor = selected ? TFT_BLUE : TFT_BLACK;
  uint16_t textColor = selected ? TFT_WHITE : TFT_CYAN;
  
  tft->fillRect(60, y, 260, 25, bgColor);
  tft->setTextColor(textColor, bgColor);
  tft->setTextDatum(TL_DATUM);
  tft->setTextSize(1);
  tft->drawString(label, 70, y + 5);
  
  tft->setTextDatum(TR_DATUM);
  tft->drawString(value, 310, y + 5);
}

void RadarMenu::drawNavigationButtons() {
  tft->setTextSize(1);
  tft->setTextColor(TFT_WHITE, TFT_DARKGREY);
  
  // UP button
  tft->fillRect(upButton.x, upButton.y, upButton.w, upButton.h, TFT_DARKGREY);
  tft->setTextDatum(MC_DATUM);
  tft->setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft->drawString("UP", upButton.x + upButton.w/2, upButton.y + upButton.h/2);
  
  // DOWN button
  tft->fillRect(downButton.x, downButton.y, downButton.w, downButton.h, TFT_DARKGREY);
  tft->drawString("DN", downButton.x + downButton.w/2, downButton.y + downButton.h/2);
  
  // SELECT button
  tft->fillRect(selectButton.x, selectButton.y, selectButton.w, selectButton.h, TFT_GREEN);
  tft->setTextColor(TFT_BLACK, TFT_GREEN);
  tft->drawString("SEL", selectButton.x + selectButton.w/2, selectButton.y + selectButton.h/2);
  
  // BACK button
  tft->fillRect(backButton.x, backButton.y, backButton.w, backButton.h, TFT_RED);
  tft->setTextColor(TFT_WHITE, TFT_RED);
  tft->drawString("BCK", backButton.x + backButton.w/2, backButton.y + backButton.h/2);
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
  
  for (int i = 0; i < 5; i++) {
    drawMenuItem(60 + i * 30, menuItems[i], "", i == selectedOption);
  }
}

void RadarMenu::displayWiFiMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("WiFi Settings");
  
  char ssidDisplay[20];
  snprintf(ssidDisplay, sizeof(ssidDisplay), "%.15s", config.ssid);
  
  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  tft->setTextDatum(TL_DATUM);
  tft->setTextSize(1);
  tft->drawString("SSID:", 70, 60);
  tft->drawString(ssidDisplay, 70, 80);
  tft->drawString("Password:", 70, 105);
  tft->drawString("***", 70, 125);
  tft->drawString("(Edit WiFi in code)", 70, 150);
}

void RadarMenu::displayLocationMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("Location Setup");
  
  char latStr[20], lonStr[20], radiusStr[20];
  dtostrf(config.latitude, 8, 4, latStr);
  dtostrf(config.longitude, 8, 4, lonStr);
  dtostrf(config.searchRadius, 4, 2, radiusStr);
  
  drawMenuItem(60, "Latitude", latStr, selectedOption == 0);
  drawMenuItem(90, "Longitude", lonStr, selectedOption == 1);
  drawMenuItem(120, "Search Radius", radiusStr, selectedOption == 2);
}

void RadarMenu::displayDisplayMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("Display Config");
  
  char radiusStr[10], intervalStr[10];
  itoa(config.radarRadius, radiusStr, 10);
  itoa(config.updateInterval, intervalStr, 10);
  
  drawMenuItem(60, "Radar Radius", radiusStr, selectedOption == 0);
  drawMenuItem(90, "Update Rate (ms)", intervalStr, selectedOption == 1);
}

void RadarMenu::displayRadarMenu() {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();
  
  drawMenuHeader("Radar Settings");
  
  const char* debugStr = config.debugMode ? "ON" : "OFF";
  drawMenuItem(60, "Debug Mode", debugStr, selectedOption == 0);
  
  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  tft->setTextDatum(TL_DATUM);
  tft->setTextSize(1);
  tft->drawString("Tap BACK to return", 70, 140);
}

void RadarMenu::saveConfig() {
  EEPROM.writeString(CONFIG_START_ADDR, config.ssid);
  EEPROM.writeString(CONFIG_START_ADDR + 32, config.password);
  EEPROM.writeFloat(CONFIG_START_ADDR + 64, config.latitude);
  EEPROM.writeFloat(CONFIG_START_ADDR + 68, config.longitude);
  EEPROM.writeFloat(CONFIG_START_ADDR + 72, config.searchRadius);
  EEPROM.writeInt(CONFIG_START_ADDR + 76, config.updateInterval);
  EEPROM.writeInt(CONFIG_START_ADDR + 80, config.radarRadius);
  EEPROM.writeBool(CONFIG_START_ADDR + 84, config.debugMode);
  EEPROM.commit();
  Serial.println("Configuration saved to EEPROM");
}

void RadarMenu::loadConfig() {
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
