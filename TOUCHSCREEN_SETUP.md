# Touchscreen Setup & Calibration Guide

## Installation Requirements

### 1. Install XPT2046 Touchscreen Library

In Arduino IDE:
1. **Sketch → Include Library → Manage Libraries**
2. Search: `XPT2046_Touchscreen`
3. Install by **Paul Stoffregen** (latest version)

### 2. Touchscreen Hardware Pinout

Your CYD 2432S028 touchscreen uses the XPT2046 controller:

| Pin | GPIO | Purpose |
|-----|------|---------|
| CS (Chip Select) | 33 | SPI Chip Select |
| IRQ (Interrupt) | 36 | Touch Interrupt |
| CLK | 18 | SPI Clock |
| MOSI | 23 | SPI Data In |
| MISO | 19 | SPI Data Out |
| GND | GND | Ground |
| VCC | 3.3V | Power (3.3V) |

**Note:** CLK, MOSI, MISO are shared with the display SPI bus.

---

## Quick Calibration

### Automatic Touch Mapping

The system uses raw XPT2046 coordinates (0-4095) mapped to screen pixels (320x240).

Default calibration values in `touchscreen.h`:
```cpp
#define TOUCH_MIN_X 150    // Left edge raw value
#define TOUCH_MAX_X 3800   // Right edge raw value
#define TOUCH_MIN_Y 150    // Top edge raw value
#define TOUCH_MAX_Y 3800   // Bottom edge raw value
```

If your touchscreen feels inaccurate, you'll need to calibrate these values.

### Manual Calibration Steps

1. **Create a test sketch** to read raw touch values:

```cpp
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#define TOUCH_CS 33
#define TOUCH_IRQ 36

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  ts.begin();
  Serial.println("Touch Calibration - Press corners");
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    Serial.print("Raw X: ");
    Serial.print(p.x);
    Serial.print(" Raw Y: ");
    Serial.print(p.y);
    Serial.print(" Pressure: ");
    Serial.println(p.z);
    delay(100);
  }
}
```

2. **Upload and open Serial Monitor** (9600 baud)

3. **Touch each corner** and record the raw values:

| Position | X Value | Y Value |
|----------|---------|---------|
| **Top-Left** | TOUCH_MIN_X | TOUCH_MIN_Y |
| **Top-Right** | TOUCH_MAX_X | TOUCH_MIN_Y |
| **Bottom-Left** | TOUCH_MIN_X | TOUCH_MAX_Y |
| **Bottom-Right** | TOUCH_MAX_X | TOUCH_MAX_Y |

4. **Update `touchscreen.h`** with your values:

```cpp
#define TOUCH_MIN_X 150     // Your top-left X
#define TOUCH_MAX_X 3800    // Your bottom-right X
#define TOUCH_MIN_Y 150     // Your top-left Y
#define TOUCH_MAX_Y 3800    // Your bottom-right Y
```

5. **Recompile and upload** your main sketch

---

## Touchscreen Button Layout

The menu displays four navigation buttons in the corners:

```
┌────────────────────────────────┐
│ [UP]          TITLE          [BACK]
│                                │
│   Menu Item 1  (tap to select) │
│   Menu Item 2  (tap to select) │
│   Menu Item 3  (tap to select) │
│   Menu Item 4  (tap to select) │
│   Menu Item 5  (tap to select) │
│                                │
│ [DOWN]                      [SEL]
└────────────────────────────────┘
```

### Button Functions

| Button | Position | Action |
|--------|----------|--------|
| **UP** | Top-Left (10,10) | Scroll menu up |
| **DOWN** | Bottom-Left (10,190) | Scroll menu down |
| **SELECT** (green) | Bottom-Right (260,190) | Confirm/Enter menu |
| **BACK** (red) | Top-Right (260,10) | Return to main menu |

### Direct Item Selection

You can **tap menu items directly** to select them instead of using UP/DOWN/SELECT buttons.

---

## Troubleshooting Touchscreen

### Touch Doesn't Respond

**Problem:** Screen doesn't register touches

**Solutions:**
1. Check GPIO33 and GPIO36 connections
2. Verify `XPT2046_Touchscreen` library is installed
3. Run calibration test sketch to confirm raw values
4. Check if touchscreen is receiving 3.3V power

### Touch is Offset/Inverted

**Problem:** Touch point doesn't match screen location

**Solutions:**
1. Run manual calibration (see above)
2. Verify display rotation matches touchscreen rotation
3. Check if touchscreen is installed backwards

```cpp
// In touchscreen.cpp init:
ts.setRotation(1);  // Match your display rotation (0-3)
```

### Wrong Coordinates After Update

**Problem:** Touch worked before, now it's wrong

**Solutions:**
1. Recalibrate after updating libraries
2. Check if TOUCH_MIN/MAX values were accidentally changed
3. Verify SPI bus is not shared with conflicting devices

---

## Advanced: Custom Touch Calibration

For production applications, implement a full calibration routine:

```cpp
void TouchHandler::calibrateTouch() {
  int minX = 4095, maxX = 0;
  int minY = 4095, maxY = 0;
  
  Serial.println("Calibration: Touch all 4 corners");
  delay(1000);
  
  // Collect points for 5 seconds
  unsigned long endTime = millis() + 5000;
  while (millis() < endTime) {
    if (ts.touched()) {
      TS_Point p = ts.getPoint();
      minX = min(minX, p.x);
      maxX = max(maxX, p.x);
      minY = min(minY, p.y);
      maxY = max(maxY, p.y);
    }
  }
  
  // Save to EEPROM
  EEPROM.writeInt(100, minX);
  EEPROM.writeInt(104, maxX);
  EEPROM.writeInt(108, minY);
  EEPROM.writeInt(112, maxY);
  EEPROM.commit();
  
  Serial.println("Calibration complete!");
}
```

---

## Performance Tips

✅ **Best Practices:**
- Touch debounce is set to 50ms (prevents false triggers)
- Raw values are mapped using Arduino's `map()` function
- Pressure detection (Z-axis) available for enhanced accuracy
- IRQ pin enables interrupt-driven touch detection

⚡ **Optimization:**
- Use `ts.touched()` before calling `getPoint()` to save power
- Implement touch debouncing (200ms delay after detection)
- Cache screen dimensions to avoid recalculation

---

## Integration with Your CYD Radar

The menu system automatically uses **both buttons and touchscreen**:

```cpp
// In your main sketch setup:
RadarMenu menu(&tft, &touch);
menu.init();

// In your main loop:
void loop() {
  menu.update();  // Handles both button and touch input
}
```

**Control Priority:**
1. Touchscreen buttons (UP/DOWN/SELECT/BACK)
2. Direct menu item touch selection
3. Hardware physical buttons (GPIO0/GPIO35)

---

## Next Steps

1. ✅ Install `XPT2046_Touchscreen` library
2. ✅ Verify touchscreen connections (CS=GPIO33, IRQ=GPIO36)
3. ✅ Run calibration test if touches feel offset
4. ✅ Upload CYD_Radar.ino to your device
5. ✅ Test menu navigation with both touches and buttons
6. ✅ Enjoy your touchscreen-enabled aircraft radar! 🎯

For more info, see:
- [XPT2046 Library](https://github.com/PaulStoffregen/XPT2046_Touchscreen)
- [CYD 2432S028 Documentation](https://www.makerfabs.com/article/esp32-2432s028.html)
