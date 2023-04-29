// 2023/04/19 16:12:23

import {atom, atomFamily, selector} from "recoil";

export class PortPair { // This represents a selectable port
  path: string; // Used to connect to the device (unique to each device)
  friendly_name: string; // Used to inform the user what port options are available
}

export const updatePorts = selector({
  key: 'UpdatePorts',
  get: async () => {
    try {
      const port_list = await window.SerialIPC.List();
      return port_list || [];
    } catch (err) {
      alert(err);
      return [];
    }
  },
});

export const Serial_Ports_Available_State = atom({
// export const Serial_Ports_Available_State = atom({
  key: "Serial:Ports:Available",
  default: updatePorts // Ties this atom into using updatePorts as its value
});

export const Serial_Ports_Selected_State = atom<PortPair>({
  key: "Serial:Ports:Selected",
  default: new PortPair
});

export const Serial_Ports_Connected_State = atom<boolean>({
  key: "Serial:Ports:Connected",
  default: false
});

export const JSON_Data_RX = atom<string>({
  key: "JSON:RX",
  default: ""
});
