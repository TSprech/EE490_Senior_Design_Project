// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

// eslint-disable-next-line @typescript-eslint/no-var-requires
const { contextBridge, ipcRenderer } = require('electron')

// console.log("Preload Running\n");

contextBridge.exposeInMainWorld('SerialIPC', {
    List: () => ipcRenderer.invoke('Serial:List'),
    Connect: (port_data) => ipcRenderer.invoke('Serial:Connect', port_data),
    Connected: () => ipcRenderer.invoke('Serial:Connected'),
    Disconnect: () => ipcRenderer.invoke('Serial:Disconnect')
})