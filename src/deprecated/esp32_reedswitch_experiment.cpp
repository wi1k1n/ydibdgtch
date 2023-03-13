#if 0
// The Proof of Concept: A single reed switch
// inputs/rows
#define PRFCNPT_PIN_DIODE_1       GPIO_NUM_14
#define PRFCNPT_PIN_DIODE_2       GPIO_NUM_27
// outputs/cols
#define PRFCNPT_PIN_REEDSWITCH_1  GPIO_NUM_16
#define PRFCNPT_PIN_REEDSWITCH_2  GPIO_NUM_17

#include <Arduino.h>

int lastState = HIGH; // the previous state from the input pin
int currentState;     // the current reading from the input pin

void setup() {
  Serial.begin(115200);
  pinMode(PRFCNPT_PIN_REEDSWITCH_1, OUTPUT);
  pinMode(PRFCNPT_PIN_REEDSWITCH_2, OUTPUT);
  pinMode(PRFCNPT_PIN_DIODE_1, INPUT_PULLDOWN);
  pinMode(PRFCNPT_PIN_DIODE_2, INPUT_PULLDOWN);
}

bool readCell(int row, int col) {
  digitalWrite(PRFCNPT_PIN_REEDSWITCH_1, col == 0 ? HIGH : LOW);
  digitalWrite(PRFCNPT_PIN_REEDSWITCH_2, col == 1 ? HIGH : LOW);
  bool v = digitalRead(row == 0 ? PRFCNPT_PIN_DIODE_1 : PRFCNPT_PIN_DIODE_2);
  digitalWrite(PRFCNPT_PIN_REEDSWITCH_1, LOW);
  digitalWrite(PRFCNPT_PIN_REEDSWITCH_2, LOW);
  return v;
}

void loop() {

  // return;
  Serial.print(readCell(0, 0));
  Serial.print(readCell(0, 1));
  Serial.print(readCell(1, 0));
  Serial.println(readCell(1, 1));
  delay(500);
}
#endif