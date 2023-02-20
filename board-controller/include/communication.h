#ifndef COMMUNICATION_H__
#define COMMUNICATION_H__

#include "constants.h"

#include <Arduino.h>

class Communication {
public:
	Communication() = default;

	bool begin();
	void tick();
};

#endif // COMMUNICATION_H__