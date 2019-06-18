Screenshot Buffer
====

Library for taking screenshots of a specific window and returning a buffer (for Windows).

# Usage

```
npm i screenshot-buffer --save
```

# API

`capture(processName: string, options: ScreenshotBufferOptions)`

## ScreenshotBufferOptions

* `grayscale: boolean = false` - grayscale mode toggle

* `bringToFront: boolean = true` - bring the window to the front before taking a screenshot

* `quality: number = 100` - quality of the JPEG buffer

# Example

```
const screenshot = require('screenshot-buffer');
const { writeFileSync } = require('fs');

try {
	const bytes = screenshot.capture('notepad.exe', { bringToFront: true, grayscale: true, quality: 90 });
	writeFileSync('grayscale-notepad.jpg', Buffer.from(bytes));
} catch(e) {
	console.error(e);
}
```

# Notes

Only works on Windows.

# Contributions

... Are very welcome!