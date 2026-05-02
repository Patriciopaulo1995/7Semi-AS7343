# 7Semi AS7343 Arduino Library

Arduino driver for the ams OSRAM AS7343 multi-channel spectral sensor.

The AS7343 provides advanced light sensing with multiple spectral channels, enabling applications such as color detection, light analysis, flicker detection, and environmental monitoring.

---

## Features

- Multi-channel spectral sensing (up to 18 channels)  
  - Covers visible + near-infrared spectrum

- SConfigurable SMUX modes
  - 6-channel / 12-channel / 18-channel operation

- FIFO buffer support
  - Continuous data streaming
  - Interrupt-based reading

- Gain control
  - Wide dynamic range (0.5× to 2048×)

- Adjustable integration time

- Flicker detection
  - Detects 100Hz / 120Hz light flicker

- LED control
  - Built-in LED driver with adjustable current

- Interrupt system
  - FIFO, spectral, and alert interrupts

- Threshold-based alerts

---

## Connections / Wiring

The AS7343 uses **I²C communication**.

---

## I²C Connection

| HDC302x Pin | MCU Pin         | Notes                |
|-------------|-----------------|----------------------|
| VCC         | 3.3V / 5V*      | Check module specs   |
| GND         | GND             | Common ground        |
| SDA         | SDA             | I²C data             |
| SCL         | SCL             | I²C clock            |
| INT         | GPIO (optional) | Alert interrupt pin  |

---

## I²C Notes

- Default I²C address: `0x44`  
- Requires pull-up resistors on SDA/SCL (4.7kΩ–10kΩ)  
- Supported bus speeds:
  - 100 kHz  
  - 400 kHz (recommended)  

---

## Installation

### Arduino Library Manager

1. Open Arduino IDE  
2. Go to Library Manager  
3. Search for **7Semi AS7343**  
4. Click Install  

---

### Manual Installation

1. Download repository as ZIP  
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library  

---

## Library Overview

---

### Initialize Sensor

```cpp
sensor.begin(0x39, Wire, 400000);
```

- Initializes I²C communication
- Verifies sensor presence

### Reading Spectral Data

```cpp
AS7343_SpectralData data;

sensor.readSpectralData(data);
```

- Returns all channel values (ch0 → ch17)
- Each channel represents a specific wavelength band

### Setting Gain

```cpp
sensor.setGain(Gain::GAIN_16X);
```

- Controls sensitivity
- Higher gain → higher sensitivity

### Setting Integration Time

```cpp
sensor.setIntegrationTime(100, 999);
```

- Adjusts exposure time
- Higher values → more light capture

### SMUX Mode Configuration

```cpp
sensor.setAutoSMUX(AS7343_Auto_SMUX::AUTO_18CH);
```

- AUTO_6CH → faster, fewer channels
- AUTO_12CH → balanced
- AUTO_18CH → full spectral data

### Flicker Detection

```cpp
bool detected;
uint16_t freq;

sensor.getFlickerInfo(detected, freq);
```

- Detects light flicker
- Returns frequency (100Hz / 120Hz)

### LED Control

```cpp
sensor.setLED(true, 50);
```

- Enables onboard LED
- Brightness in mA (approx range 4–258)

### FIFO Mode

```cpp
sensor.setFIFOMap(0x7F);
sensor.getFifoLevel(level);
sensor.readFifo(buffer, len);
```

- Enables buffered data streaming
- Useful for high-speed sampling

### Interrupt Configuration

```cpp
sensor.setInterruptEnable(false, false, true, false);
```

- Enables FIFO interrupt (example)
- Supports multiple interrupt sources

### RGB Approximation

```cpp
uint16_t r, g, b;

sensor.getRGB(r, g, b);
```

- Converts spectral data into approximate RGB values
- Useful for basic color detection

### Status Monitoring

```cpp
AS7343_Status status;

sensor.readStatus(status);
```

- Provides:
  - Interrupt flags
  - Saturation status
  - Error conditions

## Notes

- AS7343 is a spectral sensor, not a true RGB sensor
- RGB values are approximations and depend on configuration
- For best results:
  - Use proper gain and integration settings
  - Ensure stable lighting conditions

## Typical Applications

- Color sensing
- Light spectrum analysis
- Flicker detection
- Environmental monitoring
- Optical measurements

## License

MIT License
