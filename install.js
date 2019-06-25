const os = require('os');
var spawn = require('cross-spawn');

if (os.platform() === 'win32') {
    spawn.sync('npm', ['run', 'rebuild'], {
        stdio: 'inherit'
    });
} else {
    throw new Error('`screenshot-buffer` module is Windows only.');
}
