#ifndef WIFIINTERFACE_H__
#define WIFIINTERFACE_H__

#if 0 // temporary disabled because of emulation errors

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

	bool init();
	void tick();
};

#endif // if 0

#endif // WIFIINTERFACE_H__