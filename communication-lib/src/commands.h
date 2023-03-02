#ifndef COMMANDS_H__
#define COMMANDS_H__

#include <Arduino.h>

enum class Commands : uint8_t {
	PING = 0,	// ping
	PONG,		// answer for ping
	SET,		// request to adjust some setting
	MSG,		// regular message with some payload
};

#endif // COMMANDS_H__