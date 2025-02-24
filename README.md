# GPS and Microphone Module for Raspberry Pi Pico

## Overview

This repository contains the implementation of a **GPS and Microphone Module** for the **Raspberry Pi Pico** using **UART, ADC, DMA, and PWM**. The project consists of two main components:

1. **GPS Module**: Reads and processes NMEA sentences from a GPS module via UART.
2. **Microphone Module**: Captures and analyzes ambient noise levels using ADC and DMA.

## Features

- **GPS Module**:
  - Reads and parses **GGA sentences** to extract location data.
  - Converts **NMEA coordinates to decimal format**.
  - Generates a **Google Maps link** with the obtained latitude and longitude.
  - Uses **UART1** for communication with the GPS module.

- **Microphone Module**:
  - Uses **ADC and DMA** for efficient audio signal sampling.
  - Computes **average voltage** and converts it to **decibels (dB)**.
  - Implements **PWM** to trigger periodic ADC readings.

## Hardware Requirements

- **Raspberry Pi Pico**  
- **GPS Module (e.g., NEO-6M)**  
- **Analog Microphone Module**  
- Jumper wires

## Wiring Diagram

### GPS Module Connections:

| GPS Pin | Raspberry Pi Pico Pin |
|---------|-----------------------|
| VCC     | 3.3V                 |
| GND     | GND                   |
| TX      | GP5 (UART RX)         |
| RX      | GP4 (UART TX)         |

### Microphone Module Connections:

| Microphone Pin | Raspberry Pi Pico Pin |
|---------------|-----------------------|
| VCC           | 3.3V                 |
| GND           | GND                   |
| Ao (Analog)   | GP26 (ADC0)           |
| Do (Digital)  | **Not used**           |

## Installation

1. Clone this repository:
   ```sh
   git clone https://github.com/joseamorenom/GPS-Microphone-Module.git
   cd GPS-Microphone-Module
   ```

2. Compile and upload the code to the Raspberry Pi Pico using CMake and the Pico SDK.

## Code Structure
📂 GPS-Microphone-Module
│── 📄 gps.h            # GPS module function declarations
│── 📄 gps.c            # GPS module implementation
│── 📄 microphone.h     # Microphone module function declarations
│── 📄 microphone.c     # Microphone module implementation
│── 📄 main.c           # Main file integrating GPS and Microphone
│── 📄 CMakeLists.txt   # CMake build configuration
│── 📄 README.md        # Project documentation

## Usage
### GPS Module:
- Reads GPS data via UART1.
- Parses GGA sentences and extracts latitude, longitude, altitude.
- Prints data to the serial console and generates a Google Maps link.
### Microphone Module:
- Uses ADC and DMA for high-speed signal acquisition.
- Converts microphone readings into voltage and decibel levels.
- Displays real-time noise level analysis.
### Challenges and Solutions
- Calibrating the Microphone: Initial readings were inconsistent. We adjusted the ADC sampling rate and voltage reference to ensure accurate noise measurements.
- GPS Data Parsing: Extracting meaningful location data from raw NMEA sentences required handling multiple parameters and implementing robust parsing logic.
- DMA Configuration: Setting up DMA for ADC was challenging due to synchronization issues, but optimizing buffer handling improved data integrity.
## Future Improvements
- Implement real-time noise level visualization.
- Add GPS logging functionality to store location history.
- Improve power efficiency for battery-operated applications.
## License
This project is licensed under the MIT License.
