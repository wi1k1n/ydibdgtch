#include "senseboard.h"

SenseBoardState::SenseBoardState(const String& s) {
	m_rows = Array<uint8_t>(8);
	if (s.length() != 64)
		return;

	for (int i = 0; i < 64; ++i)
		Set(i, s[i] == '1');
}

bool SenseBoardState::Get(int8_t row, int8_t col) const { return (m_rows[row] >> col) & 1u; }
bool SenseBoardState::Get(int8_t idx) const { return Get(idx / 8, idx % 8); }
bool SenseBoardState::Get(const ChessPieceLocation& loc) const { return Get(loc.GetRow(), loc.GetCol()); }

void SenseBoardState::Set(int8_t row, int8_t col, bool val) { m_rows[row] ^= (-static_cast<uint8_t>(val) ^ m_rows[row]) & (1u << col); }
void SenseBoardState::Set(int8_t idx, bool value) { return Set(idx / 8, idx % 8, value); }
void SenseBoardState::Set(const ChessPieceLocation& loc, bool value) { return Set(loc.GetRow(), loc.GetCol(), value); }

void SenseBoardState::Toggle(int8_t row, int8_t col) { m_rows[row] ^= 1u << col; }
void SenseBoardState::Toggle(int8_t idx) { return Toggle(idx / 8, idx % 8); }
void SenseBoardState::Toggle(const ChessPieceLocation& loc) { return Toggle(loc.GetRow(), loc.GetCol()); }

Array<ChessPieceLocation> SenseBoardState::operator-(const SenseBoardState& other) const {
	Array<ChessPieceLocation> diff;
	for (int rowIdx = 0; rowIdx < 8; ++rowIdx) {
		if (uint8_t row = m_rows[rowIdx] ^ other.m_rows[rowIdx]) {
			for (int colIdx = 0; colIdx < 8; ++colIdx) {
				if ((row >> colIdx) & 1u)
					diff.push_back(ChessPieceLocation(rowIdx, colIdx));
			}
		}
	}
	return diff;
}

String SenseBoardState::ToString() const {
	String s;
	for (int i = 0; i < 64; ++i)
		s += Get(i) ? '1' : '0';
	return s;
}
