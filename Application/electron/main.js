"use strict";
// TSprech 2023/04/05 13:53:16
// Thanks to: https://davembush.medium.com/typescript-and-electron-the-right-way-141c2e15e4e1
Object.defineProperty(exports, "__esModule", { value: true });
const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const url = require('url');
const { SerialPort, BindingPort, PortInfo } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
// const RendererEvents = require('./src/components/App');
// @ts-ignore
// import RendererEvents from './src/components/App.js';
const EventEmitter = require('events');
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
        // @ts-ignore
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
PortManager.List().then((port_names) => console.log(port_names));
PortManager.Connect('COM5', 115200);
console.log("Connected!");
PortManager.Write({ "LED": true });
setTimeout(() => PortManager.Write({ "LED": false }), 2000);
// @ts-ignore
const RendererEvents = require('./src/components/Test');
RendererEvents.on("BLINK", () => PortManager.Write({ "LED": true }));
class Main {
    static mainWindow;
    static application;
    static BrowserWindow;
    static dev;
    static onWindowAllClosed() {
        if (process.platform !== 'darwin') {
            Main.application.quit();
        }
    }
    static onClose() {
        Main.mainWindow = null; // Dereference the window object.
    }
    static onReady() {
        Main.mainWindow = new Main.BrowserWindow({
            width: 1024,
            height: 768,
            show: false,
            webPreferences: {
                nodeIntegration: true,
                contextIsolation: false // allow use with Electron 12+
            }
        });
        // let indexPath: typeof url;
        let indexPath;
        if (Main.dev && process.argv.indexOf('--noDevServer') === -1) {
            indexPath = url.format({
                protocol: 'http:',
                host: 'localhost:8080',
                pathname: 'index.html',
                slashes: true
            });
        }
        else {
            indexPath = url.format({
                protocol: 'file:',
                pathname: path.join(__dirname, 'dist', 'index.html'),
                slashes: true
            });
        }
        Main.mainWindow.loadURL(indexPath);
        Main.mainWindow.on('closed', Main.onClose);
        // Don't show until we are ready and loaded
        Main.mainWindow.once('ready-to-show', () => {
            // @ts-ignore
            Main.mainWindow.show();
            // Open the DevTools automatically if developing
            if (Main.dev) {
                const { default: installExtension, REACT_DEVELOPER_TOOLS } = require('electron-devtools-installer');
                // @ts-ignore
                installExtension(REACT_DEVELOPER_TOOLS).catch(err => console.log('Error loading React DevTools: ', err));
                // @ts-ignore
                Main.mainWindow.webContents.openDevTools();
            }
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
        if (process.env.NODE_ENV !== undefined && process.env.NODE_ENV === 'development') {
            Main.dev = true;
        }
    }
}
exports.default = Main;
// const {app, BrowserWindow} = require('electron');
// import Main from './main';
Main.main(app, BrowserWindow);
