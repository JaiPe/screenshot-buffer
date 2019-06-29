Screenshot Buffer
====

Library for taking screenshots of a specific window and returning a buffer (for Windows).

# Usage

```
npm i screenshot-buffer --save
```

# API

`capture(processName: string, options: ScreenshotOptions)`

## ScreenshotOptions

* `grayscale: boolean = false` - grayscale mode toggle

* `bringToFront: boolean = true` - bring the window to the front before taking a screenshot

* `mime: string = "image/png"` - mime type of the image (available from `MIME` enum export in Typescript)

# ImageData Example

```ts
import { capture } from 'screenshot-buffer';
import { writeFileSync } from 'fs';

(async () => {
    try {
        const { data, width, height } = await capture('notepad.exe', { bringToFront: true, grayscale: true });

        // Write file directly
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
        const { data, width, height } = await screenshot.capture('notepad.exe', { bringToFront: true, grayscale: true });

        // Write file directly
        writeFileSync('grayscale-notepad.png', data);
    } catch(e) {
        console.error(e);
    }
})();
```

# Notes

Only works on Windows.

# Contributions

... Are very welcome!