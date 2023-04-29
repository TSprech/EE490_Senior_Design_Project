// TSprech 2023/04/11 22:02:06

import {useSetRecoilState} from "recoil";

const {SerialPort, BindingPort, PortInfo} = require('serialport');
// import SerialPort = require('serialport');
// import type BindingPort = require('serialport');
// import type PortInfo = require('serialport');
const {ReadlineParser} = require('@serialport/parser-readline');
const EventEmitter = require('events');

import {JSON_Data_RX} from "./Atoms"
import {getRecoil, setRecoil} from "recoil-nexus";

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
  async Connect(port_path: string, baudrate: number): Promise<boolean> {
    const portOpenPromise = () => { // Converts the callback based SerialPort.open to a promise based one (maybe could use promisify?)
      return new Promise<void>((resolve, reject) => { // It doesn't return anything, just is fulfilled or not
        this.port.open((err) => { // Open using the callback but with the promise resolution as the callback body
          if (err) return reject(err);
          resolve();
        })
      })
    }

    try {
      this.port = new SerialPort({path: port_path, baudRate: baudrate, autoOpen: false}); // Create the new port and but don't open it
      this.parser = this.port.pipe(new ReadlineParser({delimiter: '\n'})); // Create a parser which parses based on a delimiter (\n)
      this.parser.on('data', (data: string) => { // Whenever JSON data is received, this is called
        // console.log('New JSON Data: ', data);
        this.ParseJSON(data); // Call for the data to be converted from a string into an Object
      });

      await portOpenPromise(); // Wait for the port to open
      return this.port.isOpen; // Return whether the port opened successfully
    } catch (e) {
      return false; // If something threw, the port didn't open TODO: Double check this
    }
  }

  Connected() {
      return this?.port?.isOpen;
  }

  ParseJSON(json_data: string): any { // TODO: Keep using any and a dispatcher?
    // const set_json_data_rx = getRecoil(JSON_Data_RX);
    try {
      // setRecoil(JSON_Data_RX, json_data);
      console.log("Valid JSON: ", json_data);
      // set_json_data_rx(JSON.parse(json_data));
      // this.emitter.emit("PortManager:NewJSON", JSON.parse(json_data)); // Send out the parsed JSON Object for a dispatcher to handle
    } catch (e) {
      console.log(e);
      console.log("Invalid JSON: ", json_data); // This is not a huge issue as long as it isn't happening too much
    }
  }

  Write(json_obj: any) { // TODO: Don't write if the port is not open
    if (this.port.isOpen) {
      this.port.write(JSON.stringify(json_obj) + '\n'); // Convert the object to a string and append the terminator that indicates end of data (\n)
    }
  }

  Disconnect() {
    try {
      if (this?.port?.isOpen) { // Check if the port is actually open (otherwise the library complains the port isn't open)
        this.port.close(); // Then close it
      }
    } catch (e) {
      console.log(e);
    }
  }
}