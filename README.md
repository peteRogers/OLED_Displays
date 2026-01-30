# OLED Displays

#### Wiring Labelling for OLED
![alt text](i2cGraphic.jpg "i2c graphics")


# Adafruit GFX Library Drawing Functions

The Adafruit GFX library provides versatile functions for drawing shapes and text on various graphical displays. Below are the descriptions and usage instructions for each drawing function.

---

## 1. Drawing Lines

### `drawLine(x0, y0, x1, y1, color)`
- **Description**: Draws a straight line from point `(x0, y0)` to `(x1, y1)`.
- **Parameters**:
  - `x0, y0`: Starting point of the line.
  - `x1, y1`: Ending point of the line.
  - `color`: The color of the line.
- **Example**:
```Arduino
  display.drawLine(0, 0, 100, 50, WHITE);
  ```

---

## 2. Drawing Rectangles

### `drawRect(x, y, w, h, color)`
- **Description**: Draws a rectangle outline with the top-left corner at `(x, y)`.
- **Parameters**:
  - `x, y`: Top-left corner coordinates.
  - `w, h`: Width and height of the rectangle.
  - `color`: The color of the rectangle outline.
- **Example**:
```Arduino
  display.drawRect(10, 10, 50, 30, BLUE);
  ```

### `fillRect(x, y, w, h, color)`
- **Description**: Draws a filled rectangle with the top-left corner at `(x, y)`.
- **Parameters**:
  - `x, y`: Top-left corner coordinates.
  - `w, h`: Width and height of the rectangle.
  - `color`: The color of the rectangle.
- **Example**:
```Arduino
  display.fillRect(20, 20, 40, 20, GREEN);
  ```

---

## 3. Drawing Circles

### `drawCircle(x, y, r, color)`
- **Description**: Draws a circle outline centered at `(x, y)` with radius `r`.
- **Parameters**:
  - `x, y`: Center of the circle.
  - `r`: Radius of the circle.
  - `color`: The color of the circle outline.
- **Example**:
```Arduino
  display.drawCircle(60, 60, 30, RED);
  ```

### `fillCircle(x, y, r, color)`
- **Description**: Draws a filled circle centered at `(x, y)` with radius `r`.
- **Parameters**:
  - Same as `drawCircle`.
- **Example**:
```Arduino
  display.fillCircle(80, 80, 20, YELLOW);
  ```

---

## 4. Drawing Rounded Rectangles

### `drawRoundRect(x, y, w, h, r, color)`
- **Description**: Draws a rectangle with rounded corners.
- **Parameters**:
  - `x, y`: Top-left corner of the rectangle.
  - `w, h`: Width and height of the rectangle.
  - `r`: Radius of the rounded corners.
  - `color`: The color of the rectangle outline.
- **Example**:
```Arduino
  display.drawRoundRect(30, 30, 60, 40, 10, MAGENTA);
  ```

### `fillRoundRect(x, y, w, h, r, color)`
- **Description**: Draws a filled rectangle with rounded corners.
- **Parameters**:
  - Same as `drawRoundRect`.
- **Example**:
```Arduino
  display.fillRoundRect(40, 40, 50, 30, 8, CYAN);
  ```

---

## 5. Drawing Triangles

### `drawTriangle(x0, y0, x1, y1, x2, y2, color)`
- **Description**: Draws a triangle outline using three vertices.
- **Parameters**:
  - `x0, y0`: First vertex.
  - `x1, y1`: Second vertex.
  - `x2, y2`: Third vertex.
  - `color`: The color of the triangle outline.
- **Example**:
```Arduino
  display.drawTriangle(20, 20, 50, 20, 35, 50, ORANGE);
  ```

### `fillTriangle(x0, y0, x1, y1, x2, y2, color)`
- **Description**: Draws a filled triangle using three vertices.
- **Parameters**:
  - Same as `drawTriangle`.
- **Example**:
```Arduino
  display.fillTriangle(25, 25, 60, 25, 45, 60, PURPLE);
  ```

---

## 6. Drawing Pixels

### `drawPixel(x, y, color)`
- **Description**: Draws a single pixel at `(x, y)`.
- **Parameters**:
  - `x, y`: Coordinates of the pixel.
  - `color`: The color of the pixel.
- **Example**:
```Arduino
  display.drawPixel(10, 10, WHITE);
  ```

---

## 7. Drawing Ellipses *(Approximation with Circles)*

The library does not directly support ellipses but you can simulate ellipses using successive `drawCircle` calls with varying radii.

---

## 8. Drawing Text

### `setCursor(x, y)`
- **Description**: Sets the cursor position for text rendering.
- **Parameters**:
  - `x, y`: Coordinates for the text cursor.
- **Example**:
```Arduino
  display.setCursor(10, 10);
  ```

### `setTextColor(color)`
- **Description**: Sets the text color. Optionally, you can provide a background color.
- **Parameters**:
  - `color`: Text color.
  - *(Optional)* `bg`: Background color.
- **Example**:
```Arduino
  display.setTextColor(RED, BLACK);
  ```

### `setTextSize(size)`
- **Description**: Sets the size of the text. Default is `1` (smallest size).
- **Parameters**:
  - `size`: Scale factor for the text.
- **Example**:
```Arduino
  display.setTextSize(2);
  ```

### `print("text")` / `println("text")`
- **Description**: Prints text on the screen. Use `println` for a new line after text.
- **Parameters**:
  - `"text"`: The text to display.
- **Example**:
```Arduino
  display.print("Hello, World!");
  ```

---

## 9. Custom Functions

### `drawBitmap(x, y, bitmap, w, h, color)`
- **Description**: Draws a monochrome bitmap starting at `(x, y)`.
- **Parameters**:
  - `x, y`: Top-left corner of the bitmap.
  - `bitmap`: Pointer to the bitmap data.
  - `w, h`: Width and height of the bitmap.
  - `color`: Foreground color of the bitmap.
- **Example**:
```Arduino
  display.drawBitmap(0, 0, myBitmap, 16, 16, WHITE);
  ```

---
#### Here are files supporting using OLED displays with Arduino.
OLED uses the adafruit GFX library for doing drawing commands. [GFX Library Documentation](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf)

Web link to convert images to show on screens [Make BITMAP IMAGES](https://javl.github.io/image2cpp/)

Get animated icons and emojis to show on OLED [WOKWI animator](https://animator.wokwi.com/)

List of u8g2 fonts [U8G2 fonts](https://github.com/olikraus/u8g2/wiki/fntlist8)
