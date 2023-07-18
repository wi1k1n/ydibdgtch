#ifndef EMULATE_CONFIG_H__
#define EMULATE_CONFIG_H__

#include <inttypes.h>

#include <gpio.h>

#define _EMULATE_CONFIG_

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

unsigned long micros();
unsigned long millis();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);

void setup();
void loop();
int main() {
	setup();
	while (true) {
		loop();
	}
	return 0;
}

#endif // EMULATE_CONFIG_H__