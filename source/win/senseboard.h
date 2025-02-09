#pragma once

#include "chs_types.h"
#include "chs_string.h"

class SenseBoardState {
public:
	SenseBoardState() : m_rows(8) {}
	SenseBoardState(const SenseBoardState& other) : m_rows(other.m_rows) {}

	SenseBoardState(const String& s);

	bool Get(int8_t row, int8_t col) const;
	bool Get(int8_t idx) const;
	void Set(int8_t row, int8_t col, bool value);
	void Set(int8_t idx, bool value);

	String ToString() const;

private:
	Array<uint8_t> m_rows;
};