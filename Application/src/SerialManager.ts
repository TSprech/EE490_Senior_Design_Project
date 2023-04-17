// TSprech 2023/04/11 22:02:06

const {SerialPort, BindingPort, PortInfo} = require('serialport');
// import SerialPort = require('serialport');
// import type BindingPort = require('serialport');
// import type PortInfo = require('serialport');
const {ReadlineParser} = require('@serialport/parser-readline');
const EventEmitter = require('events');

export default class PortManager {
    port: typeof BindingPort | null;
    parser: typeof ReadlineParser | null;
    emitter: typeof EventEmitter;

    constructor() {
        this.port = null;
        this.emitter = new EventEmitter();
    }

    async List(): Promise<typeof PortInfo[]> {
        return await SerialPort.list();
    }

    // Based off: https://medium.com/@machadogj/arduino-and-node-js-via-serial-port-bcf9691fab6a
    Connect(port_path: string, baudrate: number) {
        this.port = new SerialPort({path: port_path, baudRate: baudrate, autoOpen: true}); // Create the new port and open it (autoOpen)
        this.parser = this.port.pipe(new ReadlineParser({delimiter: '\n'})); // Create a parser which parses based on a delimiter (\n)
        this.parser.on('data', (data: string) => { // Whenever JSON data is received, this is called
            console.log('New JSON Data: ', data);
            this.ParseJSON(data); // Call for the data to be converted from a string into an Object
        });
        return this.port.isConnected;
    }

    Connected() {
        if (this?.port) {
            return false;
        } else {
            return this?.port?.isConnected;
        }
    }

    ParseJSON(json_data: string): any { // TODO: Keep using any and a dispatcher?
        try {
            this.emitter.emit("PortManager:NewJSON", JSON.parse(json_data)); // Send out the parsed JSON Object for a dispatcher to handle
        } catch {
            console.log("Invalid JSON was attempted to be parsed"); // This is not a huge issue as long as it isn't happening too much
        }
    }

    Write(json_obj: any) { // TODO: Don't write if the port is not open
        this.port.write(JSON.stringify(json_obj) + '\n'); // Convert the object to a string and append the terminator that indicates end of data (\n)
    }

    Disconnect() { // TODO: Complains the port isn't open when called?
        this.port.close();
    }
}