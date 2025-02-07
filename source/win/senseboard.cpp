#include "senseboard.h"

bool SenseBoardState::Get(int8_t row, int8_t col) const {
	return (m_rows[row] >> col) & 1u;
}

void SenseBoardState::Set(int8_t row, int8_t col, bool val) {
	m_rows[row] ^= (-static_cast<uint8_t>(val) ^ m_rows[row]) & (1u << col);
}
