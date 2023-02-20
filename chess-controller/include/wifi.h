#ifndef WIFI_H__
#define WIFI_H__

#include "constants.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <EEPROM.h>

class WiFiManager {
	static DNSServer _dnsServer;
	static ESP8266WebServer _webServer;
	static bool _startedSuccessfully;

	static void _sendWebServerHeaders(bool startSendingContent = true);
	static void _sendWebPageHeaders(const String& title);

	static void _handleWebRoot();
	static void _handleWebNotFound();

	static bool _redirectToCaptivePortal();
public:
	WiFiManager() = default;

	bool begin();
	void tick();
};

#endif // WIFI_H__