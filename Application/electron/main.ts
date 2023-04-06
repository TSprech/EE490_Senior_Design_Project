// TSprech 2023/04/05 13:53:16
// Thanks to: https://davembush.medium.com/typescript-and-electron-the-right-way-141c2e15e4e1

const {BrowserWindow} = require('electron');

// import {ReadlineParser, SerialPort} from "serialport";
// All non-electron based requires should be placed below this comment after pnp.setup(), require('electron') should be above this
require('./.pnp.cjs').setup(); // Required for Yarn PnP (Plug N Play) functionality without changing CL args
const {SerialPort} = require('serialport');
// const {ReadlineParser} = require('@serialport/parser-readline');

// import {ReadlineParser, SerialPort} from "serialport";

class PortManager { // Based off: https://github.com/serialport/electron-serialport/blob/master/renderer.js
  // static port: typeof SerialPort;
  // static parser: typeof ReadlineParser;

  static async listSerialPorts() {
    // await SerialPort.list().then((ports, err) => {
    await SerialPort.list().then((ports: any) => {
      console.log('ports', ports);

      if (ports.length === 0) {
        console.log("No ports found")
      }
    })
  }
}

// PortManager.listSerialPorts().then(() => console.log("Done"))

function listPorts() {
  // listSerialPorts();
  PortManager.listSerialPorts().then(() => console.log("Done"))
  setTimeout(listPorts, 2000);
}

// Set a timeout that will check for new serialPorts every 2 seconds.
// This timeout reschedules itself.
setTimeout(listPorts, 2000);



export default class Main {
  static mainWindow: Electron.BrowserWindow;
  static application: Electron.App;
  static BrowserWindow: typeof Electron.CrossProcessExports.BrowserWindow;

  private static onWindowAllClosed() {
    if (process.platform !== 'darwin') {
      Main.application.quit();
    }
  }

  private static onClose() {
    // Dereference the window object.
    // @ts-ignore
    Main.mainWindow = null;
  }

  private static onReady() {
    Main.mainWindow = new Main.BrowserWindow({
      width: 800,
      height: 600,
      backgroundColor: "#ccc",
      webPreferences: {
        nodeIntegration: true, // to allow require
        contextIsolation: false // allow use with Electron 12+
      }
    });
    Main.mainWindow.loadFile('index.html');
    Main.mainWindow.on('closed', Main.onClose);
  }

  static main(app: Electron.App, browserWindow: typeof BrowserWindow
  ) {
    // we pass the Electron.App object and the
    // Electron.BrowserWindow into this function
    // so this class has no dependencies. This
    // makes the code easier to write tests for
    Main.BrowserWindow = browserWindow;
    Main.application = app;
    Main.application.on('window-all-closed', Main.onWindowAllClosed);
    Main.application.on('ready', Main.onReady);
  }
}
