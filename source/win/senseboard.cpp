#include "senseboard.h"

SenseBoardState::SenseBoardState(const String& s) {
	m_rows = Array<uint8_t>(8);
	if (s.length() != 64)
		return;

	for (int i = 0; i < 64; ++i)
		Set(i, s[i] == '1');
}

bool SenseBoardState::Get(int8_t row, int8_t col) const {
	return (m_rows[row] >> col) & 1u;
}

bool SenseBoardState::Get(int8_t idx) const {
	return Get(idx / 8, idx % 8);
}

void SenseBoardState::Set(int8_t row, int8_t col, bool val) {
	m_rows[row] ^= (-static_cast<uint8_t>(val) ^ m_rows[row]) & (1u << col);
}

void SenseBoardState::Set(int8_t idx, bool value) {
	return Set(idx / 8, idx % 8, value);
}

String SenseBoardState::ToString() const {
	String s;
	for (int i = 0; i < 64; ++i)
		s += Get(i) ? '1' : '0';
	return s;
}
