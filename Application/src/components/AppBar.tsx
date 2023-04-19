// 2023/04/19 11:15:48
import * as React from 'react';
import {useColorScheme} from "@mui/joy/styles";
import IconButton from "@mui/joy/IconButton";
import DarkModeRoundedIcon from "@mui/icons-material/DarkModeRounded";
import LightModeRoundedIcon from "@mui/icons-material/LightModeRounded";
import {Button, Option, Select, Tooltip} from "@mui/joy";
import LinkOffIcon from "@mui/icons-material/LinkOff";
import LinkIcon from "@mui/icons-material/Link";

import StateObj from "../useStateObj";
import Box from "@mui/joy/Box";
import GroupRoundedIcon from "@mui/icons-material/GroupRounded";
import Typography from "@mui/joy/Typography";
import RefreshIcon from "@mui/icons-material/Refresh";
import Layout from "./Layout";

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

// async function updatePorts(available_ports: StateObj<any>) {
async function UpdatePorts(available_ports) {
// async function updatePorts(props) {
  // e.preventDefault();
  try {
    available_ports.value = await window.SerialIPC.List();
  } catch (err) {
    alert(err);
  }
}

export class PortPair { // This represents a selectable port
  path = ''; // Used to connect to the device (unique to each device)
  friendly_name = ''; // Used to inform the user what port options are available
}

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
function SerialList({available_ports, selected_port, port_connected}) {
  const list_items = available_ports.value.map(item =>
    <Option value={{path: item.path, friendly_name: item.friendlyName}} key={item.path}>
      {item.friendlyName}
    </Option>
  );

  return (
    <Select
      color="primary"
      placeholder={selected_port.value.friendly_name === undefined ? 'Select Port' : selected_port.value.friendly_name} // Check if no serial port had been selected before (like on startup) and display the Select Port prompt, otherwise display the last selected port name
      variant="outlined"
      disabled={!!port_connected.value}
      onChange={(event, selected_key: PortPair) => selected_port.value = selected_key} // When a new option is selected, change the state to reflect what port was selected
    >
      {list_items}
    </Select>
  );
}

// function SerialConnectButton(available_ports: StateObj<any>, selected_port: StateObj<any>, port_connected: StateObj<any>) {
function SerialConnectButton({available_ports, selected_port, port_connected}) {
// function SerialConnectButton(props) {
  return (
    // The tool tip gives a hint to the user whether pressing the button will connect to the serial device or disconnect from it, as such the tool tip's text depends on whether it is connected
    <Tooltip title={port_connected.value ? "Disconnect" : "Connect"}>
      <IconButton
        size="sm"
        variant={port_connected.value ? "solid" : "outlined"} //
        color={port_connected.value ? "danger" : "success"}
        component="a"
        onClick={() => {
          if (!port_connected.value) {
            window.SerialIPC.Connect({ // Call the connect function, which returns whether it successfully connected, and set the connected state to the returned value
              path: selected_port.value.path,
              baud: 115200 // This is just a constant as native USB serial, like the RP2040, does not require a baud rate (and is ignored)
            }).then((success: boolean) => {
              console.log("Connect success: ");
              console.log(success);
              port_connected.value = success;
            });
          } else {
            window.SerialIPC.Disconnect();
            port_connected.value = false;
          }
        }}
      > {port_connected.value ? <LinkOffIcon/> : <LinkIcon/>} </IconButton>
    </Tooltip>);
}

// export default function AppBar(available_ports: StateObj<any>, selected_port: StateObj<any>, port_connected: StateObj<any>) {
export default function AppBar({available_ports, selected_port, port_connected}) {
// export default function AppBar(props) {
  console.log(available_ports);
  console.log(selected_port);
  console.log(port_connected);
  return (
    <Layout.Header>
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
        <SerialList
          available_ports={available_ports}
          selected_port={selected_port}
          port_connected={port_connected}
        />

        <IconButton
          size="sm"
          variant="outlined"
          color="primary"
          component="a"
          onClick={() => UpdatePorts(available_ports)}
        > <RefreshIcon/> </IconButton>

        <SerialConnectButton
          available_ports={available_ports}
          selected_port={selected_port}
          port_connected={port_connected}/>
        <ColorSchemeToggle/>
      </Box>
    </Layout.Header>
  );
}