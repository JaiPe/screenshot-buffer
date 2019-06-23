const screenshot = require('../');
const { writeFileSync } = require('fs');

(async () => {
	try {
		const bytes = await screenshot.capture('notepad.exe', { bringToFront: true, grayscale: true, quality: 90 });
		writeFileSync('grayscale-notepad.jpg', bytes);
	} catch(e) {
		console.error(e);
	}
})();
