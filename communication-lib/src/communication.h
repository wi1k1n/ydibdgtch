#ifndef COMMUNICATION_H__
#define COMMUNICATION_H__

#include <Arduino.h>
#include "commands.h"

class Communication {
public:
	static void SendCommand(Commands cmd, void* payload = nullptr);
};

#endif // COMMUNICATION_H__