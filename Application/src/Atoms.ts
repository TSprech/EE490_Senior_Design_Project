// 2023/04/19 16:12:23

import {atom, selector} from "recoil";
// import {PortInfo} from "serialport";

export class PortPair { // This represents a selectable port
  path: string; // Used to connect to the device (unique to each device)
  friendly_name: string; // Used to inform the user what port options are available
}

// function timeout(ms) {
//   return new Promise(resolve => setTimeout(resolve, ms));
// }

// export const updatePortTrigger = atom({
//   key: "updatePortTrigger",
//   default: false
// });

export const updatePorts = selector({
  key: 'UpdatePorts',
  get: async ({get}) => {
    // const nothing = get(updatePortTrigger);
    try {
      const port_list = await window.SerialIPC.List();
      return port_list || [];
    } catch (err) {
      alert(err);
      return [];
    }
  },
});

// export const Serial_Ports_Available_State = atom<typeof PortInfo[]>({
export const Serial_Ports_Available_State = atom({
  key: "Serial:Ports:Available",
  default: updatePorts
});

export const Serial_Ports_Selected_State = atom({
  key: "Serial:Ports:Selected",
  default: new PortPair
});

export const Serial_Ports_Connected_State = atom({
  key: "Serial:Ports:Connected",
  default: false
});