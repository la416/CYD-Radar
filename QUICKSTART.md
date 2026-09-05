# CYD Radar - Quick Start Guide

## Getting Started in 5 Minutes

### 1. Upload the Sketch
1. Open Arduino IDE
2. Open `CYD_Radar.ino`
3. Verify all libraries are installed (see README.md)
4. Select the correct board: **ESP32-WROOM-32**
5. Click **Upload**

### 2. First Boot - Configuration Menu
When the CYD starts, you'll see:
```
CYD RADAR
Press UP to configure
```

**Press the UP button (GPIO0)** to enter the configuration menu.

### 3. Main Menu Navigation

Use the **UP and DOWN buttons** to navigate:
- **UP Button (GPIO0)**: Move cursor up
- **DOWN Button (GPIO35)**: Select option or adjust value

```
┌─────────────────────┐
│ MAIN MENU           │
│ > WiFi Settings     │  ← Selected (highlighted in blue)
│   Location Setup    │
│   Display Config    │
│   Radar Settings    │
│   Start Radar       │
└─────────────────────┘
```

### 4. Configure WiFi

Select **"WiFi Settings"** and press DOWN:

```
┌─────────────────────┐
│ WiFi Settings       │
│ > SSID: YourSSID    │
│   Password: ***     │
│   Back              │
└─────────────────────┘
```

Currently, you need to edit the config in the menu structure. Update in `menu.cpp`:
```cpp
strcpy(config.ssid, "YOUR_NETWORK_NAME");
strcpy(config.password, "YOUR_PASSWORD");
```

Then re-upload the sketch.

### 5. Configure Location

Select **"Location Setup"**:

```
┌─────────────────────┐
│ Location Setup      │
│ > Latitude: 51.5074 │
│   Longitude:-0.1278 │
│   Search Radius: 1.5│
└─────────────────────┘
```

- **UP Button**: Increment latitude
- **DOWN Button**: Decrement latitude
- Move to next option and repeat

**Popular Coordinates:**
- London: 51.5074, -0.1278
- New York: 40.7128, -74.0060
- Tokyo: 35.6762, 139.6503
- Sydney: -33.8688, 151.2093

### 6. Configure Display

Select **"Display Config"**:

```
┌─────────────────────┐
│ Display Config      │
│ > Radar Radius: 80  │
│   Update Rate: 100  │
│   Back              │
└─────────────────────┘
```

- **Radar Radius**: Size of radar circle (50-100 px recommended)
- **Update Rate**: Screen refresh rate in ms (50-200 ms)

### 7. Radar Settings

Select **"Radar Settings"**:

```
┌─────────────────────┐
│ Radar Settings      │
│ > Debug Mode: OFF   │
│   Info: Press DOWN  │
│   Back              │
└─────────────────────┘
```

- Toggle debug mode ON to see serial output
- Configure altitude filters and detection thresholds

### 8. Start the Radar

Select **"Start Radar"** and press DOWN:

The display will:
1. Show "Connecting WiFi..."
2. Connect to your configured WiFi network
3. Start fetching aircraft data
4. Display the radar screen

### 9. Radar Display

Once running, you'll see:

```
┌──────────────────────────────┐
│ Aircraft: 12  RSSI: -45 dBm  │
│                              │
│        ◆ Radar Display       │
│     ◆    ◆  ◆              │
│  ◆──────●──────◆            │
│    ◆  ◆    ◆               │
│        ◆                     │
│ 51.50, -0.12           1.5deg│
│                              │
│ Press UP for menu            │
└──────────────────────────────┘
```

**Display Elements:**
- **●** (center) = Your location
- **◆** (colored dots) = Aircraft
  - Blue = Low altitude
  - Cyan = Medium-low
  - Yellow = Medium
  - Orange = Medium-high
  - Red = High altitude
- **Rings** = Distance rings (every 50km)
- **RSSI** = WiFi signal strength
- **Aircraft count** = Number of planes detected

### 10. Return to Menu

**Press UP button** at any time to return to the main menu.

---

## Button Reference

| Button | Location | Function |
|--------|----------|----------|
| UP | GPIO0 (left side) | Scroll up / Decrement values / Return to menu |
| DOWN | GPIO35 (right side) | Scroll down / Select / Increment values |

---

## Troubleshooting

### "WiFi Disconnected" Message
- Check SSID and password in `menu.cpp`
- Ensure you're connecting to 2.4GHz WiFi (not 5GHz)
- Verify WiFi is in range

### No Aircraft Detected
- Check your latitude/longitude coordinates
- Ensure there are flights near your location
- Try a busier airport area for testing
- Increase search radius in Display Config menu

### Display Shows Garbled Text
- Verify TFT_eSPI pins in `menu.cpp` match your CYD
- Check User_Setup.h configuration (see README.md)

### Menu Won't Respond
- Press UP button to wake display
- Check button pins are not shorted
- Verify GPIO0 and GPIO35 connections

---

## Advanced: Editing WiFi Without Re-uploading

For easier WiFi configuration, you can add a simple text input system:

1. Open `menu.cpp`
2. In `displayWiFiMenu()`, add character input handling
3. Use serial monitor to input credentials

Or modify to connect to a temporary AP for web-based setup.

---

## Tips & Tricks

✅ **Best Practices:**
- Use a location near a busy airport for more aircraft
- Increase update rate (lower ms) for smoother animation
- Set radar radius to 80-100 px for best visibility
- Enable debug mode to monitor API calls

⚡ **Performance:**
- Keep WiFi signal strong (RSSI > -70 dBm)
- Don't set update rate below 50ms (causes slowdown)
- Limit search radius to 2-3 degrees for faster API responses

🎨 **Customization:**
- Modify altitude thresholds in `menu.cpp` for different color ranges
- Change heartbeat pulse timing in config
- Adjust scale factor in `drawAircraft()` for zoom level

---

## Next Steps

- **Add callsign display** - Show aircraft identifiers
- **Trail visualization** - Draw flight paths
- **Speed vectors** - Arrows showing direction
- **Web setup interface** - Configure via WiFi AP
- **Persistent storage** - Save settings to EEPROM

For updates and advanced features, check the repository!
