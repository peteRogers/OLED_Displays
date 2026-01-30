var outputString = "#include \"displayManager.h\"\n";
let lineData = []; 
var frameNum = 0;

function processGifFrame() {
 
  gif.setFrame(frameNum); // Process only the first frame for now
  fill(0);
  rect(0, 0, width, height);
  image(gif, 0, 0, width, 64); // Render frame to canvas
  loadPixels(); // Load pixel data from the canvas

  for (let y = 0; y < height; y++) {
    let startX = -1;

    for (let x = 0; x < width; x++) {
      let index = (x + y * width) * 4; // RGBA pixel index
      let brightness = (pixels[index] + pixels[index + 1] + pixels[index + 2]) / 3;

      if (brightness < 128) { // Black pixel
        if (startX !== -1) {
          // End the current white line
          lineData.push([y, startX, x - startX]); // Store triplet [y, startX, length]
          startX = -1;
        }
      } else { // White pixel
        if (startX === -1) {
          // Start a new white line
          startX = x;
        }
      }
    }

    // If a line ends at the last column
    if (startX !== -1) {
      lineData.push([y, startX, width - startX]); // Store triplet [y, startX, length]
    }
  }

  // Export the data to a file
  let jsonOutput = formatForArduino(lineData);
  lineData = [];
  outputString = outputString + [jsonOutput];
 // saveStrings([jsonOutput], "horizontalLines.txt");

 // print("Processing complete. Data exported as 'horizontalLines.txt'.");
 // noLoop(); // Stop the draw loop
  outputString += makeFrameFunction();
}

function makeFramesListArray(){
  
  var s = "\nvoid (*functionArray[])() = {";
  for(var i = 0; i < gif.numFrames(); i ++){
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

function makeFrameFunction(){
  var s = "void displayFrame"+frameNum+"() {\ndisplay.clearDisplay();\n for (uint16_t i = 0; i < sizeof(frame"+frameNum+"); i += 3) {\nuint8_t y = pgm_read_byte(&frame"+frameNum+"[i]);\nif (y == 255) {break;} // End of data marker\nuint8_t startX = pgm_read_byte(&frame"+frameNum+"[i + 1]);\nuint8_t endX = pgm_read_byte(&frame"+frameNum+"[i + 2]);\ndisplay.drawFastHLine(startX, y, endX, SSD1306_WHITE);\n}\ndisplay.display(); // Send the buffer to the OLED\n}\n";
  return s;
}

function draw() {
  background(0);
  processGifFrame();
  frameNum++;
  if(gif.numFrames() == frameNum){
    makeFramesListArray();
    outputString += "\nint totalFrames = "+frameNum+";"
    saveStrings([outputString], "optimizedFrames"); // Save to a file
    console.log("Processing complete.");
    noLoop();
  }
}

