// TSprech 2023/04/29 16:10:13

import {BrowserWindow} from "electron";

export default class AtomMain {
    static mainWindow: BrowserWindow;

    static SerialDataRX(data: string) {
        AtomMain.mainWindow.webContents.send('Serial:Data:RX', data);
    }

    static SerialList(data: any[]) {
        AtomMain.mainWindow.webContents.send('Serial:List', data);
    }
}
