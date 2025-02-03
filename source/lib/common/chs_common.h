#pragma once

#ifdef TARGET_WIN
#include <iostream>
#define __LOGVAL(x) std::cout << x
#define __LOGVALLN(x) std::cout << x << std::endl
#else
#include <Arduino.h>
#define __LOGVAL(x) Serial.print(x)
#define __LOGVALLN(x) Serial.println(x)
#endif

// TODO: this is temporarily the same, change it to have more debug info
#define LOG DLOG
#define LOGLN DLOGLN

template<typename T>
void DLOGLN(T v) {
	__LOGVALLN(v);
}
template <typename T, typename... Ts>
void DLOGLN(T v, Ts... ts) {
	__LOGVAL(v);
	DLOGLN(ts...);
	__LOGVALLN("");
}
void DLOGLN() {
	__LOGVALLN("");
}

template<typename T>
void DLOG(T v) {
	__LOGVAL(v);
}
template <typename T, typename... Ts>
void DLOG(T v, Ts... ts) {
	__LOGVAL(v);
	DLOG(ts...);
	__LOGVAL("");
}
void DLOG() {
	__LOGVAL("");
}