# YDIBDGTCH

An abbreviation YDIBDGTCH comes from shuffling the following DIYDGTCHB (which stands for DIY DGT Chess Board) and stands for Why Do I Bring DGT (into) Chess.

## Conceptual scheme and requirement

The board contains different systems:

1. Piece sensing system - represented by a matrix of magnetic field sensors (reed switches)
2. Cell illumination system - represented by an LED strip, each k=2 consequent LEDs are placed in each board cell
3. Board interaction system - represented by other illumination means (LED strip or display and/or additional LEDs) and user input means (button(s) and/or encoder(s) etc.)

An additional __User interaction system__ extends the field covered by the "board interaction system" and allows the user to adjust other systems behavior (implements interfaces for adjusting illumination preferences, board mode). The following requirements stand for the user interaction system:

1. Interface to control board behavior - user can set preferences on board's behavior (different illumination modes, board modes)
2. Interface to register moves times - (wired) connection to chess timer to accurately track move timings
3. Interface to record and share played games - record moves and later share to user (e.g. for further computer analysis)
4. Interface for streaming games to online platforms (e.g. lichess.com)

## Wiring diagram

### Board-controller

The following components need to be connected:

- ESP32 - 48 multifunctional pins
- Reed Switch matrix - 8 input pins + 8 output pins
- LED strip - 3 pins

| Power Supply | ESP32  | Reed Switch matrix | LED strip | BTN |
|--------------|--------|--------------------|-----------|-----|
| GND          | GND    |                    | GND       | 1   |
| +5V          |        |                    | VIN       |     |
|              | GPIO4  |                    | Data      |     |
|              | GPIO15 |                    |           | 2   |
|              | GPIO16 | RS-0               |           |     |
|              | GPIO17 | RS-1               |           |     |
|              | GPIO5  | RS-2               |           |     |
|              | GPIO18 | RS-3               |           |     |
|              | GPIO19 | RS-4               |           |     |
|              | GPIO21 | RS-5               |           |     |
|              | GPIO22 | RS-6               |           |     |
|              | GPIO23 | RS-7               |           |     |
|              | GPIO14 | D-0                |           |     |
|              | GPIO27 | D-1                |           |     |
|              | GPIO26 | D-2                |           |     |
|              | GPIO25 | D-3                |           |     |
|              | GPIO13 | D-4                |           |     |
|              | GPIO12 | D-5                |           |     |
|              | GPIO33 | D-6                |           |     |
|              | GPIO32 | D-7                |           |     |

# Types of board lighting:

- Showing currently sensed pieces
- Showing if illegal move was made
- Showing if in check
- Showing if checkmated
- Showing available places for the piece

# TODOs
- get rid of sets/unordered_sets in favor of vector
	- or implement more memory-efficient container myself

# Technical details
- Pieces can be covered with felt tape from the bottom to make collisions between pieces smooth