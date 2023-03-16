#ifndef CONSTANTS_H__
#define CONSTANTS_H__

#include "Arduino.h"

#include "secret.h"

#define _DEBUG_ 1
#define _CHRULESDBG_ 1
#define SERIAL_BAUDRATE 115200

#define PIN_LEDS 			GPIO_NUM_4
#define PIN_PUSHBUTTON1		GPIO_NUM_21

#define PINS_INPUT_LIST { \
			GPIO_NUM_14, \
			GPIO_NUM_27, \
			GPIO_NUM_26, \
			GPIO_NUM_25, \
			GPIO_NUM_13, \
			GPIO_NUM_12, \
			GPIO_NUM_33, \
			GPIO_NUM_32 \
		} // esp32 -> diodes
#define PINS_OUTPUT_LIST { \
			GPIO_NUM_16, \
			GPIO_NUM_17 \
		} // esp32 -> reed switches
#define ESP_WIRING // undef this if Arduino wiring is used

#define LEDMATRIX_CONSEQUENT_LEDS_COUNT 2


// WiFi
#define WIFI_AP_NAME		"DIY DGT Chessboard"
#define WIFI_AP_HOSTNAME	"diydgtchessboard"
#define WIFI_AP_IP			IPAddress(172, 20, 0, 1)
#define WIFI_AP_NETMASK 	IPAddress(255, 255, 255, 0)
#define WIFI_AP_DNS_PORT	53
#define WIFI_WEB_PORT		80

#endif // CONSTANTS_H__