let gif; // To hold the GIF

function preload() {
  // Load the GIF (ensure it's 128x64 or will be resized)
  gif = loadImage("horse3.gif");
}

function setup() {
  createCanvas(128, 64);
  pixelDensity(1);
  gif.resize(128, 64); // Ensure the GIF is resized to 128x64
}






