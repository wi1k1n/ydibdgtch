#ifndef WIFIINTERFACE_H__
#define WIFIINTERFACE_H__

#include "constants.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <mdns.h>
#include <DNSServer.h>
#include <EEPROM.h>

class WiFiManager {
	static DNSServer _dnsServer;
	static WebServer _webServer;
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

#endif // WIFIINTERFACE_H__