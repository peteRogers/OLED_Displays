# I2CRecovery

A tiny Arduino library that fixes a common, confusing problem:

> Your I2C device (like an SSD1306 OLED) works fine... until you upload new
> code. Then it stops responding, and the only fix is to unplug the board
> and plug it back in.

## Why this happens

When you upload a new sketch, the board resets — but the I2C *device*
(the OLED) usually doesn't lose power. If the old sketch was reset in the
middle of talking to it, the device can be left waiting for more clock
pulses that never arrive, holding the data line (SDA) low forever. That
"locks up" the bus. A normal reset doesn't fix this, because it only
resets your board's side of the conversation, not the device's.

`i2cRecoverBus()` fixes this by manually pulsing the clock line (SCL) up
to 9 times to free the stuck device, then sending a proper STOP signal so
the bus starts clean — no unplugging required.

## Install

1. Clone or download this repo.
2. Copy the whole `I2CRecovery` folder into your Arduino libraries folder:
   - macOS/Linux: `~/Documents/Arduino/libraries/`
   - Windows: `Documents\Arduino\libraries\`
3. Restart the Arduino IDE.

(If you're working from a clone of this repo on the same computer it was
built on, this is already set up as a symlink — no copying needed.)

## Usage

```cpp
#include <Wire.h>
#include <I2CRecovery.h>

void setup() {
  i2cRecoverBus(); // call this FIRST, before Wire.begin() - defaults to Wire
  Wire.begin();
  Wire.setClock(400000); // optional: speed up I2C (400kHz is standard "Fast Mode")

  // ... your Wire/display.begin() code here
}
```

That's it — one function call, right at the top of `setup()`, before
anything else touches the I2C bus.

### Recovering a second I2C bus (e.g. Wire1)

Some boards have more than one I2C bus - for example the UNO R4 WiFi,
where the Qwiic/STEMMA QT connector is wired to `Wire1`, not `Wire`. Pass
the `TwoWire` object you're about to `.begin()` and it'll recover the
matching pins automatically:

```cpp
void setup() {
  i2cRecoverBus(Wire1); // recovers the Qwiic bus specifically
  Wire1.begin();
  // ...
}
```

## See it in action

- [`OLED_STARTER/OLED_STARTER.ino`](../../OLED_STARTER/OLED_STARTER.ino)
- [`SHOWING_IMAGES/arduinoAnimatedGif/arduinoAnimatedGif.ino`](../../SHOWING_IMAGES/arduinoAnimatedGif/arduinoAnimatedGif.ino)
