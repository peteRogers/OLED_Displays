let gif; // To hold the currently loaded GIF (kept at its natural resolution)
let currentUrl; // Object URL for the loaded file, so we can revoke the old one
let mode = "idle"; // "idle" | "processing" | "previewing"
let previewFrame = 0;

let cropBuffer; // Offscreen 128x64 buffer used to sample the crop rectangle

// Inclusive range of source GIF frame indices to render. Defaults to the
// whole GIF; processGifFrame() (saveGifToArray.js) samples gif frame
// (startFrame + frameNum), so frameNum itself stays a simple 0-based
// sequential counter for naming/output purposes.
let startFrame = 0;
let endFrame = 0;

// Crop rectangle in the source GIF's own pixel coordinates (0,0 = top-left
// of the GIF). Free-form -- its aspect ratio need not match the OLED's
// 2:1, so mapping it onto the fixed 128x64 canvas can squash or stretch.
// Defaults to the full image (maximum zoomed-out view).
let cropX0 = 0, cropY0 = 0, cropX1 = 1, cropY1 = 1;

// "move" | "resize" | null. dragCorner is one of "tl" "tr" "bl" "br" when resizing.
let dragMode = null;
let dragCorner = null;
let dragStartX, dragStartY; // mouse position, in CSS px, at drag start
let dragStartRect; // {x0,y0,x1,y1} at drag start
const MIN_CROP_SIZE = 4; // pixels, in source space -- keeps the rect from collapsing

function setup() {
  let cnv = createCanvas(128, 64);
  cnv.parent("canvas-container");
  pixelDensity(1);
  noLoop();

  cropBuffer = createGraphics(128, 64);
  cropBuffer.pixelDensity(1);

  select("#gifInput").changed(handleFile);
  select("#invertCheckbox").changed(handleSettingChange);
  select("#rotateCheckbox").changed(handleRotateChange);
  select("#thresholdSlider").input(handleThresholdChange);
  select("#thicknessSlider").input(handleThicknessChange);
  select("#startFrameSlider").input(handleStartFrameChange);
  select("#endFrameSlider").input(handleEndFrameChange);
  select("#copyBtn").mousePressed(copyOutput);
  setupDragHandlers();

  window.addEventListener("resize", () => {
    if (!gif) return;
    updateSourceWrapperSize();
    updateCropOverlay();
  });

  invert = select("#invertCheckbox").elt.checked;
  rotate90 = select("#rotateCheckbox").elt.checked;
  threshold = select("#thresholdSlider").elt.valueAsNumber;
  lineThickness = select("#thicknessSlider").elt.valueAsNumber;
}

function handleFile() {
  let file = select("#gifInput").elt.files[0];
  if (!file) return;

  if (currentUrl) URL.revokeObjectURL(currentUrl);
  currentUrl = URL.createObjectURL(file);

  select("#status").html("Loading image...");
  select("#copyBtn").attribute("disabled", "");
  select("#output").elt.value = "";
  select("#sourceImg").elt.src = currentUrl;

  loadImage(
    currentUrl,
    (img) => {
      gif = img; // kept at natural resolution so the crop rect has room to work with

      cropX0 = 0;
      cropY0 = 0;
      cropX1 = gif.width;
      cropY1 = gif.height;

      // Static images (PNG/JPG) have no gifProperties, so numFrames()
      // returns undefined -- treat those as a single frame.
      let sourceFrames = gif.numFrames ? gif.numFrames() : undefined;
      let totalFrames = sourceFrames && sourceFrames > 0 ? sourceFrames : 1;

      startFrame = 0;
      endFrame = totalFrames - 1;
      let startSlider = select("#startFrameSlider").elt;
      let endSlider = select("#endFrameSlider").elt;
      startSlider.max = endFrame;
      endSlider.max = endFrame;
      startSlider.value = startFrame;
      endSlider.value = endFrame;
      select("#startFrameValue").html(startFrame);
      select("#endFrameValue").html(endFrame);

      updateSourceWrapperSize();
      updateCropOverlay();
      startProcessing();
    },
    () => {
      select("#status").html("Failed to load that image.");
    }
  );
}

// Sizes the source-preview box to the GIF's own aspect ratio, capped so
// tall/portrait GIFs don't blow up the page height. Pixel (not percentage)
// sizing keeps the <img> undistorted, which is what the crop-overlay math
// in updateCropOverlay() assumes.
function updateSourceWrapperSize() {
  if (!gif) return;
  let wrapper = select("#source-wrapper").elt;
  let maxW = wrapper.parentElement.getBoundingClientRect().width;
  let maxH = 400;
  let scale = Math.min(maxW / gif.width, maxH / gif.height);
  wrapper.style.width = gif.width * scale + "px";
  wrapper.style.height = gif.height * scale + "px";
}

function handleSettingChange() {
  invert = select("#invertCheckbox").elt.checked;
  if (gif) startProcessing(); // re-run with the new setting
}

function handleRotateChange() {
  rotate90 = select("#rotateCheckbox").elt.checked;
  if (gif) startProcessing(); // re-run with the new setting
}

function handleThresholdChange() {
  threshold = select("#thresholdSlider").elt.valueAsNumber;
  select("#thresholdValue").html(threshold);
  if (gif) startProcessing(); // re-run with the new setting
}

function handleThicknessChange() {
  lineThickness = select("#thicknessSlider").elt.valueAsNumber;
  select("#thicknessValue").html(lineThickness);
  if (gif) startProcessing(); // re-run with the new setting
}

function handleStartFrameChange() {
  startFrame = select("#startFrameSlider").elt.valueAsNumber;
  if (startFrame > endFrame) {
    endFrame = startFrame;
    select("#endFrameSlider").elt.value = endFrame;
    select("#endFrameValue").html(endFrame);
  }
  select("#startFrameValue").html(startFrame);
  if (gif) startProcessing(); // re-run with the new range
}

function handleEndFrameChange() {
  endFrame = select("#endFrameSlider").elt.valueAsNumber;
  if (endFrame < startFrame) {
    startFrame = endFrame;
    select("#startFrameSlider").elt.value = startFrame;
    select("#startFrameValue").html(startFrame);
  }
  select("#endFrameValue").html(endFrame);
  if (gif) startProcessing(); // re-run with the new range
}

// Crop rectangle (in the source GIF's own pixel coordinates) that gets
// scaled to fill the 128x64 OLED canvas -- shared with saveGifToArray.js.
// No aspect-ratio constraint, so a non-2:1 rect squashes or stretches.
function computeCropRectSourceSpace() {
  return {
    srcX: cropX0,
    srcY: cropY0,
    cropW: cropX1 - cropX0,
    cropH: cropY1 - cropY0,
    iw: gif.width,
    ih: gif.height,
  };
}

// Positions the blue crop-rectangle overlay (and, via CSS, its corner
// handles) on top of the (plain <img>) source preview using percentages,
// so it tracks the crop rect and stays correct at any display size.
function updateCropOverlay() {
  if (!gif) return;
  let iw = gif.width;
  let ih = gif.height;
  let rectEl = select("#cropRect").elt;
  rectEl.style.left = (cropX0 / iw) * 100 + "%";
  rectEl.style.top = (cropY0 / ih) * 100 + "%";
  rectEl.style.width = ((cropX1 - cropX0) / iw) * 100 + "%";
  rectEl.style.height = ((cropY1 - cropY0) / ih) * 100 + "%";
}

// Lets the user drag the crop rectangle's body (to move it) or one of its
// four corner handles (to resize/reshape it, changing its aspect ratio)
// directly on the source preview.
function setupDragHandlers() {
  let wrapper = select("#source-wrapper").elt;
  let rectEl = select("#cropRect").elt;

  rectEl.addEventListener("mousedown", (e) => {
    if (!gif) return;
    let corner = e.target.dataset.corner; // set only on handle elements
    dragMode = corner ? "resize" : "move";
    dragCorner = corner || null;
    dragStartX = e.clientX;
    dragStartY = e.clientY;
    dragStartRect = { x0: cropX0, y0: cropY0, x1: cropX1, y1: cropY1 };
    e.preventDefault();
    e.stopPropagation();
  });

  window.addEventListener("mousemove", (e) => {
    if (!dragMode || !gif) return;
    let bounds = wrapper.getBoundingClientRect();
    let iw = gif.width;
    let ih = gif.height;

    let dxSrc = ((e.clientX - dragStartX) / bounds.width) * iw;
    let dySrc = ((e.clientY - dragStartY) / bounds.height) * ih;

    if (dragMode === "move") {
      let w = dragStartRect.x1 - dragStartRect.x0;
      let h = dragStartRect.y1 - dragStartRect.y0;
      let x0 = constrain(dragStartRect.x0 + dxSrc, 0, iw - w);
      let y0 = constrain(dragStartRect.y0 + dySrc, 0, ih - h);
      cropX0 = x0;
      cropY0 = y0;
      cropX1 = x0 + w;
      cropY1 = y0 + h;
    } else {
      // Resize: only the dragged corner's edges move; the opposite edges
      // stay put. Clamp to the image bounds and a minimum size so the
      // rect can never invert or collapse to nothing.
      let { x0, y0, x1, y1 } = dragStartRect;
      if (dragCorner === "tl" || dragCorner === "bl") {
        x0 = constrain(dragStartRect.x0 + dxSrc, 0, dragStartRect.x1 - MIN_CROP_SIZE);
      }
      if (dragCorner === "tr" || dragCorner === "br") {
        x1 = constrain(dragStartRect.x1 + dxSrc, dragStartRect.x0 + MIN_CROP_SIZE, iw);
      }
      if (dragCorner === "tl" || dragCorner === "tr") {
        y0 = constrain(dragStartRect.y0 + dySrc, 0, dragStartRect.y1 - MIN_CROP_SIZE);
      }
      if (dragCorner === "bl" || dragCorner === "br") {
        y1 = constrain(dragStartRect.y1 + dySrc, dragStartRect.y0 + MIN_CROP_SIZE, ih);
      }
      cropX0 = x0;
      cropY0 = y0;
      cropX1 = x1;
      cropY1 = y1;
    }

    updateCropOverlay();
  });

  window.addEventListener("mouseup", () => {
    if (!dragMode) return;
    dragMode = null;
    dragCorner = null;
    if (gif) startProcessing();
  });
}

function startProcessing() {
  resetProcessing();
  mode = "processing";
  frameRate(60);
  let total = endFrame - startFrame + 1;
  select("#status").html("Processing frame 0 of " + total + "...");
  loop();
}

function draw() {
  if (mode === "processing") {
    processGifFrame(); // also draws this frame's processed (black/white) result
    frameNum++;

    let total = endFrame - startFrame + 1;
    if (frameNum < total) {
      select("#status").html("Processing frame " + frameNum + " of " + total + "...");
    } else {
      let code = finalizeOutput();
      select("#output").elt.value = code;
      select("#copyBtn").removeAttribute("disabled");
      select("#status").html("Done: " + frameNum + " frames processed. Previewing OLED output below.");
      mode = "previewing";
      previewFrame = 0;
      frameRate(12); // slower, watchable loop of the actual OLED output
    }
  } else if (mode === "previewing") {
    drawPreviewFrame(previewFrame);
    previewFrame = (previewFrame + 1) % frameRuns.length;
  }
}

// Redraws one already-processed frame from its stored runs, without
// re-reading the GIF -- used to loop the preview after generation is done.
function drawPreviewFrame(i) {
  background(0);
  noStroke();
  fill(255);
  for (let [y, startX, runLength] of frameRuns[i]) {
    rect(startX, y, runLength, 1);
  }
}

function copyOutput() {
  let output = select("#output").elt;
  output.select();
  navigator.clipboard.writeText(output.value).then(
    () => select("#status").html("Copied to clipboard."),
    () => document.execCommand("copy") // fallback for non-secure contexts
  );
}
