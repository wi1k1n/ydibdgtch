#ifndef UTILITIES_H__
#define UTILITIES_H__

#include "Arduino.h"

template<std::size_t N, class T>
constexpr std::size_t countof(T(&)[N]) { return N; }

#endif // UTILITIES_H__