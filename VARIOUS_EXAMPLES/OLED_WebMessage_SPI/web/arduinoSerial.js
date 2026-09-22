/**
 * ArduinoSerial - a small wrapper around the Web Serial API.
 *
 * Usage:
 *   const arduino = new ArduinoSerial({ baudRate: 115200 });
 *   arduino.onLine = (line) => { ... };
 *
 * The library manages its own "Connect to Arduino" button (shown only while
 * disconnected) and auto-reconnects to a previously authorized device on
 * page load, so nothing else needs to be wired up in the sketch.
 */
class ArduinoSerial {
  constructor({ baudRate = 115200, autoConnect = true, buttonLabel = "Connect to Arduino" } = {}) {
    this.baudRate = baudRate;
    this.port = null;
    this.reader = null;
    this.writer = null;
    this._buffer = "";
    this._status = "disconnected";

    this.onLine = null; // (line: string) => void
    this.onStatusChange = null; // (status: string) => void, optional
    this.onError = null; // (error: Error) => void, optional - defaults to console.error

    this._button = this._createConnectButton(buttonLabel);

    if (typeof navigator !== "undefined" && navigator.serial) {
      navigator.serial.addEventListener("disconnect", (event) => {
        if (event.target === this.port) this._handleDisconnect();
      });
    }

    if (autoConnect) this._tryAutoConnect();
  }

  get status() {
    return this._status;
  }

  get isConnected() {
    return this._status === "connected";
  }

  /** Prompt the user to pick a port. Must be called from a user gesture (e.g. a click). */
  async connect() {
    if (!navigator.serial) {
      this._fail(new Error("Web Serial API not supported in this browser."));
      return;
    }
    try {
      const port = await navigator.serial.requestPort();
      await this._openPort(port);
    } catch (err) {
      // User cancelled the picker, or the open failed.
      this._fail(err);
    }
  }

  async disconnect() {
    await this._teardown();
    this._setStatus("disconnected");
  }

  /** Write a string to the board. No-op if not connected. */
  async send(text) {
    if (!this.writer) return;
    try {
      await this.writer.write(text);
    } catch (err) {
      this._fail(err);
    }
  }

  _createConnectButton(label) {
    const button = document.createElement("button");
    button.textContent = label;
    button.style.position = "absolute";
    button.style.top = "10px";
    button.style.left = "10px";
    button.style.zIndex = 1000;
    button.addEventListener("click", () => this.connect());
    document.body.appendChild(button);
    return button;
  }

  async _tryAutoConnect() {
    if (!navigator.serial) return;
    try {
      const ports = await navigator.serial.getPorts();
      if (ports.length > 0) await this._openPort(ports[0]);
    } catch (err) {
      this._fail(err);
    }
  }

  async _openPort(port) {
    this._setStatus("connecting");
    try {
      await port.open({ baudRate: this.baudRate });
    } catch (err) {
      this._setStatus("disconnected");
      this._fail(err);
      return;
    }
    this.port = port;

    const textDecoder = new TextDecoderStream();
    // Kept and awaited in _teardown() - port.close() throws if port.readable
    // is still locked, which it is until this pipe actually unwinds.
    this._readableClosed = port.readable.pipeTo(textDecoder.writable).catch(() => {});
    this.reader = textDecoder.readable.getReader();

    if (port.writable) {
      const textEncoder = new TextEncoderStream();
      this._writableClosed = textEncoder.readable.pipeTo(port.writable).catch(() => {});
      this.writer = textEncoder.writable.getWriter();
    }

    this._setStatus("connected");
    this._readLoop();
  }

  async _readLoop() {
    try {
      while (this.reader) {
        const { value, done } = await this.reader.read();
        if (done) break;
        this._buffer += value;
        const lines = this._buffer.split("\n");
        this._buffer = lines.pop();
        for (const line of lines) {
          if (this.onLine) this.onLine(line.trim());
        }
      }
    } catch (err) {
      this._fail(err);
    }
  }

  async _handleDisconnect() {
    await this._teardown();
    this._setStatus("disconnected");
  }

  async _teardown() {
    if (this.reader) {
      try {
        await this.reader.cancel();
      } catch (_) {}
      this.reader = null;
    }
    if (this.writer) {
      try {
        await this.writer.close();
      } catch (_) {}
      this.writer = null;
    }
    // Wait for the pipeTo() chains from _openPort() to actually finish
    // unwinding - port.close() below throws (silently, into the catch)
    // if port.readable/port.writable are still locked, which leaves the
    // OS-level port handle open and the next connect() failing to reopen it.
    if (this._readableClosed) {
      await this._readableClosed;
      this._readableClosed = null;
    }
    if (this._writableClosed) {
      await this._writableClosed;
      this._writableClosed = null;
    }
    if (this.port) {
      try {
        await this.port.close();
      } catch (_) {}
      this.port = null;
    }
  }

  _setStatus(status) {
    this._status = status;
    this._button.style.display = status === "connected" ? "none" : "";
    if (this.onStatusChange) this.onStatusChange(status);
  }

  _fail(err) {
    if (this.onError) this.onError(err);
    else console.error(err);
  }
}
