#ifndef I2C_RECOVERY_H
#define I2C_RECOVERY_H

#include <Arduino.h>
#include <Wire.h>

// Frees a locked I2C bus. Call this once in setup(), before Wire.begin()
// (or Wire1.begin(), if you're recovering a second I2C bus).
//
// Why you need this: if a sketch is reset/re-uploaded mid I2C-transaction,
// a slave device (like an SSD1306 OLED) can be left holding SDA low,
// waiting for clock pulses that never come. That hangs the bus until the
// slave is fully power-cycled. This function manually toggles SCL to clock
// the stuck slave through, then issues a STOP condition to put the bus
// back in a known idle state - no unplugging required.
//
// Pass whichever TwoWire object you're about to call .begin() on - it
// defaults to Wire. On boards with more than one I2C bus (e.g. Wire1 on
// the UNO R4 WiFi's Qwiic connector), this matches the object to its
// pins automatically, so you never need to look up pin numbers yourself.
void i2cRecoverBus(TwoWire &wire = Wire);

#endif
