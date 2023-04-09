// TSprech 2023/04/07 14:51:12

const { contextBridge, ipcRenderer } = require('electron')

// import React from 'react'
// import logo from './logo.svg';
// import '../assets/css/App.css'

console.log("Preload Running\n");

contextBridge.exposeInMainWorld('LED', {
  TurnOn: (title: string) => ipcRenderer.send('LED:On', title)
})
