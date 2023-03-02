#ifndef BOARDCONTROLLER_H__
#define BOARDCONTROLLER_H__

#include <Arduino.h>

class BoardController {
public:
	bool begin();
	void tick();
};

#endif // BOARDCONTROLLER_H__