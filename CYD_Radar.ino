#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>

// Display setup
TFT_eSPI tft = TFT_eSPI();

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// FlightRadar24 API
const char* flightRadarAPI = "https://api.flightradar24.com/common/v1/aircraft.json";

// Display parameters
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;
const int CENTER_X = SCREEN_WIDTH / 2;
const int CENTER_Y = SCREEN_HEIGHT / 2;
const int RADAR_RADIUS = 80;

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

// Your location (update these)
const float USER_LAT = 51.5074;  // Example: London
const float USER_LON = -0.1278;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  
  Serial.println("\n\nCYD Radar starting...");
  
  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Fetch aircraft data every 5 seconds
    if (millis() % 5000 < 100) {
      fetchFlightData();
    }
    
    // Update display with heartbeat animation
    updateRadarDisplay();
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("WiFi Disconnected", CENTER_X, CENTER_Y);
  }
  
  delay(100);
}

void connectToWiFi() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Connecting WiFi...", CENTER_X, CENTER_Y);
  
  WiFi.begin(ssid, password);
  
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
  }
}

void fetchFlightData() {
  HTTPClient http;
  
  // Build URL with your location bounds (simplified)
  String url = String(flightRadarAPI);
  url += "?bounds=";
  url += String(USER_LAT + 1.5, 4);  // top
  url += ",";
  url += String(USER_LON - 1.5, 4);  // left
  url += ",";
  url += String(USER_LAT - 1.5, 4);  // bottom
  url += ",";
  url += String(USER_LON + 1.5, 4);  // right
  
  Serial.println("Fetching: " + url);
  
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

void updateRadarDisplay() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  
  // Update display every 100ms
  if (now - lastUpdate < 100) return;
  lastUpdate = now;
  
  // Clear screen (with fade effect for heartbeat)
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    heartbeatPhase = 0;
  }
  
  heartbeatPhase++;
  int brightness = 20 + (heartbeatPhase < 10 ? heartbeatPhase * 3 : 0);
  
  // Draw radar
  drawRadarBackground(brightness);
  drawAircraft();
  drawUserLocation();
  drawInfo();
}

void drawRadarBackground(int intensity) {
  tft.fillScreen(TFT_BLACK);
  
  // Draw concentric circles (radar rings)
  int ringColor = tft.color565(intensity, intensity / 2, intensity / 2);
  
  // Outer ring
  tft.drawCircle(CENTER_X, CENTER_Y, RADAR_RADIUS, ringColor);
  tft.drawCircle(CENTER_X, CENTER_Y, RADAR_RADIUS / 2, ringColor);
  tft.drawCircle(CENTER_X, CENTER_Y, RADAR_RADIUS / 4, ringColor);
  
  // Crosshairs
  tft.drawLine(CENTER_X - RADAR_RADIUS, CENTER_Y, CENTER_X + RADAR_RADIUS, CENTER_Y, ringColor);
  tft.drawLine(CENTER_X, CENTER_Y - RADAR_RADIUS, CENTER_X, CENTER_Y + RADAR_RADIUS, ringColor);
  
  // Diagonal lines
  int offset = (RADAR_RADIUS * 0.707); // 45 degrees
  tft.drawLine(CENTER_X - offset, CENTER_Y - offset, CENTER_X + offset, CENTER_Y + offset, ringColor);
  tft.drawLine(CENTER_X - offset, CENTER_Y + offset, CENTER_X + offset, CENTER_Y - offset, ringColor);
}

void drawAircraft() {
  for (int i = 0; i < aircraftCount; i++) {
    float dx = aircraft[i].lon - USER_LON;
    float dy = aircraft[i].lat - USER_LAT;
    
    // Convert to screen coordinates (simple projection)
    int screenX = CENTER_X + (dx * 50);  // Scale factor
    int screenY = CENTER_Y - (dy * 50);  // Latitude inverted
    
    // Check if within radar range
    if (abs(screenX - CENTER_X) < RADAR_RADIUS && abs(screenY - CENTER_Y) < RADAR_RADIUS) {
      // Color based on altitude
      uint16_t color = getAltitudeColor(aircraft[i].altitude);
      
      // Draw aircraft as small triangle/dot
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

void drawInfo() {
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  
  // Aircraft count
  tft.drawString("Aircraft: " + String(aircraftCount), 5, 5);
  
  // Signal strength
  int rssi = WiFi.RSSI();
  tft.drawString("RSSI: " + String(rssi) + " dBm", 5, 20);
  
  // Scale info
  tft.drawString("1.5deg", RADAR_RADIUS - 30, 5);
}

uint16_t getAltitudeColor(float altitude) {
  // Color gradient based on altitude
  if (altitude < 5000) return TFT_BLUE;      // Low altitude
  if (altitude < 15000) return TFT_CYAN;     // Medium-low
  if (altitude < 25000) return TFT_YELLOW;   // Medium
  if (altitude < 35000) return TFT_ORANGE;   // Medium-high
  return TFT_RED;                            // High altitude
}
