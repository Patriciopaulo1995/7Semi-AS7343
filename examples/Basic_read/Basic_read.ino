/**
 * 7Semi AS7343 Spectral Sensor Example
 *
 * Library:
 * - Name    : 7Semi_AS7343
 * - Platform: Arduino / Embedded (I2C)
 *
 * Description:
 * - Initializes AS7343 spectral sensor
 * - Reads spectral channel data
 * - Detects flicker frequency (100/120Hz)
 * - Converts spectral data to approximate RGB
 * - Prints results to Serial Monitor
 *
 * Connections:
 * -----------------------------
 * AS7343    →   MCU
 * -----------------------------
 * VCC       →   3.3V / 5V
 * GND       →   GND
 * SDA       →   SDA (Wire)
 * SCL       →   SCL (Wire)
 * INT       →   Optional (Interrupt pin)
 *
 * Notes:
 * - Default I2C Address: 0x39
 * - Use proper pull-up resistors on SDA/SCL if required
 * - For best results, use stable light source
 */

#include <7Semi_AS7343.h>

AS7343_7Semi sensor;

void setup() {
  Serial.begin(115200);

  if (!sensor.begin(0x39, Wire, 400000)) {
    Serial.println("AS7343 init failed!");
    while (1)
      ;
  }

  Serial.println("AS7343 initialized");

  // Sensor configuration
  sensor.setAutoSMUX(AS7343_Auto_SMUX::AUTO_18CH);
  sensor.setGain(Gain::GAIN_2X);
  sensor.setIntegrationTime(100, 999);
  sensor.setSpectralEnable(true);

  // Enable flicker detection
  sensor.setFlickerEnable(true);

  // Turn on onboard LED 
  sensor.setLED(true, 4);
}

void loop() {
  AS7343_SpectralData data;

  // Read spectral data
  if (sensor.readSpectralData(data)) {
    Serial.print("Red: ");
    Serial.print(data.RED_690);

    Serial.print(" | Green: ");
    Serial.print(data.GREEN_550);

    Serial.print(" | Blue: ");
    Serial.println(data.BLUE_425);
  }

  // Read flicker frequency
  uint16_t freq = 0;
  bool detected = false;

  if (sensor.getFlickerInfo(detected, freq)) {
    if (detected) {
      Serial.print("Flicker: ");
      Serial.print(freq);
      Serial.println(" Hz");
    } else {
      Serial.println("No Flicker");
    }
  }

  delay(500);
}