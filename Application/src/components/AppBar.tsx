// 2023/04/19 11:15:48
import * as React from 'react';
import {useColorScheme} from "@mui/joy/styles";
import IconButton from "@mui/joy/IconButton";
import DarkModeRoundedIcon from "@mui/icons-material/DarkModeRounded";
import LightModeRoundedIcon from "@mui/icons-material/LightModeRounded";
import {Button, Option, Select, Tooltip} from "@mui/joy";
import LinkOffIcon from "@mui/icons-material/LinkOff";
import LinkIcon from "@mui/icons-material/Link";
import Box from "@mui/joy/Box";
import GroupRoundedIcon from "@mui/icons-material/GroupRounded";
import Typography from "@mui/joy/Typography";
import RefreshIcon from "@mui/icons-material/Refresh";
import Layout from "./Layout";

import {
  PortPair,
  Serial_Ports_Available_State,
  Serial_Ports_Connected_State,
  Serial_Ports_Selected_State,
  updatePorts
} from './../Atoms'
import {selector, useRecoilRefresher_UNSTABLE, useRecoilState, useRecoilValue, useSetRecoilState} from "recoil";
import {Suspense, useEffect} from "react";

function ColorSchemeToggle() {
  const {mode, setMode} = useColorScheme();
  const [mounted, setMounted] = React.useState(false);
  React.useEffect(() => {
    setMounted(true);
  }, []);
  if (!mounted) {
    return <IconButton size="sm" variant="outlined" color="primary"/>;
  }
  return (
    <IconButton
      id="toggle-mode"
      size="sm"
      variant="outlined"
      color="primary"
      onClick={() => {
        if (mode === 'light') {
          setMode('dark');
        } else {
          setMode('light');
        }
      }}
    >
      {mode === 'light' ? <DarkModeRoundedIcon/> : <LightModeRoundedIcon/>}
    </IconButton>
  );
}



// // async function updatePorts(available_ports: StateObj<any>) {
// async function UpdatePorts() {
// // async function updatePorts(props) {
//   // e.preventDefault();
//   const set_available_ports = useSetRecoilState(updatePorts);
//   try {
//     const port_list = await window.SerialIPC.List();
//     set_available_ports(port_list);
//   } catch (err) {
//     alert(err);
//   }
// }

function EStopButton() {
  const [eStopped, setEStopped] = React.useState(false);
  return (
    // The tool tip gives a hint to the user whether pressing the button will connect to the serial device or disconnect from it, as such the tool tip's text depends on whether it is connected
    <Tooltip title={eStopped ? "Release E-Stop" : "Engage E-Stop"}>
      <Button
        size="sm"
        variant={eStopped ? "solid" : "outlined"} //
        color="danger"
        component="a"
        onClick={() => {
          if (!eStopped) {
            // Send EStop JSON command
            setEStopped(true);
          } else {
            // Send EStop release JSON command
            setEStopped(false);
          }
        }}
      > E-Stop </Button>
    </Tooltip>);
}

// function SerialList(available_ports: StateObj<any>, selected_port: StateObj<any>, port_connected: StateObj<any>) {
function SerialList() {
  const get_available_ports = useRecoilValue(Serial_Ports_Available_State);
  const set_selected_ports = useSetRecoilState(Serial_Ports_Selected_State);
  const get_port_connected = useRecoilValue(Serial_Ports_Connected_State);

  const list_items = get_available_ports.map(item =>
    <Option value={{path: item.path, friendly_name: item.friendlyName}} key={item.path}>
      {item.friendlyName}
    </Option>
  );

  return (
    <Select
      color="primary"
      placeholder={'Select Port'} // Check if no serial port had been selected before (like on startup) and display the Select Port prompt, otherwise display the last selected port name
      variant="outlined"
      disabled={!!get_port_connected}
      onChange={(event, selected_key: PortPair) => set_selected_ports(selected_key)} // When a new option is selected, change the state to reflect what port was selected
    >
      {list_items}
    </Select>
  );
}

// function SerialConnectButton(available_ports: StateObj<any>, selected_port: StateObj<any>, port_connected: StateObj<any>) {
function SerialConnectButton() {
// function SerialConnectButton(props) {
  const get_selected_ports = useRecoilValue(Serial_Ports_Selected_State);
  const [get_port_connected, set_port_connected] = useRecoilState(Serial_Ports_Connected_State);

  return (
    // The tool tip gives a hint to the user whether pressing the button will connect to the serial device or disconnect from it, as such the tool tip's text depends on whether it is connected
    <Tooltip title={get_port_connected ? "Disconnect" : "Connect"}>
      <IconButton
        size="sm"
        variant={get_port_connected ? "solid" : "outlined"} //
        color={get_port_connected ? "danger" : "success"}
        component="a"
        onClick={() => {
          if (!get_port_connected) {
            window.SerialIPC.Connect({ // Call the connect function, which returns whether it successfully connected, and set the connected state to the returned value
              path: get_selected_ports.path,
              baud: 115200 // This is just a constant as native USB serial, like the RP2040, does not require a baud rate (and is ignored)
            }).then((success: boolean) => {
              console.log("Connect success: ");
              console.log(success);
              set_port_connected(success);
            });
          } else {
            window.SerialIPC.Disconnect();
            set_port_connected(false);
          }
        }}
      > {get_port_connected ? <LinkOffIcon/> : <LinkIcon/>} </IconButton>
    </Tooltip>);
}

// export default function AppBar(available_ports: StateObj<any>, selected_port: StateObj<any>, port_connected: StateObj<any>) {
export default function AppBar() {
// export default function AppBar(props) {

  // useEffect(() => {
  //   const interval = setInterval(() => {
  //     const nothing = useSetRecoilState(updatePortTrigger);
  //     nothing(true);
  //   }, 2000);
  //   return () => clearInterval(interval);
  // }, []);

  // const nothing = useSetRecoilState(updatePortTrigger);
  // const nothing = useRecoilValue(Serial_Ports_Available_State);
  const refreshUserInfo = useRecoilRefresher_UNSTABLE(Serial_Ports_Available_State); // Thanks to: https://github.com/facebookexperimental/Recoil/issues/85#issuecomment-973110381

  return (
    <Layout.Header>
      <Suspense // Huge thanks to: https://www.valentinog.com/blog/await-react/
        fallback={
          <div>
            Loading info...
          </div>
        }
      >

      <Box
        sx={{
          display: 'flex',
          flexDirection: 'row',
          alignItems: 'center',
          gap: 1.5,
        }}
      >
        <IconButton
          size="sm"
          variant="solid"
          sx={{display: {xs: 'none', sm: 'inline-flex'}}}
        >
          <GroupRoundedIcon/>
        </IconButton>
        <Typography component="h1" fontWeight="xl">
          Electron Joy
        </Typography>
      </Box>
      <Box sx={{display: 'flex', flexDirection: 'row', gap: 1.5}}>
        <EStopButton/>
        <SerialList/>

        <IconButton
          size="sm"
          variant="outlined"
          color="primary"
          component="a"
          onClick={() => refreshUserInfo()}
        > <RefreshIcon/> </IconButton>

        <SerialConnectButton/>
        <ColorSchemeToggle/>
      </Box>
      </Suspense>
    </Layout.Header>
  );
}