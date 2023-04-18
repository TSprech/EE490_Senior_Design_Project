import * as React from 'react';
import {CssVarsProvider, useColorScheme} from '@mui/joy/styles';
import CssBaseline from '@mui/joy/CssBaseline';
import Box from '@mui/joy/Box';
import Chip from '@mui/joy/Chip';
import Typography from '@mui/joy/Typography';
import IconButton from '@mui/joy/IconButton';
import List from '@mui/joy/List';
import ListSubheader from '@mui/joy/ListSubheader';
import ListItem from '@mui/joy/ListItem';
import ListItemButton from '@mui/joy/ListItemButton';
import ListItemDecorator from '@mui/joy/ListItemDecorator';
import ListItemContent from '@mui/joy/ListItemContent';

// Icons import
import PeopleRoundedIcon from '@mui/icons-material/PeopleRounded';
import AssignmentIndRoundedIcon from '@mui/icons-material/AssignmentIndRounded';
import ArticleRoundedIcon from '@mui/icons-material/ArticleRounded';
import SearchRoundedIcon from '@mui/icons-material/SearchRounded';
import DarkModeRoundedIcon from '@mui/icons-material/DarkModeRounded';
import LightModeRoundedIcon from '@mui/icons-material/LightModeRounded';
import KeyboardArrowDownRoundedIcon from '@mui/icons-material/KeyboardArrowDownRounded';
import MenuIcon from '@mui/icons-material/Menu';
import GroupRoundedIcon from '@mui/icons-material/GroupRounded';
import RefreshIcon from '@mui/icons-material/Refresh';
import LinkIcon from '@mui/icons-material/Link';
import LinkOffIcon from '@mui/icons-material/LinkOff';

// custom
import teamTheme from './Theme';
import Layout from './components/Layout';
import {Button, Option, Select, Tooltip} from "@mui/joy";

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

function TeamNav() {
    return (
        <List size="sm" sx={{'--ListItem-radius': '8px', '--List-gap': '4px'}}>
            <ListItem nested>
                <ListSubheader>
                    Browse
                    <IconButton
                        size="sm"
                        variant="plain"
                        color="primary"
                        sx={{'--IconButton-size': '24px', ml: 'auto'}}
                    >
                        <KeyboardArrowDownRoundedIcon fontSize="small" color="primary"/>
                    </IconButton>
                </ListSubheader>
                <List
                    aria-labelledby="nav-list-browse"
                    sx={{
                        '& .JoyListItemButton-root': {p: '8px'},
                    }}
                >
                    <ListItem>
                        <ListItemButton variant="soft" color="primary">
                            <ListItemDecorator sx={{color: 'inherit'}}>
                                <PeopleRoundedIcon fontSize="small"/>
                            </ListItemDecorator>
                            <ListItemContent>People</ListItemContent>
                        </ListItemButton>
                    </ListItem>
                    <ListItem>
                        <ListItemButton>
                            <ListItemDecorator sx={{color: 'neutral.500'}}>
                                <AssignmentIndRoundedIcon fontSize="small"/>
                            </ListItemDecorator>
                            <ListItemContent>Managing accounts</ListItemContent>
                        </ListItemButton>
                    </ListItem>
                    <ListItem>
                        <ListItemButton>
                            <ListItemDecorator sx={{color: 'neutral.500'}}>
                                <ArticleRoundedIcon fontSize="small"/>
                            </ListItemDecorator>
                            <ListItemContent>Policies</ListItemContent>
                            <Chip
                                variant="soft"
                                color="info"
                                size="sm"
                                sx={{borderRadius: 'sm'}}
                            >
                                Beta
                            </Chip>
                        </ListItemButton>
                    </ListItem>
                </List>
            </ListItem>
        </List>
    );
}

class PortPair { // This represents a selectable port
    path: string; // Used to connect to the device (unique to each device)
    friendly_name: string; // Used to inform the user what port options are available
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

export default function RenderIndex() {
    const [drawerOpen, setDrawerOpen] = React.useState(false);
    const [ports, setPorts] = React.useState([]); // Manages the list of available ports
    const [selectedPort, setSelectedPort] = React.useState(new PortPair); // Manages the current port that the user has selected
    const [connected, setConnected] = React.useState(false); // Manages the current port that the user has selected

    async function updatePorts(e) {
        // e.preventDefault();
        try {
            const ret_ports = await window.SerialIPC.List();
            setPorts(ret_ports);
        } catch (err) {
            alert(err);
        }
    }

    function SerialList() {
        const list_items = ports.map(item =>
            <Option value={{path: item.path, friendly_name: item.friendlyName}} key={item.path}>
                {item.friendlyName}
            </Option>
        );

        return (
            <Select
                color="primary"
                placeholder={selectedPort.friendly_name === undefined ? 'Select Port' : selectedPort.friendly_name} // Check if no serial port had been selected before (like on startup) and display the Select Port prompt, otherwise display the last selected port name
                variant="outlined"
                disabled={connected ? true : false}
                onChange={(event, selected_key: PortPair) => setSelectedPort(selected_key)} // When a new option is selected, change the state to reflect what port was selected
            >
                {list_items}
            </Select>
        );
    }

    function SerialConnectButton() {
        return (
            // The tool tip gives a hint to the user whether pressing the button will connect to the serial device or disconnect from it, as such the tool tip's text depends on whether it is connected
            <Tooltip title={connected ? "Disconnect" : "Connect"}>
                <IconButton
                    size="sm"
                    variant={connected ? "solid" : "outlined"} //
                    color={connected ? "danger" : "success"}
                    component="a"
                    onClick={() => {
                        if (!connected) {
                            setConnected(window.SerialIPC.Connect({ // Call the connect function, which returns whether it successfully connected, and set the connected state to the returned value
                                path: selectedPort.path,
                                baud: 115200 // This is just a constant as native USB serial, like the RP2040, does not require a baud rate (and is ignored)
                            }))
                        } else {
                            window.SerialIPC.Disconnect();
                            setConnected(false);
                        }
                    }}
                > {connected ? <LinkOffIcon/> : <LinkIcon/>} </IconButton>
            </Tooltip>);
    }

    return (
        <CssVarsProvider disableTransitionOnChange theme={teamTheme}>
        <CssBaseline/>
            <Layout.Root
                // sx={{
                //     ...(drawerOpen && {
                //         height: '100vh',
                //         overflow: 'hidden',
                //     }),
                // }}
            >
            {/*<p>Here is text</p>*/}
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
                        <SerialList/>

                        <IconButton
                            size="sm"
                            variant="outlined"
                            color="primary"
                            component="a"
                            onClick={updatePorts}
                        > <RefreshIcon/> </IconButton>

                        <SerialConnectButton/>
                        <ColorSchemeToggle/>
                    </Box>
                </Layout.Header>
                {/*<Layout.Main>*/}
                {/*</Layout.Main>*/}
            </Layout.Root>
        </CssVarsProvider>
    );
}
