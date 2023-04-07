// TSprech 2023/04/05 13:53:16
// Thanks to: https://davembush.medium.com/typescript-and-electron-the-right-way-141c2e15e4e1

const {BrowserWindow} = require('electron');
const {EventEmitter} = require('events');

// All non-electron based requires should be placed below this comment after pnp.setup(), require('electron') should be above this
require('./.pnp.cjs').setup(); // Required for Yarn PnP (Plug N Play) functionality without changing CL args
const {SerialPort, BindingPort, PortInfo} = require('serialport');
const {ReadlineParser} = require('@serialport/parser-readline');

class PortManager {
  // static on_receive_callback: (json_obj: object) => void;
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

PortManager.List().then((port_names) => console.log(port_names))
PortManager.Connect('COM23', 115200);
console.log("Connected!");
PortManager.Write({"LED": true});
setTimeout(() => PortManager.Write({"LED": false}), 2000);
// PortManager.Disconnect();

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
