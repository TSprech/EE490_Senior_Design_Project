// TSprech 2023/04/05 13:53:16

const {app, BrowserWindow} = require('electron');
// All non-electron based requires should be placed below this comment after pnp.setup(), require('electron') should be above this
require('./.pnp.cjs').setup(); // Required for Yarn PnP (Plug N Play) functionality without changing CL args
const {SerialPort} = require('serialport');

const createWindow = () => {
  const win = new BrowserWindow({
    width: 800,
    height: 600,
  })

  win.loadFile('index.html')
}

const port = new SerialPort({
  path: 'COM5',
  baudRate: 115200,
  autoOpen: false
});

app.whenReady().then(() => {
  createWindow();
})

// process.exit(0);
