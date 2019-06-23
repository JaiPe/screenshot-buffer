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

```ts
import * as windowSnap from 'screenshot-buffer';
import { writeFileSync } from 'fs';

(async () => {
	try {
		const { width, height, data } = await windowSnap.capture('notepad.exe', { bringToFront: true, grayscale: true, quality: 90 });
		writeFileSync('grayscale-notepad.jpg', data);
	} catch(e) {
		console.error(e);
	}
})();
```

```js
const screenshot = require('screenshot-buffer');
const { writeFileSync } = require('fs');

(async () => {
	try {
		const bytes = await screenshot.capture('notepad.exe', { bringToFront: true, grayscale: true, quality: 90 });
		writeFileSync('grayscale-notepad.jpg', bytes);
	} catch(e) {
		console.error(e);
	}
})();
```

# Notes

Only works on Windows.

# Contributions

... Are very welcome!