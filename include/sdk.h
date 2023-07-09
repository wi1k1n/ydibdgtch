#ifndef SDK_H__
#define SDK_H__

#include "constants.h"
#include "Arduino.h"

#ifdef CUSTOM_UART
#include "CustomUART.h"
#define _UART_ uart
#else
#define _UART_ Serial
#endif

#ifdef _DEBUG_
#define __PRIVATE_LOG_PREAMBULE	   (_UART_.print(__FILE__)+\
									_UART_.print(F(":"))+\
									_UART_.print(__LINE__)+\
									_UART_.print(F(" "))+\
									_UART_.print(__PRETTY_FUNCTION__ )+\
									_UART_.print(F("() - ")))
#define DLOGLN(txt)		(__PRIVATE_LOG_PREAMBULE+_UART_.println(txt))
#define DLOGF(fmt, ...)	(__PRIVATE_LOG_PREAMBULE+_UART_.printf(fmt, __VA_ARGS__))
#define DLOG(txt)    	(__PRIVATE_LOG_PREAMBULE+_UART_.print(txt))
#define LOGLN(txt)		(_UART_.println(txt))
#define LOGF(fmt, ...)	(_UART_.printf(fmt, __VA_ARGS__))
#define LOG(txt)    	(_UART_.print(txt))
#else
#define DLOGLN(txt) do {} while(false)
#define DLOG(txt) do {} while(false)
#define LOGLN(txt) do {} while(false)
#define LOG(txt) do {} while(false)
#endif

// const __FlashStringHelper* operator""_f(const char* str) {
// 	return F(str);
// }
const __FlashStringHelper* operator""_f(const char* str, size_t len);

#endif // SDK_H__