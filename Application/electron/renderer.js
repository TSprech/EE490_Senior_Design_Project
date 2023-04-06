"use strict";
// TSprech 2023/04/05 15:42:58
function ChangeText(new_text) {
    const para_text = document.getElementById("MyText");
    if (para_text !== null) {
        para_text.innerHTML = new_text;
    }
    else {
        alert("Error: Could not find paragraph");
    }
}
ChangeText("CUSTOM TEXT");
console.log("This script is running");
// const {SerialPort} = require('serialport');
// const {ReadlineParser} = require('@serialport/parser-readline');
//
// // import {ReadlineParser, SerialPort} from "serialport";
//
// class PortManager {
//   static port: typeof SerialPort;
//   static parser: typeof ReadlineParser;
//
//   static async listSerialPorts() {
//     // await SerialPort.list().then((ports, err) => {
//     await SerialPort.list().then((ports: any) => {
//       const para_text = document.getElementById("MyText");
//       if (para_text !== null) {
//         // para_text.innerHTML = ports.toString();
//         para_text.innerHTML = "This is the extra newest new text";
//       } else {
//         alert("Error: Could not find paragraph");
//       }
//       console.log('ports', ports);
//
//       if (ports.length === 0) {
//         console.log("No ports found")
//         // document.getElementById('error').textContent = 'No ports discovered'
//       }
//
//       // tableHTML = tableify(ports)
//       // document.getElementById('ports').innerHTML = tableHTML
//     })
//   }
// }
//
// // PortManager.listSerialPorts().then(() => console.log("Done"))
//
// function listPorts() {
//   // listSerialPorts();
//   PortManager.listSerialPorts().then(() => console.log("Done"))
//   setTimeout(listPorts, 2000);
// }
//
// // Set a timeout that will check for new serialPorts every 2 seconds.
// // This timeout reschedules itself.
// setTimeout(listPorts, 2000);
// const port = new SerialPort({
//   path: 'COM30',
//   baudRate: 115200,
//   autoOpen: false
// });
// // const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600 });
// const parser = port.pipe(new Readline({ delimiter: '\n' }));// Read the port data
// port.on("open", () => {
//   console.log('serial port open');
// });parser.on('data', data =>{
//   console.log('got word from arduino:', data);
// });
