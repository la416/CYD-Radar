# CYD Radar - Aircraft Detection for ESP32 2432S028

A real-time aircraft radar display for the **CYD (Cheap Yellow Display)** using an **ESP32-2432S028** microcontroller. Displays nearby aircraft positions in a heartbeat sensor style visualization using FlightRadar24 API data.

## Features

- 🛩️ Real-time aircraft detection using FlightRadar24 API
- 💛 CYD 2.4" TFT display visualization
- 💓 Heartbeat animation pulse for aircraft detection
- 🌈 Altitude-based color coding
- 📡 WiFi connectivity
- 🎯 Radar-style circular display with concentric rings
- 📊 Aircraft count and signal strength display

## Hardware Requirements

- **ESP32-2432S028** (CYD - Cheap Yellow Display)
  - Built-in 2.4" TFT display (320x240)
  - ESP32 microcontroller
  - Built-in USB-C charging
- **USB-C Cable** for programming and power
- **WiFi connection** (2.4GHz)

## Arduino IDE Setup

### 1. Install ESP32 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add this URL to "Additional Boards Manager URLs":
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "esp32" and install **esp32 by Espressif Systems**

### 2. Install Required Libraries
In Arduino IDE, go to **Sketch → Include Library → Manage Libraries** and install:

- **TFT_eSPI** by Bodmer
- **ArduinoJson** by Benoit Blanchon
- **WiFi** (built-in with ESP32)
- **HTTPClient** (built-in with ESP32)

### 3. Configure TFT_eSPI for CYD
1. Locate the TFT_eSPI library folder (usually `~/Arduino/libraries/TFT_eSPI/`)
2. Open `User_Setup.h`
3. Comment out any existing driver and add:
   ```cpp
   #define ILI9341_DRIVER
   #define TFT_WIDTH  320
   #define TFT_HEIGHT 240
   #define TFT_CS   GPIO_NUM_27
   #define TFT_DC   GPIO_NUM_26
   #define TFT_SDA  GPIO_NUM_21
   #define TFT_SCL  GPIO_NUM_22
   #define TFT_RST  GPIO_NUM_5
   #define TFT_BL   GPIO_NUM_4
   #define TFT_BACKLIGHT_ON HIGH
   #define SPI_FREQUENCY  40000000
   ```

### 4. Board Settings
In Arduino IDE, set:
- **Board**: ESP32-WROOM-32
- **Upload Speed**: 115200
- **Flash Size**: 4MB
- **Partition Scheme**: Default 4MB with spiffs
- **Port**: COM/USB port of your CYD

## Configuration

Edit the sketch to add your WiFi credentials:

```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

Set your location (latitude/longitude):

```cpp
const float USER_LAT = 51.5074;   // Your latitude
const float USER_LON = -0.1278;   // Your longitude
```

## How It Works

1. **Startup**: Connects to WiFi and initializes the display
2. **Data Fetching**: Queries FlightRadar24 API every 5 seconds for aircraft in a ~150km radius
3. **Display**: Shows aircraft as colored dots on a radar screen:
   - **Blue** = Low altitude (< 5,000 ft)
   - **Cyan** = Medium-low (5,000-15,000 ft)
   - **Yellow** = Medium (15,000-25,000 ft)
   - **Orange** = Medium-high (25,000-35,000 ft)
   - **Red** = High altitude (> 35,000 ft)
4. **Heartbeat Effect**: Pulse animation around detected aircraft

## Display Layout

```
┌─────────────────────────────┐
│ Aircraft: 12  RSSI: -45 dBm │
│                             │
│       ◆ Radar Rings         │
│    ◆    ◆  ◆               │
│ ◆─────●─────◆ (Center: You)│
│   ◆  ◆    ◆                │
│       ◆                     │
│                             │
│     Scale: 1.5deg          │
└─────────────────────────────┘
```

## API Notes

- Uses **FlightRadar24 Community API** (free tier)
- Data updates every 5 seconds
- Requests aircraft within ~1.5 degrees (±150km) of your location
- No API key required for basic functionality

## Troubleshooting

| Issue | Solution |
|-------|----------|
| WiFi won't connect | Check SSID/password, ensure 2.4GHz network |
| Display shows nothing | Verify TFT_eSPI pin configuration in User_Setup.h |
| No aircraft detected | Check latitude/longitude coordinates, ensure planes flying nearby |
| Library compilation errors | Ensure all libraries are latest version from Arduino IDE |

## Future Enhancements

- [ ] Aircraft trail visualization
- [ ] Callsign display for detected aircraft
- [ ] Speed/direction vector arrows
- [ ] Web interface for location setup
- [ ] Local WiFi AP mode for configuration
- [ ] Altitude filter settings
- [ ] OLED/e-ink display support

## References

- [CYD GitHub](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)
- [TFT_eSPI Documentation](https://github.com/Bodmer/TFT_eSPI)
- [FlightRadar24 API](https://www.flightradar24.com/share/community)
- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

## License

MIT License - Feel free to modify and distribute

## Author

Created for CYD display + ESP32-2432S028
