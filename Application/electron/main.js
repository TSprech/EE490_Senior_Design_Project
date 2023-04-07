"use strict";
// TSprech 2023/04/05 13:53:16
// Thanks to: https://davembush.medium.com/typescript-and-electron-the-right-way-141c2e15e4e1
Object.defineProperty(exports, "__esModule", { value: true });
const { app, BrowserWindow, ipcMain } = require('electron');
const { EventEmitter } = require('events');
const path = require('path');
// All non-electron based requires should be placed below this comment after pnp.setup(), require('electron') should be above this
require('./.pnp.cjs').setup(); // Required for Yarn PnP (Plug N Play) functionality without changing CL args
const { SerialPort, BindingPort, PortInfo } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
class PortManager {
    static port;
    static parser;
    static emitter = new EventEmitter();
    static async List() {
        return SerialPort.list();
    }
    // Based off: https://medium.com/@machadogj/arduino-and-node-js-via-serial-port-bcf9691fab6a
    static Connect(port_info, baudrate) {
        this.port = new SerialPort({ path: port_info, baudRate: baudrate, autoOpen: true }); // Create the new port and open it (autoOpen)
        this.parser = this.port.pipe(new ReadlineParser({ delimiter: '\n' })); // Create a parser which parses based on a delimiter (\n)
        this.parser.on('data', (data) => {
            console.log('New JSON Data: ', data);
            this.ParseJSON(data); // Call for the data to be converted from a string into an Object
        });
    }
    static ParseJSON(json_data) {
        try {
            this.emitter.emit("PortManager:NewJSON", JSON.parse(json_data)); // Send out the parsed JSON Object for a dispatcher to handle
        }
        catch {
            console.log("Invalid JSON was attempted to be parsed"); // This is not a huge issue as long as it isn't happening too much
        }
    }
    static Write(json_obj) {
        this.port.write(JSON.stringify(json_obj) + '\n'); // Convert the object to a string and append the terminator that indicates end of data (\n)
    }
    static Disconnect() {
        this.port.close();
    }
}
// PortManager.List().then((port_names) => console.log(port_names))
PortManager.Connect('COM23', 115200);
console.log("Connected!");
// PortManager.Write({"LED": true});
// setTimeout(() => PortManager.Write({"LED": false}), 2000);
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
        // @ts-ignore
        Main.mainWindow = null; // Dereference the window object.
    }
    static onReady() {
        Main.mainWindow = new Main.BrowserWindow({
            width: 800,
            height: 600,
            backgroundColor: "#ccc",
            webPreferences: {
                preload: path.join(__dirname, 'preload.js'),
                // nodeIntegration: true, // to allow require Had to disable these for preload: https://github.com/electron/forge/issues/2931
                // contextIsolation: false // allow use with Electron 12+
            }
        });
        Main.mainWindow.loadFile('index.html');
        Main.mainWindow.on('closed', Main.onClose);
        ipcMain.on('LED:On', (event, title) => {
            PortManager.Write({ "LED": true });
            setTimeout(() => PortManager.Write({ "LED": false }), 1000);
        });
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
