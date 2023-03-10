#ifndef COMMUNICATION_H__
#define COMMUNICATION_H__

#include "constants.h"

#include <Arduino.h>

class Communication {
public:
	Communication() = default;

	bool begin(uint8_t addr);
	void tick();
};

#endif // COMMUNICATION_H__