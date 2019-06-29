const screenshot = require('../');
const { writeFileSync } = require('fs');

(async () => {
    try {
        const { data, width, height } = await screenshot.capture('notepad.exe', { bringToFront: true, grayscale: true });

        writeFileSync('grayscale-notepad.png', data);
    } catch(e) {
        console.error(e);
    }
})();
