#ifndef CONSTANTS_H__
#define CONSTANTS_H__

#include "secret.h"

#define SERIAL_BAUDRATE 9600

#define WIFI_AP_NAME		"DIY DGT Chessboard"
#define WIFI_AP_HOSTNAME	"diydgtchessboard"
#define WIFI_AP_IP			IPAddress(172, 20, 0, 1)
#define WIFI_AP_NETMASK 	IPAddress(255, 255, 255, 0)
#define WIFI_AP_DNS_PORT	53
#define WIFI_WEB_PORT		80

#endif // CONSTANTS_H__