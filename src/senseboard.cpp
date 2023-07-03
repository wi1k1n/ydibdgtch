#include "senseboard.h"

bool SenseBoardInterface::getState(uint8_t row, uint8_t col) const {
	return _state.get(row, col);
}

void SenseBoardInterface::setState(uint8_t row, uint8_t col, bool val) {
	_state.set(row, col, val);
}

bool SenseBoardInterface::getState(uint8_t idx) const {
	return getState(idx / 8, idx % 8);
}

void SenseBoardInterface::setState(uint8_t idx, bool val) {
	return setState(idx / 8, idx % 8, val);
}

void SenseBoardInterface::print() const {
	for (uint8_t i = 0; i < 8; ++i) {
		for (uint8_t j = 0; j < 8; ++j)
			Serial.print(getState(i, j));
		Serial.println();
	}
	Serial.println();
	Serial.println();
}

//-----------------------------------------------------------------------------------------------------
bool SenseBoard::init() {
	for (uint8_t i = 0; i < PINS_INPUT_SIZE; ++i)
		pinMode(PINS_INPUT[i], INPUT_PULLDOWN);
	for (uint8_t i = 0; i < PINS_OUTPUT_SIZE; ++i)
		pinMode(PINS_OUTPUT[i], OUTPUT);
	return true;
}

void SenseBoard::scan() {
	for (uint8_t idxOut = 0; idxOut < PINS_OUTPUT_SIZE; ++idxOut) { // rows / reed_switches
		uint8_t pinOut = PINS_OUTPUT[idxOut];
		writePin(pinOut, 1);
		if (SENSEBOARD_SCAN_DELAY_MICROSECONDS > 0) // ESP32 is too fast, not needed with Arduino
			delayMicroseconds(SENSEBOARD_SCAN_DELAY_MICROSECONDS);
		for (uint8_t idxIn = 0; idxIn < PINS_INPUT_SIZE; ++idxIn) // cols / diodes
			setState(idxOut, idxIn, readPin(PINS_INPUT[idxIn]));
		writePin(pinOut, 0);
	}
}

bool SenseBoard::readPin(uint8_t pin) {
	return digitalRead(pin);
}
void SenseBoard::writePin(uint8_t pin, bool val) {
	digitalWrite(pin, val ? HIGH : LOW);
}

//-----------------------------------------------------------------------------------------------------
bool SenseBoardSerial::init() {
	Serial.begin(SERIAL_BAUDRATE);
	return true;
}

void SenseBoardSerial::scan() {
	while (Serial.available()) {
		PACKETTYPE type = static_cast<PACKETTYPE>(Serial.read());
		if (type == PACKETTYPE::STATE_UPDATE) {
			int count = Serial.readBytes(_buffer, 8);
			if (count < 8) {
				Serial.flush();
				return;
			}
			memcpy(_state._rows, _buffer, 8);
		}
	}
}