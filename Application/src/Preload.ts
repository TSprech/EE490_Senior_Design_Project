// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

// eslint-disable-next-line @typescript-eslint/no-var-requires
const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('SerialIPC', {
    // Main → Renderer
    List: (callback) => ipcRenderer.on('Serial:List', callback),
    DataRX: (callback) => ipcRenderer.on('Serial:Data:RX', callback),
    // Renderer → Main
    Connect: (port_data) => ipcRenderer.invoke('Serial:Connect', port_data),
    Connected: () => ipcRenderer.invoke('Serial:Connected'),
    Disconnect: () => ipcRenderer.invoke('Serial:Disconnect')
})