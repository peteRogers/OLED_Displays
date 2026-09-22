// A typewriter bound to the OLED's exact character grid: 128x64 pixels,
// Adafruit_GFX's default font at 6px/char x 8px/line = 21 columns x 8 rows.
// Typing is purely sequential (no arrow-key cursor movement), so each row
// is just a string that grows/shrinks as you type/backspace - there's no
// separate cursor column to track, it's always the current row's length.
// Every edit sends the whole grid to the board, '\r' between rows, so what
// you see here is what appears on the physical screen.

const OLED_WIDTH = 128;
const OLED_HEIGHT = 64;
const CHAR_WIDTH = 6;
const CHAR_HEIGHT = 8;
// Math.floor(), not p5's floor() - this runs before p5 attaches its globals
// to window (that happens on window load, after this script's top level).
const COLS = Math.floor(OLED_WIDTH / CHAR_WIDTH);   // 21
const ROWS = Math.floor(OLED_HEIGHT / CHAR_HEIGHT); // 8
const SCALE = 4; // preview is drawn 4x actual size

let grid = makeBlankGrid();
let cursorRow = 0;

const arduino = new ArduinoSerial({ baudRate: 115200 });
arduino.onStatusChange = (status) => {
  if (status === "connected") sendGrid(); // sync the board to whatever's on screen already
};

document.getElementById("clearBtn").addEventListener("click", () => {
  grid = makeBlankGrid();
  cursorRow = 0;
  if (arduino.isConnected) sendGrid();
});

window.addEventListener("keydown", handleKeyDown);

function makeBlankGrid() {
  return new Array(ROWS).fill("");
}

function setup() {
  const canvas = createCanvas(OLED_WIDTH * SCALE, OLED_HEIGHT * SCALE);
  canvas.parent("canvas-container");
  noSmooth();
  textFont("monospace");
  textSize(CHAR_HEIGHT * SCALE * 0.8);
  textAlign(LEFT, TOP);
}

function draw() {
  background(0);

  fill(255);
  noStroke();
  // Draw one character at a time, each pinned to a fixed CHAR_WIDTH*SCALE
  // cell, instead of text(wholeLine, ...) - the browser's "monospace" font
  // doesn't actually advance at exactly that width, so letting it lay out
  // the whole string drifted further out of sync with the cursor (and with
  // the real board's fixed 6px/char grid) the longer a line got.
  for (let row = 0; row < ROWS; row++) {
    const line = grid[row];
    for (let col = 0; col < line.length; col++) {
      text(line[col], col * CHAR_WIDTH * SCALE, row * CHAR_HEIGHT * SCALE);
    }
  }

  // blinking block cursor at the next character position
  if (floor(millis() / 500) % 2 === 0) {
    const cx = grid[cursorRow].length * CHAR_WIDTH * SCALE;
    const cy = cursorRow * CHAR_HEIGHT * SCALE;
    rect(cx, cy, CHAR_WIDTH * SCALE, CHAR_HEIGHT * SCALE);
  }
}

function handleKeyDown(e) {
  if (e.key === "Enter") {
    e.preventDefault();
    newLine();
  } else if (e.key === "Backspace") {
    e.preventDefault();
    backspace();
  } else if (e.key.length === 1 && !e.ctrlKey && !e.metaKey && !e.altKey) {
    e.preventDefault();
    typeChar(e.key);
  } else {
    return; // leave other keys (tab, browser shortcuts, ...) alone
  }

  if (arduino.isConnected) sendGrid();
}

function typeChar(ch) {
  if (grid[cursorRow].length >= COLS) {
    advanceRow();
  }
  grid[cursorRow] += ch;
}

function newLine() {
  advanceRow();
}

// Moves to the next row; once the last row is full, scrolls everything up
// by one row, like a terminal, instead of refusing further input.
function advanceRow() {
  if (cursorRow < ROWS - 1) {
    cursorRow++;
  } else {
    grid.shift();
    grid.push("");
  }
}

function backspace() {
  if (grid[cursorRow].length > 0) {
    grid[cursorRow] = grid[cursorRow].slice(0, -1);
  } else if (cursorRow > 0) {
    cursorRow--;
  }
}

async function sendGrid() {
  await arduino.send(grid.join("\r") + "\n"); // ArduinoSerial.send() doesn't add \n itself
}
