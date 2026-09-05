# CYD Radar - Setup Guide

## Arduino IDE Configuration

### Step 1: Install ESP32 Board Support

1. **Open Arduino IDE**
2. Go to **File → Preferences**
3. In "Additional Boards Manager URLs", paste:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Click **OK**
5. Go to **Tools → Board → Boards Manager**
6. Search for "esp32"
7. Install **esp32 by Espressif Systems** (latest version)

### Step 2: Install Required Libraries

Open **Sketch → Include Library → Manage Libraries** and search for:

1. **TFT_eSPI** by Bodmer
   - Latest version recommended
   - Used for display control

2. **ArduinoJson** by Benoit Blanchon
   - v6.x or newer
   - Used for parsing FlightRadar24 API

3. **WiFi** (Built-in - no installation needed)

4. **HTTPClient** (Built-in - no installation needed)

### Step 3: Configure TFT_eSPI for CYD Display

1. Locate TFT_eSPI library folder:
   - **Windows**: `Documents/Arduino/libraries/TFT_eSPI/`
   - **Mac**: `~/Arduino/libraries/TFT_eSPI/`
   - **Linux**: `~/Arduino/libraries/TFT_eSPI/`

2. Open `User_Setup.h` file

3. **Comment out** any existing display driver:
   ```cpp
   // #define ST7735_DRIVER
   // #define ILI9163_DRIVER
   ```

4. **Add** these lines for CYD 2432S028:
   ```cpp
   #define ILI9341_DRIVER
   #define TFT_WIDTH  320
   #define TFT_HEIGHT 240
   
   // Pin definitions for CYD 2432S028
   #define TFT_CS   GPIO_NUM_27
   #define TFT_DC   GPIO_NUM_26
   #define TFT_SDA  GPIO_NUM_21
   #define TFT_SCL  GPIO_NUM_22
   #define TFT_RST  GPIO_NUM_5
   #define TFT_BL   GPIO_NUM_4
   
   #define TFT_BACKLIGHT_ON HIGH
   #define SPI_FREQUENCY  40000000
   ```

5. Save and close

### Step 4: Board Selection

In Arduino IDE, go to **Tools** and set:

| Setting | Value |
|---------|-------|
| Board | ESP32-WROOM-32 |
| Upload Speed | 115200 |
| CPU Frequency | 240 MHz |
| Flash Size | 4MB |
| Flash Mode | QIO |
| Partition Scheme | Default 4MB with spiffs |
| Core Debug Level | None |
| Port | COM3 (or your CYD port) |

### Step 5: WiFi Configuration

Edit `menu.cpp` and update line 30-31:

```cpp
strcpy(config.ssid, "YOUR_WIFI_NETWORK");
strcpy(config.password, "YOUR_WIFI_PASSWORD");
```

Make sure you're using **2.4GHz WiFi** (5GHz not supported on ESP32)

### Step 6: Location Setup

Edit `menu.cpp` and update line 32-33:

```cpp
config.latitude = 51.5074;   // Change to your latitude
config.longitude = -0.1278;  // Change to your longitude
```

**Finding Coordinates:**
- Use Google Maps: Right-click location → Copy coordinates
- Format: Latitude (N/S), Longitude (E/W)
- Examples:
  - New York: 40.7128, -74.0060
  - London: 51.5074, -0.1278
  - Tokyo: 35.6762, 139.6503
  - Sydney: -33.8688, 151.2093

### Step 7: Compile and Upload

1. Open `CYD_Radar.ino`
2. Click **Sketch → Verify** to compile
3. If successful, click **Upload**
4. Wait for "Upload complete"

### Step 8: Test the Display

1. Open **Tools → Serial Monitor** (9600 baud)
2. You should see startup messages:
   ```
   CYD Radar starting...
   WiFi Connected!
   Found 12 aircraft
   ```

3. On the display:
   - "CYD RADAR" splash screen appears
   - "Press UP to configure" message
   - Menu appears when UP button pressed

---

## Button Mapping

The CYD has two built-in buttons:

| Button | GPIO | Location | Function in Menu |
|--------|------|----------|------------------|
| UP | 0 | Bottom left | Navigate up / Decrement |
| DOWN | 35 | Bottom right | Navigate down / Select |

---

## Troubleshooting Setup

### Arduino IDE Can't Find Board

**Solution:**
- Check: Tools → Board → Boards Manager
- Search "esp32" and verify installed
- Restart Arduino IDE
- Check USB cable (data cable, not charge-only)

### Upload Fails - "Serial Port Not Found"

**Solution:**
- Reconnect CYD with USB cable
- Try different USB ports
- Check Device Manager (Windows) or System Report (Mac)
- Install CH340 drivers if needed:
  - Windows/Mac/Linux: https://www.wemos.cc/en/latest/ch340_driver.html

### Display Stays Black

**Solution:**
1. Verify TFT_eSPI User_Setup.h pin configuration
2. Check power to CYD (should show LED light)
3. Verify ILI9341_DRIVER is enabled
4. Check backlight GPIO4 is configured

### WiFi Won't Connect

**Solution:**
- Verify SSID/password in menu.cpp
- Ensure 2.4GHz WiFi network (not 5GHz)
- Check WiFi is within range
- Try moving closer to router
- Restart CYD

### "JSON parse error"

**Solution:**
- Verify ArduinoJson is installed
- Check internet connection on WiFi
- Verify FlightRadar24 API is accessible
- Check if coordinates are in valid range

---

## First Run Checklist

- [ ] ESP32 board installed
- [ ] TFT_eSPI configured with correct pins
- [ ] ArduinoJson library installed
- [ ] WiFi SSID/password configured in code
- [ ] Latitude/longitude set correctly
- [ ] Sketch compiles without errors
- [ ] Upload successful to CYD
- [ ] Display shows splash screen
- [ ] Buttons respond to presses
- [ ] WiFi connects and shows aircraft

---

## Next Steps

Once setup is complete:
1. Read QUICKSTART.md for menu navigation
2. Customize location and display settings
3. Explore different airport areas
4. Adjust radar radius and update rate for best performance

Need help? Check the README.md for advanced troubleshooting.
