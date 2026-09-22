var outputString = "";
let lineData = [];
var frameNum = 0;

// Runs (per frame) of the exact pixels that will light up on the OLED,
// kept around so the preview can loop through them after processing.
var frameRuns = [];

// Set to false for dark line art on a light/white background (e.g. a
// matplotlib/PIL sketch). Defaults to true for the common case: light
// line art / subject on a dark background, so that content is what gets
// lit up white on the OLED.
var invert = true;

// When true, the sampled crop is rotated 90° clockwise before being fit to
// the 128x64 OLED buffer -- lets landscape-shot source footage display
// correctly on an OLED that's physically mounted in portrait.
var rotate90 = false;

// Brightness cutoff (0-255) between "off" and "lit" pixels.
var threshold = 128;

// Pixels to grow (positive) or shrink (negative) the lit areas by, applied
// as that many dilate/erode passes before run-length encoding. 0 = no change.
var lineThickness = 0;

// Called once per GIF (or re-run) before processing its frames.
function resetProcessing() {
  outputString = "#include <Adafruit_GFX.h>\n#include <Adafruit_SSD1306.h>\n\nextern Adafruit_SSD1306 display;\n\n" + makeApplyRunsFunction();
  lineData = [];
  frameNum = 0;
  frameRuns = [];
}

// One dilate/erode pass over a flat width*height 0/1 grid, using a
// 4-neighbor structuring element. grow=true lights up any off pixel
// touching a lit neighbor; grow=false turns off any lit pixel touching
// an off neighbor (edges count as off, so shapes shrink at the border too).
function growOrShrinkGrid(grid, width, height, grow) {
  let out = new Uint8Array(width * height);

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      let idx = y * width + x;
      let up = y > 0 ? grid[idx - width] : 0;
      let down = y < height - 1 ? grid[idx + width] : 0;
      let left = x > 0 ? grid[idx - 1] : 0;
      let right = x < width - 1 ? grid[idx + 1] : 0;

      if (grow) {
        out[idx] = (grid[idx] || up || down || left || right) ? 1 : 0;
      } else {
        out[idx] = (grid[idx] && up && down && left && right) ? 1 : 0;
      }
    }
  }

  return out;
}

function processGifFrame() {

  // Static images (PNG/JPG) have no gifProperties/setFrame support -- only
  // animated GIFs need their frame advanced. frameNum is 0-based output
  // index; startFrame (sketch.js) offsets into the source GIF.
  if (gif.numFrames && gif.numFrames() > 1) {
    gif.setFrame(startFrame + frameNum);
  }

  // Draw the current crop rectangle of this frame into the 128x64 sample
  // buffer (cropBuffer + computeCropRectSourceSpace live in sketch.js),
  // then read pixels from that instead of the raw GIF.
  let { srcX, srcY, cropW, cropH } = computeCropRectSourceSpace();

  if (rotate90) {
    // Sample the crop into a portrait (height x width) rect centered on
    // the buffer's origin, then rotate that rect 90° clockwise so its
    // footprint fills the buffer's normal landscape (width x height) area.
    cropBuffer.push();
    cropBuffer.translate(width / 2, height / 2);
    cropBuffer.rotate(HALF_PI);
    cropBuffer.image(gif, -height / 2, -width / 2, height, width, srcX, srcY, cropW, cropH);
    cropBuffer.pop();
  } else {
    cropBuffer.image(gif, 0, 0, width, height, srcX, srcY, cropW, cropH);
  }
  cropBuffer.loadPixels();

  let litGrid = new Uint8Array(width * height);
  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      let index = (x + y * width) * 4; // RGBA pixel index
      let brightness = (cropBuffer.pixels[index] + cropBuffer.pixels[index + 1] + cropBuffer.pixels[index + 2]) / 3;
      let isLit = invert ? (brightness >= threshold) : (brightness < threshold);
      litGrid[y * width + x] = isLit ? 1 : 0;
    }
  }

  for (let i = 0; i < Math.abs(lineThickness); i++) {
    litGrid = growOrShrinkGrid(litGrid, width, height, lineThickness > 0);
  }

  // Draw exactly what will light up on the OLED, so the canvas doubles
  // as an accurate preview instead of showing the raw source GIF.
  background(0);
  noStroke();
  fill(255);

  for (let y = 0; y < height; y++) {
    let startX = -1;

    for (let x = 0; x < width; x++) {
      let isLit = litGrid[y * width + x] === 1;

      if (!isLit) {
        if (startX !== -1) {
          // End the current line of lit pixels
          let runLength = x - startX;
          lineData.push([y, startX, runLength]); // Store triplet [y, startX, length]
          rect(startX, y, runLength, 1);
          startX = -1;
        }
      } else {
        if (startX === -1) {
          // Start a new line of lit pixels
          startX = x;
        }
      }
    }

    // If a line ends at the last column
    if (startX !== -1) {
      let runLength = width - startX;
      lineData.push([y, startX, runLength]); // Store triplet [y, startX, length]
      rect(startX, y, runLength, 1);
    }
  }

  frameRuns.push(lineData.slice());

  // Export the data to a file
  let jsonOutput = formatForArduino(lineData);
  lineData = [];
  outputString = outputString + [jsonOutput];

  outputString += makeFrameFunction();
}

// totalFrames is the number of frames actually generated (frameNum count,
// i.e. endFrame - startFrame + 1), which may be fewer than gif.numFrames()
// when a start/end frame range is selected.
function makeFramesListArray(totalFrames){

  var s = "\nvoid (*functionArray[])() = {";
  for(var i = 0; i < totalFrames; i ++){
    s += "displayFrame"+i+",";
  }
  s+="};\n";
  outputString += s;

}

// Format the line data as a single array for Arduino
function formatForArduino(data) {
  let formatted = "\nconst uint8_t frame"+frameNum+"[] PROGMEM = {\n";

  for (let i = 0; i < data.length; i++) {
    let [y, startX, length] = data[i];
    formatted += `  ${y}, ${startX}, ${length},`;
  }

  formatted += "  255 // End of data marker\n};\n"; // Add a marker for the end of data
  return formatted;
}

// Emitted once at the top of the file instead of once per frame -- every
// displayFrameN() below just calls this with its own frame array, instead
// of each frame carrying its own copy of the decode loop.
function makeApplyRunsFunction() {
  return "void applyRuns(const uint8_t* data, uint16_t len) {\n" +
    " for (uint16_t i = 0; i + 2 < len; i += 3) {\n" +
    "uint8_t y = pgm_read_byte(&data[i]);\n" +
    "if (y == 255) {break;} // End of data marker\n" +
    "uint8_t startX = pgm_read_byte(&data[i + 1]);\n" +
    "uint8_t width = pgm_read_byte(&data[i + 2]);\n" +
    "display.drawFastHLine(startX, y, width, SSD1306_WHITE);\n" +
    "}\n" +
    "}\n";
}

function makeFrameFunction(){
  var s = "void displayFrame"+frameNum+"() {\ndisplay.clearDisplay();\napplyRuns(frame"+frameNum+", sizeof(frame"+frameNum+"));\ndisplay.display(); // Send the buffer to the OLED\n}\n";
  return s;
}

// Called once all frames have been processed. Returns the full Arduino
// source as a string instead of writing it to a file, so the caller can
// display it for copy/paste.
function finalizeOutput() {
  makeFramesListArray(frameNum);
  outputString += "\nint totalFrames = " + frameNum + ";";
  return outputString;
}