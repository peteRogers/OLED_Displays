# Using Fonts and Displaying Text with Adafruit GFX Library

The Adafruit GFX Library allows you to display text with customizable size, color, and fonts. Follow the steps below to understand how to configure and display text.

[Adafruit GFX Library Fonts Directory](https://github.com/adafruit/Adafruit-GFX-Library/tree/master/Fonts)

---

## 1. Displaying Text with Default Font

The default font is a 5x7 pixel bitmap font. To display text using this font:

### Setting the Cursor Position
Use `setCursor(x, y)` to position the text cursor.
- `x`: Horizontal position (in pixels) from the left edge of the display.
- `y`: Vertical position (in pixels) from the top edge of the display.

Example:
```Arduino
display.setCursor(0, 0); // Top-left corner of the display
```

### Printing Text
Use `print("text")` or `println("text")` to display text at the cursor position.
- `print`: Displays text without moving to a new line.
- `println`: Displays text and moves the cursor to the next line.

Example:
```Arduino
display.print("Hello, World!");
```

### Setting Text Color
Use `setTextColor(color)` to set the text color.
- `color`: The text color.
- Optionally, you can also specify a background color by adding a second parameter.

Example:
```Arduino
display.setTextColor(WHITE);
display.setTextColor(RED, BLACK); // Red text with a black background
```

### Setting Text Size
Use `setTextSize(size)` to scale the size of the text.
- `size`: A scaling factor (integer) to increase the text size. Default is 1.

Example:
```Arduino
display.setTextSize(2); // 2x scale
```

---

## 2. Using Custom Fonts

To use custom fonts, you need to include them in your project. The Adafruit GFX library provides several predefined fonts.

### Enabling Custom Fonts
1. Include the required font header file:
   - Font files are located in the Adafruit GFX library under the `Fonts` directory.
   - Example: `#include <Fonts/FreeSerif12pt7b.h>`

2. Set the font using `setFont(&fontName)`.
   - Replace `fontName` with the desired font name.

Example:
```Arduino
#include <Fonts/FreeSerif12pt7b.h>
display.setFont(&FreeSerif12pt7b);
```

### Switching Back to Default Font
Use `setFont()` with no arguments to switch back to the default font.

Example:
```Arduino
display.setFont(); // Switch to default font
```

### Supported Custom Fonts
Custom fonts provided by Adafruit include serif, sans-serif, bold, italic, and various sizes. Examples:
- FreeMono9pt7b
- FreeSans18pt7b
- FreeSerifBoldItalic24pt7b

---

## 3. Aligning and Wrapping Text

### Text Alignment
The library doesn’t provide direct text alignment options. You can calculate the position manually based on the display size and text width.

Example:
To center text, calculate the width of the text using `getTextBounds()`.

### Text Wrapping
Text wrapping is enabled by default. Long lines will wrap to the next line if they exceed the display width. You can disable this using `setTextWrap(false)`.

Example:
```Arduino
display.setTextWrap(false); // Disable wrapping
```

---

## 4. Measuring Text

Use `getTextBounds()` to calculate the dimensions of a string before drawing it.
- Syntax: `getTextBounds(str, x, y, &x1, &y1, &w, &h)`
- Returns the width (`w`) and height (`h`) of the string.

Example:
```Arduino
int16_t x1, y1;
uint16_t w, h;
display.getTextBounds("Hello", 0, 0, &x1, &y1, &w, &h);
// Now w and h contain the dimensions of the text.
```

---

