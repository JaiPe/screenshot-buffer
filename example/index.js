const screenshot = require('../');
const { writeFileSync } = require('fs');

try {
	const bytes = screenshot.capture('notepad.exe', { bringToFront: true, grayscale: false, quality: 90 });
	writeFileSync('grayscale-notepad.jpg', Buffer.from(bytes));
} catch(e) {
	console.error(e);
}
