#ifndef CONSTANTS_H__
#define CONSTANTS_H__

#include "Arduino.h"

#include "secret.h"

#define _DEBUG_ 1
#define SERIAL_BAUDRATE 9600

#define PIN_REED_SWITCH 2
#define PIN_LEDS 13

#define PINS_INPUT_LIST { 2, 3 }
#define PINS_OUTPUT_LIST { 11, 12, 14/*A0*/ }

#define LEDMATRIX_CONSEQUENT_LEDS_COUNT 2


// WiFi
#define WIFI_AP_NAME		"DIY DGT Chessboard"
#define WIFI_AP_HOSTNAME	"diydgtchessboard"
#define WIFI_AP_IP			IPAddress(172, 20, 0, 1)
#define WIFI_AP_NETMASK 	IPAddress(255, 255, 255, 0)
#define WIFI_AP_DNS_PORT	53
#define WIFI_WEB_PORT		80

#endif // CONSTANTS_H__