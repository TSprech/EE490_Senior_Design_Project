// See the Electron documentation for details on how to use preload scripts:
// https://www.electronjs.org/docs/latest/tutorial/process-model#preload-scripts

// eslint-disable-next-line @typescript-eslint/no-var-requires
import {setRecoil} from "recoil-nexus";
import {
    JSON_Data_RX
} from "./Atoms";

const { contextBridge, ipcRenderer } = require('electron')

// console.log("Preload Running\n");

contextBridge.exposeInMainWorld('SerialIPC', {
    List: () => ipcRenderer.invoke('Serial:List'),
    Connect: (port_data) => ipcRenderer.invoke('Serial:Connect', port_data),
    Connected: () => ipcRenderer.invoke('Serial:Connected'),
    Disconnect: () => ipcRenderer.invoke('Serial:Disconnect')
})

// contextBridge.exposeInMainWorld('RendererIPC', {
//     DoneFirstRender: () => ipcRenderer.invoke('Renderer:Done:FirstRender')
// })

// contextBridge.exposeInMainWorld('AtomIPC', {
//     IPCCall: () => ipcRenderer.on('Atom:Call', () => setRecoil(JSON_Data_RX, "HELLO"))
// })

// function callback() {
//     // setRecoil(JSON_Data_RX, "HELLO");
//     console.log("Test");
// }

contextBridge.exposeInMainWorld('AtomIPC', {
    SerialNewData: (callback) => ipcRenderer.on('Atom:Serial:NewData', callback)
})
