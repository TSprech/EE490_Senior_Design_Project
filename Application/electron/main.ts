// TSprech 2023/04/05 13:53:16
// Thanks to: https://davembush.medium.com/typescript-and-electron-the-right-way-141c2e15e4e1

const {app, BrowserWindow, ipcMain} = require('electron');
const EventEmitter = require('events');
const path = require('path');
const url = require('url');

const {SerialPort, BindingPort, PortInfo} = require('serialport');
const {ReadlineParser} = require('@serialport/parser-readline');

class PortManager {
  static port: typeof BindingPort;
  static parser: typeof ReadlineParser;
  static emitter = new EventEmitter();

  static async List(): Promise<typeof PortInfo[]> {
    return SerialPort.list();
  }

  // Based off: https://medium.com/@machadogj/arduino-and-node-js-via-serial-port-bcf9691fab6a
  static Connect(port_info: string, baudrate: number) {
    this.port = new SerialPort({path: port_info, baudRate: baudrate, autoOpen: true}); // Create the new port and open it (autoOpen)
    this.parser = this.port.pipe(new ReadlineParser({delimiter: '\n'})); // Create a parser which parses based on a delimiter (\n)
    this.parser.on('data', (data: string) => { // Whenever JSON data is received, this is called
      console.log('New JSON Data: ', data);
      this.ParseJSON(data); // Call for the data to be converted from a string into an Object
    });
  }

  static ParseJSON(json_data: string): any { // TODO: Keep using any and a dispatcher?
    try {
      this.emitter.emit("PortManager:NewJSON", JSON.parse(json_data)); // Send out the parsed JSON Object for a dispatcher to handle
    } catch {
      console.log("Invalid JSON was attempted to be parsed"); // This is not a huge issue as long as it isn't happening too much
    }
  }

  static Write(json_obj: any) { // TODO: Don't write if the port is not open
    this.port.write(JSON.stringify(json_obj) + '\n'); // Convert the object to a string and append the terminator that indicates end of data (\n)
  }

  static Disconnect() { // TODO: Complains the port isn't open when called?
    this.port.close();
  }
}

// PortManager.List().then((port_names) => console.log(port_names))
// PortManager.Connect('COM23', 115200);
// console.log("Connected!");
// PortManager.Write({"LED": true});
// setTimeout(() => PortManager.Write({"LED": false}), 2000);

export default class Main {
  static mainWindow: Electron.BrowserWindow | null;
  static application: Electron.App;
  static BrowserWindow: typeof Electron.CrossProcessExports.BrowserWindow;
  static dev: boolean;

  private static onWindowAllClosed() {
    if (process.platform !== 'darwin') {
      Main.application.quit();
    }
  }

  private static onClose() {
    Main.mainWindow = null; // Dereference the window object.
  }

  private static onReady() {
    Main.mainWindow = new Main.BrowserWindow({
      width: 1024,
      height: 768,
      show: false,
      webPreferences: {
        // preload: path.join(__dirname, 'preload.js'),
        nodeIntegration: true, // to allow require Had to disable these for preload: https://github.com/electron/forge/issues/2931
        contextIsolation: false // allow use with Electron 12+
      }
    });

    // let indexPath: typeof url;
    let indexPath: any;
    if (Main.dev && process.argv.indexOf('--noDevServer') === -1) {
      indexPath = url.format({
        protocol: 'http:',
        host: 'localhost:8080',
        pathname: 'index.html',
        slashes: true
      })
    } else {
      indexPath = url.format({
        protocol: 'file:',
        pathname: path.join(__dirname, 'dist', 'index.html'),
        slashes: true
      })
    }

    Main.mainWindow.loadURL(indexPath);
    Main.mainWindow.on('closed', Main.onClose);

    // Don't show until we are ready and loaded
    Main.mainWindow.once('ready-to-show', () => {
      // @ts-ignore
      Main.mainWindow.show()

      // Open the DevTools automatically if developing
      if (Main.dev) {
        const {default: installExtension, REACT_DEVELOPER_TOOLS} = require('electron-devtools-installer')

        // @ts-ignore
        installExtension(REACT_DEVELOPER_TOOLS).catch(err => console.log('Error loading React DevTools: ', err))
        // @ts-ignore
        Main.mainWindow.webContents.openDevTools()
      }
    });

    ipcMain.on('LED:On', (event, title) => {
      console.log("LED WOULD BE TURNED ON\n");
      // PortManager.Write({"LED": true});
      // setTimeout(() => PortManager.Write({"LED": false}), 1000);
    })
  }

  static main(app: Electron.App, browserWindow: typeof BrowserWindow) {
    // we pass the Electron.App object and the
    // Electron.BrowserWindow into this function
    // so this class has no dependencies. This
    // makes the code easier to write tests for
    Main.BrowserWindow = browserWindow;
    Main.application = app;
    Main.application.on('window-all-closed', Main.onWindowAllClosed);
    Main.application.on('ready', Main.onReady);

    if (process.env.NODE_ENV !== undefined && process.env.NODE_ENV === 'development') {
      Main.dev = true
    }
  }
}

// const {app, BrowserWindow} = require('electron');
// import Main from './main';

Main.main(app, BrowserWindow);
