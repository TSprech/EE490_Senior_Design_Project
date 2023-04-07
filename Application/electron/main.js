"use strict";
// TSprech 2023/04/05 13:53:16
// Thanks to: https://davembush.medium.com/typescript-and-electron-the-right-way-141c2e15e4e1
Object.defineProperty(exports, "__esModule", { value: true });
const { BrowserWindow } = require('electron');
// All non-electron based requires should be placed below this comment after pnp.setup(), require('electron') should be above this
require('./.pnp.cjs').setup(); // Required for Yarn PnP (Plug N Play) functionality without changing CL args
const { SerialPort, BindingPort, PortInfo } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
class PortManager {
    static port;
    static parser;
    static async List() {
        let port_list = []; // Create an array that will be filled with the port info
        await SerialPort.list().then(// Get the list of serial ports
        function (ports) {
            for (const portsKey in ports) { // Go through each port listed
                port_list.push(ports.at(portsKey));
                // port_list.push({ // And create a new object entry in the array
                // path: ports.at(portsKey).path, // Get the path | eg: "COM6"
                // friendly_name: ports.at(portsKey).friendlyName, // Get the friendly name | eg. "USB Serial Device (COM6)"
                // manufacturer: ports.at(portsKey).manufacturer // Get the manufacturer, probably not used but good to have
                // });
            }
        }, function (error) {
        });
        return port_list; // Return whatever entries have been added to the PortInfoSmall array
    }
    static Connect(port_info, baudrate) {
        this.port = new SerialPort(port_info.path, { baudRate: baudrate });
        this.parser = this.port.pipe(new ReadlineParser({ delimiter: '\n' }));
    }
    static Disconnect() {
        // this.port.
    }
}
PortManager.List().then((port_names) => console.log(port_names));
class Main {
    static mainWindow;
    static application;
    static BrowserWindow;
    static onWindowAllClosed() {
        if (process.platform !== 'darwin') {
            Main.application.quit();
        }
    }
    static onClose() {
        // Dereference the window object.
        // @ts-ignore
        Main.mainWindow = null;
    }
    static onReady() {
        Main.mainWindow = new Main.BrowserWindow({
            width: 800,
            height: 600,
            backgroundColor: "#ccc",
            webPreferences: {
                nodeIntegration: true,
                contextIsolation: false // allow use with Electron 12+
            }
        });
        Main.mainWindow.loadFile('index.html');
        Main.mainWindow.on('closed', Main.onClose);
    }
    static main(app, browserWindow) {
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
exports.default = Main;
