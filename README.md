# OLED Displays

A collection of Arduino sketches for driving small monochrome OLED screens
(SSD1306, 128x64 and similar) - a good place to start if you've never talked
to one of these before.

## Getting started

Need the [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
and [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library) libraries
installed via the Arduino Library Manager.

## How a sketch is put together

Every sketch here follows the same shape:

```cpp
void setup() {
  display.begin(...);   // wake the screen up, once
}

void loop() {
  display.clearDisplay();                 // 1. wipe the screen's memory (not visible yet)
  display.drawCircle(64, 32, 20, WHITE);   // 2. draw whatever you want
  display.display();                      // 3. push it all to the actual screen
}
```

Nothing you draw actually appears until you call `display.display()` -
everything before that just builds up a picture in memory. That's normal: it
lets you draw a whole frame (background, shapes, text) before it's shown, so
the screen never flashes a half-drawn frame.

### The coordinate system

`(0, 0)` is the **top-left** corner. `x` increases to the right, `y`
increases downward. A 128x64 display is 128 pixels wide and 64 tall, so its
bottom-right pixel is `(127, 63)`.

### Colour

These are monochrome displays - every pixel is simply on or off. There's no
red/green/blue here, only:

- `WHITE` - turn the pixel on
- `BLACK` - turn the pixel off (handy for erasing)
- `INVERSE` - flip whatever's already there

If you spot example code elsewhere using colours like `RED` or `BLUE`, that's
written for a different, full-colour display - it won't compile against this
library.

## Drawing functions (Adafruit_GFX)

All of these are called on your `display` object, e.g. `display.drawPixel(...)`.

### Shapes

| Function | Draws | Example |
|---|---|---|
| `drawPixel(x, y, color)` | A single pixel | `display.drawPixel(10, 10, WHITE);` |
| `drawLine(x0, y0, x1, y1, color)` | A line between two points | `display.drawLine(0, 0, 100, 50, WHITE);` |
| `drawRect(x, y, w, h, color)` | Rectangle outline | `display.drawRect(10, 10, 50, 30, WHITE);` |
| `fillRect(x, y, w, h, color)` | Filled rectangle | `display.fillRect(20, 20, 40, 20, WHITE);` |
| `drawRoundRect(x, y, w, h, r, color)` | Rectangle with rounded corners of radius `r` | `display.drawRoundRect(30, 30, 60, 40, 10, WHITE);` |
| `fillRoundRect(x, y, w, h, r, color)` | Filled version of the above | `display.fillRoundRect(40, 40, 50, 30, 8, WHITE);` |
| `drawCircle(x, y, r, color)` | Circle outline, centred at `(x, y)` | `display.drawCircle(60, 32, 20, WHITE);` |
| `fillCircle(x, y, r, color)` | Filled circle | `display.fillCircle(80, 32, 15, WHITE);` |
| `drawTriangle(x0,y0, x1,y1, x2,y2, color)` | Triangle outline from 3 points | `display.drawTriangle(20,20, 50,20, 35,50, WHITE);` |
| `fillTriangle(x0,y0, x1,y1, x2,y2, color)` | Filled triangle | `display.fillTriangle(25,25, 60,25, 45,60, WHITE);` |

There's no built-in ellipse - fake one with a squashed circle, or layer a few
`drawCircle()` calls of different radii.

### Text

| Function | Does | Example |
|---|---|---|
| `setCursor(x, y)` | Moves the text cursor - call this before printing | `display.setCursor(10, 10);` |
| `setTextSize(size)` | Scales text up; `1` is the smallest (~6x8px per character) | `display.setTextSize(2);` |
| `setTextColor(color)` / `setTextColor(color, bg)` | Sets the text colour, optionally with a background fill | `display.setTextColor(WHITE, BLACK);` |
| `setTextWrap(bool)` | Whether long text wraps onto the next line | `display.setTextWrap(true);` |
| `print("text")` / `println("text")` | Draws text at the cursor; `println` also moves to the next line | `display.print("Hello!");` |

Text uses the library's built-in 5x7 pixel font unless you load a custom one
- see the [u8g2 font list](https://github.com/olikraus/u8g2/wiki/fntlist8)
below for more variety.

### Bitmaps

| Function | Draws | Example |
|---|---|---|
| `drawBitmap(x, y, bitmap, w, h, color)` | A monochrome image from a `PROGMEM` byte array | `display.drawBitmap(0, 0, myBitmap, 16, 16, WHITE);` |

Turn any image into a bitmap array with [image2cpp](https://javl.github.io/image2cpp/),
or grab ready-made animated icons from the [WOKWI animator](https://animator.wokwi.com/).

## Reference & tools

- [Adafruit GFX library documentation (PDF)](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf) - the full API this repo is built on.
- [image2cpp](https://javl.github.io/image2cpp/) - convert an image into a bitmap array for `drawBitmap()`.
- [WOKWI animator](https://animator.wokwi.com/) - ready-made animated icons/emoji for OLEDs.
- [u8g2 font list](https://github.com/olikraus/u8g2/wiki/fntlist8) - alternative fonts you can use instead of the built-in one.

## Once you've got the basics

- [`VARIOUS_EXAMPLES/`](VARIOUS_EXAMPLES) - small, focused demos (bouncing balls, a sine wave, a distance sensor readout, QR codes, and more).
- [`SHOWING_IMAGES/`](SHOWING_IMAGES) - animated GIFs and image playback.
- [`TYPOGRAPHY/`](TYPOGRAPHY) - working with text and fonts in more depth.

---

## Troubleshooting: blank screen on Arduino UNO R4 (SPI)

If an SSD1306 wired for SPI (hardware or software) shows nothing on an
UNO R4 (WiFi or Minima) but works fine on other boards, this is a known
`Adafruit_SSD1306` library bug, not your wiring.

The library has a `HAVE_PORTREG` fast path that pokes hardware registers
directly instead of using `digitalWrite()` for the DC/CS/CLK/MOSI pins.
It's enabled for any ARM board (`__arm__`), which includes the R4's
Renesas core - but the direct register writes don't work correctly there,
so those pins silently never toggle. `display.begin()` still returns
`true` because SPI has no way to detect a non-responding device, so it
fails silently.

**Fix:** in your installed copy of `Adafruit_SSD1306.h` (typically
`~/Documents/Arduino/libraries/Adafruit_SSD1306/Adafruit_SSD1306.h`),
find the `#elif (defined(__arm__) ...)` block guarding `HAVE_PORTREG`
and add `&& !defined(ARDUINO_ARCH_RENESAS)` to its condition, so it reads:

```cpp
#elif (defined(__arm__) || defined(ARDUINO_FEATHER52)) &&                      \
    !defined(ARDUINO_ARCH_MBED) && !defined(ARDUINO_ARCH_RP2040) &&            \
    !defined(ARDUINO_ARCH_RENESAS)
```

This is a global library file, not part of this repo, so the fix won't
survive a library reinstall/update - if the blank-screen issue comes back
after updating the library, re-apply it. See the original report on the
[Arduino Forum](https://forum.arduino.cc/t/adafruit-oleds-wont-work-on-r4-ok-on-r3/1180549).
