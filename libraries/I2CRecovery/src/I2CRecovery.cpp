#include "I2CRecovery.h"

// Matches a TwoWire instance to the SDA/SCL pins it actually uses, by
// comparing object identity against the board's known global Wire
// instances. Returns false if this board doesn't have a pin mapping for it.
static bool pinsForWire(TwoWire &wire, uint8_t &sdaPin, uint8_t &sclPin) {
  if (&wire == &Wire) {
    sdaPin = SDA;
    sclPin = SCL;
    return true;
  }
#if defined(WIRE1_SDA_PIN) && defined(WIRE1_SCL_PIN)
  if (&wire == &Wire1) {
    sdaPin = WIRE1_SDA_PIN;
    sclPin = WIRE1_SCL_PIN;
    return true;
  }
#endif
  return false;
}

void i2cRecoverBus(TwoWire &wire) {
  uint8_t sdaPin, sclPin;
  if (!pinsForWire(wire, sdaPin, sclPin)) return; // unknown bus - nothing we can safely recover

  pinMode(sdaPin, INPUT_PULLUP);
  pinMode(sclPin, OUTPUT);

  for (int i = 0; i < 9; i++) {
    if (digitalRead(sdaPin)) break; // slave released the bus
    digitalWrite(sclPin, LOW);
    delayMicroseconds(5);
    digitalWrite(sclPin, HIGH);
    delayMicroseconds(5);
  }

  // manual STOP condition: SDA low->high while SCL is high
  pinMode(sdaPin, OUTPUT);
  digitalWrite(sdaPin, LOW);
  delayMicroseconds(5);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(sdaPin, HIGH);
  delayMicroseconds(5);
}
