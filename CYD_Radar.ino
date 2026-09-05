#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "menu.h"

// Display setup
TFT_eSPI tft = TFT_eSPI();
RadarMenu* radarMenu;

// Display parameters
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int CENTER_X = SCREEN_WIDTH / 2;
const int CENTER_Y = SCREEN_HEIGHT / 2;

// Heartbeat animation
unsigned long lastHeartbeat = 0;
int heartbeatPhase = 0;
const int HEARTBEAT_INTERVAL = 600; // ms between heartbeats

// Aircraft data structure
struct Aircraft {
  float lat;
  float lon;
  float altitude;
  float speed;
  String callsign;
};

Aircraft aircraft[50];
int aircraftCount = 0;

// FlightRadar24 API
const char* flightRadarAPI = "https://api.flightradar24.com/common/v1/aircraft.json";

// Application states
enum AppState {
  STATE_MENU,
  STATE_RUNNING
};

AppState appState = STATE_MENU;
unsigned long lastAPICall = 0;
unsigned long lastDisplayUpdate = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  Serial.println("\n\nCYD Radar starting...");
  
  // Initialize menu system
  radarMenu = new RadarMenu(&tft);
  radarMenu->init();
}

void loop() {
  if (appState == STATE_MENU) {
    radarMenu->update();
    
    // Check if user wants to start radar
    if (radarMenu->getConfig().latitude != 0) {
      // Menu will transition to running state
      // Check button to start
      if (digitalRead(35) == LOW) {  // Down button
        delay(500);
        appState = STATE_RUNNING;
        connectToWiFi();
      }
    }
  } else if (appState == STATE_RUNNING) {
    Config cfg = radarMenu->getConfig();
    
    // Fetch aircraft data periodically
    if (millis() - lastAPICall > cfg.updateInterval) {
      lastAPICall = millis();
      if (WiFi.status() == WL_CONNECTED) {
        fetchFlightData(cfg.latitude, cfg.longitude, cfg.searchRadius);
      }
    }
    
    // Update display
    if (millis() - lastDisplayUpdate > 100) {
      lastDisplayUpdate = millis();
      updateRadarDisplay(cfg);
    }
  }
  
  delay(10);
}

void connectToWiFi() {
  Config cfg = radarMenu->getConfig();
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Connecting WiFi...", CENTER_X, CENTER_Y);
  
  WiFi.begin(cfg.ssid, cfg.password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("WiFi OK", CENTER_X, CENTER_Y);
    delay(1000);
  } else {
    Serial.println("\nWiFi Failed!");
    tft.setTextColor(TFT_RED);
    tft.drawString("WiFi Failed", CENTER_X, CENTER_Y);
    delay(2000);
  }
}

void fetchFlightData(float userLat, float userLon, float searchRadius) {
  HTTPClient http;
  
  // Build URL with location bounds
  String url = String(flightRadarAPI);
  url += "?bounds=";
  url += String(userLat + searchRadius, 4);  // top
  url += ",";
  url += String(userLon - searchRadius, 4);  // left
  url += ",";
  url += String(userLat - searchRadius, 4);  // bottom
  url += ",";
  url += String(userLon + searchRadius, 4);  // right
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    parseFlightData(payload);
  } else {
    Serial.printf("HTTP Error: %d\n", httpCode);
  }
  
  http.end();
}

void parseFlightData(String payload) {
  // Parse JSON response from FlightRadar24
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }
  
  aircraftCount = 0;
  
  for (JsonPair p : doc.as<JsonObject>()) {
    if (aircraftCount >= 50) break;
    
    JsonArray arr = p.value().as<JsonArray>();
    if (arr.size() >= 4) {
      aircraft[aircraftCount].lat = arr[1].as<float>();
      aircraft[aircraftCount].lon = arr[2].as<float>();
      aircraft[aircraftCount].altitude = arr[4].as<float>();
      aircraft[aircraftCount].speed = arr[5].as<float>();
      aircraft[aircraftCount].callsign = arr[13].as<String>();
      
      aircraftCount++;
    }
  }
  
  Serial.printf("Found %d aircraft\n", aircraftCount);
}

void updateRadarDisplay(Config cfg) {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  
  // Update heartbeat phase
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    heartbeatPhase = 0;
  }
  
  heartbeatPhase++;
  int brightness = 20 + (heartbeatPhase < 10 ? heartbeatPhase * 3 : 0);
  
  // Draw radar
  drawRadarBackground(brightness, cfg.radarRadius);
  drawAircraft(cfg.latitude, cfg.longitude, cfg.radarRadius);
  drawUserLocation();
  drawInfo(cfg);
}

void drawRadarBackground(int intensity, int radarRadius) {
  tft.fillScreen(TFT_BLACK);
  
  // Draw concentric circles (radar rings)
  int ringColor = tft.color565(intensity, intensity / 2, intensity / 2);
  
  // Outer ring
  tft.drawCircle(CENTER_X, CENTER_Y, radarRadius, ringColor);
  tft.drawCircle(CENTER_X, CENTER_Y, radarRadius / 2, ringColor);
  tft.drawCircle(CENTER_X, CENTER_Y, radarRadius / 4, ringColor);
  
  // Crosshairs
  tft.drawLine(CENTER_X - radarRadius, CENTER_Y, CENTER_X + radarRadius, CENTER_Y, ringColor);
  tft.drawLine(CENTER_X, CENTER_Y - radarRadius, CENTER_X, CENTER_Y + radarRadius, ringColor);
  
  // Diagonal lines
  int offset = (radarRadius * 0.707); // 45 degrees
  tft.drawLine(CENTER_X - offset, CENTER_Y - offset, CENTER_X + offset, CENTER_Y + offset, ringColor);
  tft.drawLine(CENTER_X - offset, CENTER_Y + offset, CENTER_X + offset, CENTER_Y - offset, ringColor);
}

void drawAircraft(float userLat, float userLon, int radarRadius) {
  for (int i = 0; i < aircraftCount; i++) {
    float dx = aircraft[i].lon - userLon;
    float dy = aircraft[i].lat - userLat;
    
    // Convert to screen coordinates (simple projection)
    int screenX = CENTER_X + (dx * 50);  // Scale factor
    int screenY = CENTER_Y - (dy * 50);  // Latitude inverted
    
    // Check if within radar range
    if (abs(screenX - CENTER_X) < radarRadius && abs(screenY - CENTER_Y) < radarRadius) {
      // Color based on altitude
      uint16_t color = getAltitudeColor(aircraft[i].altitude);
      
      // Draw aircraft as small dot
      tft.fillCircle(screenX, screenY, 3, color);
      
      // Heartbeat pulse effect
      if (heartbeatPhase < 5) {
        tft.drawCircle(screenX, screenY, 6 + heartbeatPhase, color);
      }
    }
  }
}

void drawUserLocation() {
  // Draw user position at center with crosshair
  tft.fillCircle(CENTER_X, CENTER_Y, 4, TFT_GREEN);
  tft.drawCircle(CENTER_X, CENTER_Y, 8, TFT_GREEN);
}

void drawInfo(Config cfg) {
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  
  // Aircraft count
  tft.drawString("Aircraft: " + String(aircraftCount), 5, 5);
  
  // Signal strength
  int rssi = WiFi.RSSI();
  tft.drawString("RSSI: " + String(rssi) + " dBm", 5, 20);
  
  // Location
  char locStr[30];
  snprintf(locStr, sizeof(locStr), "%.2f, %.2f", cfg.latitude, cfg.longitude);
  tft.drawString(locStr, 5, 215);
  
  // Scale info
  tft.setTextDatum(TR_DATUM);
  tft.drawString("1.5deg", SCREEN_WIDTH - 5, 5);
  
  // Menu hint
  tft.setTextColor(TFT_ORANGE);
  tft.setTextDatum(BL_DATUM);
  tft.drawString("Press UP for menu", 5, SCREEN_HEIGHT - 5);
}

uint16_t getAltitudeColor(float altitude) {
  // Color gradient based on altitude
  if (altitude < 5000) return TFT_BLUE;      // Low altitude
  if (altitude < 15000) return TFT_CYAN;     // Medium-low
  if (altitude < 25000) return TFT_YELLOW;   // Medium
  if (altitude < 35000) return TFT_ORANGE;   // Medium-high
  return TFT_RED;                            // High altitude
}
