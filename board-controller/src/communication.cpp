#include "communication.h"
#include "Wire.h"

bool Communication::begin(uint8_t addr) {
	Wire.begin(addr);
	return true;
}

void Communication::tick() {
	
}