#pragma once

#ifdef TARGET_WIN
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdint>

class String {
private:
	std::string str;

public:
	// Constructors
	String() : str("") {}
	String(const char* s) : str(s ? s : "") {}
	String(const char* s, unsigned int length) : str(s, length) {}
	String(const String& s) : str(s.str) {}

	explicit String(char c) : str(1, c) {}
	explicit String(unsigned char value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(int value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(unsigned int value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(long value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(unsigned long value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(long long value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(unsigned long long value, unsigned char base = 10) { convertFromNumber(value, base); }
	explicit String(float value, unsigned int decimalPlaces = 2) { convertFromFloat(value, decimalPlaces); }
	explicit String(double value, unsigned int decimalPlaces = 2) { convertFromFloat(value, decimalPlaces); }

	~String() {}

	// Methods
	size_t length() const { return str.length(); }
	inline bool isEmpty(void) const { return length() == 0; }
	const char* c_str() const { return str.c_str(); }
	void concat(const String& s) { str += s.str; }

	// Convert string to number
	long toInt(void) const { return atol(str.c_str()); }
	float toFloat(void) const { return (float)atof(str.c_str()); }
	double toDouble(void) const { return atof(str.c_str()); }

	String operator+(const String& s) const { return String((str + s.str).c_str()); }
	String operator+(const char* s) const { return String((str + s).c_str()); }
	String operator+(char c) const { return String((str + c).c_str()); }
	String operator+(const std::string& s) const { return String((str + s).c_str()); }

	String operator+=(const String& s) { str += s.str; return *this; }
	String operator+=(const char* s) { str += s; return *this; }
	String operator+=(char c) { str += c; return *this; }
	String operator+=(const std::string& s) { str += s; return *this; }

	bool operator==(const String& s) const { return str == s.str; }
	bool operator!=(const String& s) const { return str != s.str; }

	char& String::operator[](unsigned int index) { return str[index]; }
	const char& String::operator[](unsigned int index) const { return str[index]; }

	String& operator=(const String& s) { str = s.str; return *this; }

	friend std::ostream& operator<<(std::ostream& os, const String& s) { os << s.str; return os; }

private:
	// Convert numbers to string based on base
	template <typename T>
	void convertFromNumber(T value, unsigned char base) {
		std::ostringstream oss;
		if (base == 10)
			oss << value;
		else if (base == 16)
			oss << std::hex << value;
		else if (base == 8)
			oss << std::oct << value;
		else
			oss << value;
		str = oss.str();
	}

	// Convert floating-point numbers
	void convertFromFloat(double value, unsigned int decimalPlaces) {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(decimalPlaces) << value;
		str = oss.str();
	}
};

#else

#include <Arduino.h>

#endif