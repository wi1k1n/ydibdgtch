# YDIBDGTCH

An abbreviation YDIBDGTCH comes from shuffling the following DIYDGTCHB (which stands for DIY DGT Chess Board) and stands for Why Do I Bring DGT (into) Chess.

## Conceptual scheme and requirement

The board contains different systems:

1. Piece sensing system - represented by a matrix of magnetic field sensors (reed switches)
2. Cell illumination system - represented by an LED strip, each k=2 consequent LEDs are placed in each board cell
3. Board interaction system - represented by other illumination means (LED strip or display and/or additional LEDs) and user input means (button(s) and/or encoder(s) etc.) _TODO: likely better to include it into semantics system_

The board-controller encapsulates control over these systems. Arduino-based controller (e.g. Arduino Nano) is used for controlling these systems.

Another system brings chess semantics to the project on top of board-controller, implemented as a separate controller with some user-end interface (wireless: wifi and/or bluetooth; or wired: e.g. serial via USB cable), e.g. ESP8266/ESP32 controller. Semantics system is responsible for interfacing user needs (illumination preferences, board modes) and communicates with board-controller via wired interface (e.g. I2C). The following requirements stand for the semantics system:

1. Interface to control board behavior - user can set preferences on board's behavior (different illumination modes, board modes)
2. Interface to register moves times - (wired) connection to chess timer to accurately track move timings
3. Interface to record and share played games - record moves and later share to user (e.g. for further computer analysis)
4. Interface for streaming games to online platforms (e.g. lichess.com)

## Wiring diagram

### Board-controller

The following components need to be connected:

- Arduino Nano - 22 I/O pins
- Reed Switch matrix - 8 input pins + 8 output pins
- LED strip - 3 pins
- ESP8266 - 4 pins

| Power Supply | Arduino  | Reed Switch matrix | LED strip | ESP8266          |
|--------------|----------|--------------------|-----------|------------------|
|              | +3.3V    |                    |           | +3.3V            |
|              | +5V      |                    |           |                  |
| GND          | GND      |                    | GND       | GND              |
| +5V          |          |                    | VIN       |                  |
|              | A4 (SDA) |                    |           | D2 (GPIO4) (SDA) |
|              | A5 (SCL) |                    |           | D1 (GPIO5) (SCL) |
|              | D10      |                    | Data      |                  |
|              | D2       | I-1                |           |                  |
|              | D3       | I-2                |           |                  |
|              | D4       | I-3                |           |                  |
|              | D5       | I-4                |           |                  |
|              | D6       | I-5                |           |                  |
|              | D7       | I-6                |           |                  |
|              | D8       | I-7                |           |                  |
|              | D9       | I-8                |           |                  |
|              | D11      | O-1                |           |                  |
|              | D12      | O-2                |           |                  |
|              | A0       | O-3                |           |                  |
|              | A1       | O-4                |           |                  |
|              | A2       | O-5                |           |                  |
|              | A3       | O-6                |           |                  |
|              | A6       | O-7                |           |                  |
|              | A7       | O-8                |           |                  |

### Semantics System

Wemos D1 mini pro board has the following connections:
