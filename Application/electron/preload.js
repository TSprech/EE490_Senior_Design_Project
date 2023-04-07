"use strict";
// TSprech 2023/04/07 14:51:12
const { contextBridge, ipcRenderer } = require('electron');
console.log("Preload Running\n");
contextBridge.exposeInMainWorld('LED', {
    TurnOn: (title) => ipcRenderer.send('LED:On', title)
});
