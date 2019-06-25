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

* `mime: string = "image/jpeg"` - mime type of the image (available from `MIME` enum export in Typescript)

# Example

```ts
import { capture, MIME } from 'screenshot-buffer';
import { writeFileSync } from 'fs';
import { read } from 'jimp';

(async () => {
	try {
		const { width, height, data } = await capture('notepad.exe', { bringToFront: true, grayscale: true, mime: MIME.PNG });

        // Create Jimp instance from the buffer data
        const image = await read(data);

        // Write file directly
        writeFileSync('grayscale-notepad.jpg', bytes);
	} catch(e) {
		console.error(e);
	}
})();
```

```js
const screenshot = require('screenshot-buffer');
const { writeFileSync } = require('fs');
const { read } = require('jimp');

(async () => {
	try {
		const bytes = await screenshot.capture('notepad.exe', { bringToFront: true, grayscale: true, mime: 'image/png' });

        // Create Jimp instance from the buffer data
        const image = await read(data);

        // Write file directly
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