// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

// eslint-disable-next-line @typescript-eslint/no-var-requires
const { contextBridge, ipcRenderer } = require('electron')

console.log("Preload Running\n");
//
// contextBridge.exposeInMainWorld('LED', {
//     TurnOn: (title: string) => ipcRenderer.send('LED:On', title)
// })

contextBridge.exposeInMainWorld('SerialIPC', {
    ListIPC: () => ipcRenderer.invoke('Serial:List')
})

contextBridge.exposeInMainWorld('SerialIPC2', {
    ConnectIPC: (port_data) => ipcRenderer.invoke('Serial:Connect', port_data)
})

contextBridge.exposeInMainWorld('SerialIPC3', {
    ConnectedIPC: () => ipcRenderer.invoke('Serial:Connected')
})

contextBridge.exposeInMainWorld('SerialIPC4', {
    DisconnectIPC: () => ipcRenderer.invoke('Serial:Disconnect')
})