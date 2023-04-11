// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

// eslint-disable-next-line @typescript-eslint/no-var-requires
const { contextBridge, ipcRenderer } = require('electron')

console.log("Preload Running\n");

contextBridge.exposeInMainWorld('LED', {
    TurnOn: (title: string) => ipcRenderer.send('LED:On', title)
})
