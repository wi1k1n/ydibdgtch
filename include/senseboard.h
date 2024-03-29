#ifndef SENSEBOARD_H__
#define SENSEBOARD_H__

#include "constants.h"
#include "rulesengine.h"
#include <algorithm>
#include <iterator>
#include <unordered_set>

#include <Arduino.h>

const uint8_t PINS_INPUT[] = PINS_INPUT_LIST;
const uint8_t PINS_OUTPUT[] = PINS_OUTPUT_LIST;
constexpr uint8_t PINS_INPUT_SIZE = sizeof(PINS_INPUT) / sizeof(PINS_INPUT[0]);
constexpr uint8_t PINS_OUTPUT_SIZE = sizeof(PINS_OUTPUT) / sizeof(PINS_OUTPUT[0]);

struct SenseBoardState
{
	uint8_t _rows[8];

	SenseBoardState() = default;
	SenseBoardState(const SenseBoardState& other) { std::copy(std::begin(other._rows), std::end(other._rows), std::begin(_rows)); }

	bool get(uint8_t row, uint8_t col) const { return (_rows[row] >> col) & 1u; }
	bool set(uint8_t row, uint8_t col, bool val) {
		_rows[row] ^= (-static_cast<uint8_t>(val) ^ _rows[row]) & (1u << col);
	}

	std::vector<ChessPieceLocation> operator-(const SenseBoardState& other) {
		std::vector<ChessPieceLocation> diffs;
		int64_t bits = reinterpret_cast<int64_t>(_rows) - reinterpret_cast<int64_t>(other._rows);
		for (uint8_t idx = 0; idx < 64 && bits; ++idx)
			if (bits & (1 << idx))
				diffs.push_back(ChessPieceLocation(idx));
		return std::move(diffs);
	}

	inline bool operator==(const SenseBoardState& rhs) const {
		return !memcmp(_rows, rhs._rows, 8);
	}
};

namespace std {
template<>
struct hash<SenseBoardState> {
	size_t operator()(const SenseBoardState& other) const {
		return hash<uint64_t>()(reinterpret_cast<uint64_t>(other._rows));
	}
};
} // namespace std

class SenseBoard {
	SenseBoardState _state;
public:
	SenseBoard() = default;

	bool init();
	void scan();

	const SenseBoardState& getState() const { return _state; }
	bool getState(uint8_t row, uint8_t col) const;
	bool getState(uint8_t idx) const;
	void setState(uint8_t row, uint8_t col, bool val);
	void setState(uint8_t idx, bool val);

	void print() const;

	static bool readPin(uint8_t pin);
	static void writePin(uint8_t pin, bool val);
};

#endif // SENSEBOARD_H__