#pragma once

#include "chs_types.h"

class SenseBoardState {
public:
	bool Get(int8_t row, int8_t col) const;
	void Set(int8_t row, int8_t col, bool value);

private:
	Array<uint8_t> m_rows;
};