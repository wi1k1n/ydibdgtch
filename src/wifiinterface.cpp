#if 0 // temporary disabled because of emulation errors
#include "wifiinterface.h"

DNSServer WiFiManager::_dnsServer;
WebServer WiFiManager::_webServer;
bool WiFiManager::_startedSuccessfully = false;

bool WiFiManager::_redirectToCaptivePortal() {
	// Redirect to captive portal if we got a request for another domain.
	// Return true in that case so the page handler do not try to handle the request again.
	IPAddress tempIp;
	if (!tempIp.fromString(_webServer.hostHeader()) && _webServer.hostHeader() != (String(WIFI_AP_HOSTNAME) + ".local")) {
		// Serial.println("Request redirected to captive portal");
		_webServer.sendHeader("Location", String("http://") + _webServer.client().localIP().toString(), true);
		_webServer.send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
		_webServer.client().stop(); // Stop is needed because we sent no content length
		return true;
	}
	return false;
}

bool WiFiManager::init() {
	// if (!WiFi.hostname(WIFI_AP_HOSTNAME)) { // Set the DHCP hostname assigned to ourselves
	// 	Serial.println("Couldn't set dhcp hostname");
	// 	return false;
	// }

	// Create Soft Access Point
	WiFi.disconnect();
	Serial.print("Soft AP ");
	if (!WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_NETMASK)) {
		Serial.println("Couldn't do Soft AP config!");
		return false;
	}
	if (!WiFi.softAP(WIFI_AP_HOSTNAME, WIFI_AP_PASSWORD)) {
		Serial.println("Couldn't start Soft AP!");
		return false;
	}
	Serial.println("OK");
	delay(600);
	
	/* Setup the DNS server redirecting all the domains to the apIP */
	_dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	_dnsServer.start(WIFI_AP_DNS_PORT, "*", WIFI_AP_IP);

	// Initialize HTTP Server
	_webServer.on("/", _handleWebRoot);
	_webServer.onNotFound(_handleWebNotFound);
	// Captive Portal options
	_webServer.on("/generate_204", _handleWebRoot);		// Android captive portal. Maybe not needed. Might be handled By notFound handler.
	_webServer.on("/favicon.ico", _handleWebRoot); 		// Another Android captive portal. Maybe not needed. Might be handled By notFound handler. Checked on Sony Handy
	_webServer.on("/fwlink", _handleWebRoot);  			// Microsoft captive portal. Maybe not needed. Might be handled By notFound handler.
	_webServer.begin(WIFI_WEB_PORT);

	_startedSuccessfully = true;
	return true;
}
void WiFiManager::tick() {
	if (!_startedSuccessfully)
		return;
    _dnsServer.processNextRequest(); // TODO: Should be guarded, in AP mode only!
	_webServer.handleClient();
	yield();
}

void WiFiManager::_sendWebServerHeaders(bool startSendingContent) {
	_webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	_webServer.sendHeader("Pragma", "no-cache");
	_webServer.sendHeader("Expires", "-1");
	_webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
	if (startSendingContent)
		_webServer.send(200, "text/html", "");
}
void WiFiManager::_sendWebPageHeaders(const String& title) {
	String content;
	content = "<!DOCTYPE HTML><html lang='en'><head><meta charset='UTF-8'><meta name= viewport content='width=device-width, initial-scale=1.0'>";
	_webServer.sendContent(content);
	content = "<style type='text/css'><!-- DIV.container { min-height: 10em; display: table-cell; vertical-align: middle }.button {height:35px; width:90px; font-size:16px}";
	_webServer.sendContent(content);
	content = "body {background-color: powderblue;}</style>";
	content += "<head><title>";
	content += title;
	content += "</title></head>";
	_webServer.sendContent(content);
}

void WiFiManager::_handleWebRoot() {
	_sendWebServerHeaders();
	_sendWebPageHeaders("Captive Portal");

	String content;
	content = "<body>";
	content += "<h2>Captive Portal</h2>";
	_webServer.sendContent(content);
	content = "<br><table border=2 bgcolor = white width = 500 cellpadding =5 ><caption><p><h3>Sytemlinks:</h2></p></caption>";
	content += "<tr><th><br>";
	content += "<a href=\"/en/wifi\">WiFi Settings</a><br><br>";
	content += "</th></tr></table><br><br>";
	content += "</body></html>";
	_webServer.sendContent(content);

	_webServer.client().stop();
}
void WiFiManager::_handleWebNotFound() {
	if (_redirectToCaptivePortal())
		return;
	
	_sendWebServerHeaders(false);
	_sendWebPageHeaders("404 Not Found");

	String content;
	content = "<body>";
	content += "<h2> 404 File Not Found</h2><br>";
	content += "<h4>Debug Information:</h4><br>";
	content += "URI: ";
	content += _webServer.uri();
	content += "\nMethod: ";
	content += (_webServer.method() == HTTP_GET) ? "GET" : "POST";
	content += "<br>Arguments: ";
	content += _webServer.args();
	content += "\n";
	for (uint8_t i = 0; i < _webServer.args(); i++)
		content += " " + _webServer.argName(i) + ": " + _webServer.arg(i) + "\n";
	content += "<br>Server Hostheader: " + _webServer.hostHeader();
	for (uint8_t i = 0; i < _webServer.headers(); i++)
		content += " " + _webServer.headerName(i) + ": " + _webServer.header(i) + "\n<br>";
	content += "</table></form><br><br><table border=2 bgcolor = white width = 500 cellpadding =5 ><caption><p><h2>You may want to browse to:</h2></p></caption>";
	content += "<tr><th>";
	content += "<a href=\"/en\">Main Page</a><br>";
	content += "<a href=\"/en/wifi\">WIFI Settings</a><br>";
	content += "</th></tr></table><br><br>";
	//content += "<footer><p>Programmed and designed By: Tobias Kuch</p><p>Contact information: <a href='mailto:tobias.kuch@googlemail.com'>tobias.kuch@googlemail.com</a>.</p></footer>";
	content += "</body></html>";
	_webServer.send(404, "", content);
	
	_webServer.client().stop();
}
#endif // if 0