#include "sdk.h"

const __FlashStringHelper* operator""_f(const char* str, size_t len) {
	return F(str);
}