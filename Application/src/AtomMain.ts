// TSprech 2023/04/29 16:10:13

import {BrowserWindow} from "electron";

// This class helps abstract the process of sending data from the main process to the renderer process via the IPC channel
export default class AtomMain {
    static mainWindow: BrowserWindow; // This should be the same BrowserWindow instance as in the main script

    /* Structure
    static <Function Name>(data: <Atom Type>) {
        AtomMain.mainWindow.webContents.send(<String Name In Preload>, data);
    } */

    static SerialDataRX(data: string) {
        AtomMain.mainWindow.webContents.send('Serial:Data:RX', data);
    }

    static SerialList(data: any[]) {
        AtomMain.mainWindow.webContents.send('Serial:List', data);
    }
}
