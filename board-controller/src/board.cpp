#include "board.h"

bool Board::begin() {
	for (uint8_t i = 0; i < PINS_INPUT_SIZE; ++i)
		pinMode(PINS_INPUT[i], INPUT_PULLUP);
	for (uint8_t i = 0; i < PINS_OUTPUT_SIZE; ++i)
		pinMode(PINS_OUTPUT[i], OUTPUT);
	return true;
}

void Board::scan() {
	for (uint8_t idxOut = 0; idxOut < PINS_OUTPUT_SIZE; ++idxOut) {
		uint8_t pinOut = PINS_OUTPUT[idxOut];
		digitalWrite(pinOut, LOW);
		for (uint8_t idxIn = 0; idxIn < PINS_INPUT_SIZE; ++idxIn)
			setState(idxIn, idxOut, !digitalRead(PINS_INPUT[idxIn])); // in and out are swapped, due to hardware wiring specialties, LOW -> reedswitch closed, HIGH - opened
		digitalWrite(pinOut, HIGH);
	}
}

bool Board::getState(uint8_t row, uint8_t col) const {
	return !((_states[row] >> col) & 1u);
}

void Board::setState(uint8_t row, uint8_t col, bool val) {
	_states[row] ^= (-static_cast<int8_t>(!val) ^ _states[row]) & (1u << col);
}

bool Board::getState(uint8_t idx) const {
	return getState(idx / 8, idx % 8);
}

void Board::setState(uint8_t idx, bool val) {
	return setState(idx / 8, idx % 8, val);
}

void Board::print() const {
	for (uint8_t i = 0; i < 8; ++i) {
		for (uint8_t j = 0; j < 8; ++j)
			Serial.print(getState(i, j));
		Serial.println();
	}
	Serial.println();
	Serial.println();
}

uint8_t Board::getActiveStatesCount() const {
	uint8_t sum = 0;
	for (uint8_t i = 0; i < 8; ++i) {
		uint8_t state = _states[i];
		while (state) {
		sum += state & 1u;
		state >>= 1u;
		}
	}
	return sum;
}