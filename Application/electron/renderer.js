// TSprech 2023/04/05 15:42:58

function ChangeText() {
  document.getElementById("MyText").innerHTML = "NEW NEW NEW";
}

ChangeText();

console.log("This script is running");


const {SerialPort} = require('serialport');
const Readline = require('@serialport/parser-readline');
const port = new SerialPort({
  path: 'COM5',
  baudRate: 115200,
  // autoOpen: false
});
// const port = new SerialPort('/dev/ttyACM0', { baudRate: 9600 });
const parser = port.pipe(new Readline({ delimiter: '\n' }));// Read the port data
port.on("open", () => {
  console.log('serial port open');
});parser.on('data', data =>{
  console.log('got word from arduino:', data);
});
