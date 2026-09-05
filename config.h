// Configuration file for CYD Radar
// Copy this file or use these settings in your main sketch

#ifndef CONFIG_H
#define CONFIG_H

// ============ WiFi Configuration ============
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// ============ Location Configuration ============
// Set your location here (latitude, longitude)
#define USER_LATITUDE 51.5074      // London example
#define USER_LONGITUDE -0.1278     // London example

// Search radius (in degrees, ~1.5 degrees ≈ 150km at equator)
#define SEARCH_RADIUS 1.5

// ============ Display Configuration ============
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define RADAR_RADIUS 80

// ============ API Configuration ============
#define FLIGHTRADAR_API "https://api.flightradar24.com/common/v1/aircraft.json"
#define API_UPDATE_INTERVAL 5000  // milliseconds

// ============ Display Update ============
#define DISPLAY_UPDATE_INTERVAL 100  // milliseconds

// ============ Heartbeat Animation ============
#define HEARTBEAT_INTERVAL 600  // milliseconds between pulses
#define MAX_AIRCRAFT 50         // Maximum aircraft to track

// ============ Altitude Thresholds (feet) ============
#define ALT_LOW 5000
#define ALT_MED_LOW 15000
#define ALT_MED 25000
#define ALT_MED_HIGH 35000

// ============ Display Colors (RGB565 format) ============
#define COLOR_RADAR_RING TFT_GREEN
#define COLOR_AIRCRAFT_LOW TFT_BLUE
#define COLOR_AIRCRAFT_MED_LOW TFT_CYAN
#define COLOR_AIRCRAFT_MED TFT_YELLOW
#define COLOR_AIRCRAFT_MED_HIGH TFT_ORANGE
#define COLOR_AIRCRAFT_HIGH TFT_RED
#define COLOR_USER_POS TFT_GREEN
#define COLOR_TEXT TFT_WHITE
#define COLOR_ERROR TFT_RED
#define COLOR_SUCCESS TFT_GREEN

// ============ Debug Options ============
#define DEBUG_SERIAL true
#define DEBUG_API_CALLS true
#define DEBUG_COORDINATES true

#endif // CONFIG_H
