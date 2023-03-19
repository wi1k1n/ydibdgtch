#ifndef SENSEBOARD_H__
#define SENSEBOARD_H__

#include "constants.h"

#include <Arduino.h>

const uint8_t PINS_INPUT[] = PINS_INPUT_LIST;
const uint8_t PINS_OUTPUT[] = PINS_OUTPUT_LIST;
constexpr uint8_t PINS_INPUT_SIZE = sizeof(PINS_INPUT) / sizeof(PINS_INPUT[0]);
constexpr uint8_t PINS_OUTPUT_SIZE = sizeof(PINS_OUTPUT) / sizeof(PINS_OUTPUT[0]);

class SenseBoard {
	uint8_t _states[8];
public:
	SenseBoard() = default;

	bool init();
	void scan();
	bool getState(uint8_t row, uint8_t col) const;
	void setState(uint8_t row, uint8_t col, bool val);
	bool getState(uint8_t idx) const;
	void setState(uint8_t idx, bool val);
	void print() const;
	uint8_t getActiveStatesCount() const;

	static bool readPin(uint8_t pin);
	static void writePin(uint8_t pin, bool val);
};

#endif // SENSEBOARD_H__