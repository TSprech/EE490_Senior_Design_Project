// TSprech 2023/04/29 16:11:18

import {setRecoil} from "recoil-nexus";
import {JSON_Data_RX, Serial_Ports_Available_State} from "./Atoms";
import IpcMainEvent = Electron.IpcMainEvent;

window.SerialIPC.DataRX((event: IpcMainEvent, value: string) => {
    console.log("Serial");
    setRecoil(JSON_Data_RX, value);
});

window.SerialIPC.List((event: IpcMainEvent, value: any[]) => {
    console.log("Interval");
    setRecoil(Serial_Ports_Available_State, value);
});
