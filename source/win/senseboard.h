#pragma once

#include "chs_types.h"
#include "chs_string.h"

#include "rulesengine.h"

class SenseBoardState {
public:
	SenseBoardState() : m_rows(8) {}
	SenseBoardState(const SenseBoardState& other) : m_rows(other.m_rows) {}

	SenseBoardState(const String& s);

	bool Get(int8_t row, int8_t col) const;
	bool Get(int8_t idx) const;
	bool Get(const ChessPieceLocation& loc) const;
	void Set(int8_t row, int8_t col, bool value);
	void Set(int8_t idx, bool value);
	void Set(const ChessPieceLocation& loc, bool value);
	void Toggle(int8_t row, int8_t col);
	void Toggle(int8_t idx);
	void Toggle(const ChessPieceLocation& loc);

	Array<ChessPieceLocation> operator-(const SenseBoardState& other) const;

	String ToString() const;

private:
	Array<uint8_t> m_rows;
};