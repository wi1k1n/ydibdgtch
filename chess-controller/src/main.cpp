#include "constants.h"

#include <Arduino.h>
#include "wifi.h"
#include "boardcontroller.h"

BoardController board;
WiFiManager wifiManager;

void setup() {
  board.begin();
  wifiManager.begin();
}

void loop() {
  wifiManager.tick();
}