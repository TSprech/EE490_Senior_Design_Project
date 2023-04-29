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
import RecoilNexus, {getRecoil, setRecoil} from "recoil-nexus";
import {
    RecoilRoot,
    atom,
    selector,
    useRecoilState,
    useRecoilValue, useSetRecoilState,
} from 'recoil';

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
import {Button, Option, Select, Textarea, Tooltip} from "@mui/joy";

import StateObj from './useStateObj';
// import useStateObj from './useStateObj';
import PortPair from './components/AppBar';
import AppBar from './components/AppBar';
import {
    JSON_Data_RX,
    Serial_Ports_Available_State,
    Serial_Ports_Connected_State,
    Serial_Ports_Selected_State
} from "./Atoms";

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


function JSONPrint() {
    const get_json_data_rx = useRecoilValue(JSON_Data_RX);
    return (
        <p> {get_json_data_rx} </p>
    );
}

window.AtomIPC.SerialNewData((event, value) => {
    setRecoil(JSON_Data_RX, value);
})

export default function RenderIndex() {
    const [drawerOpen, setDrawerOpen] = React.useState(false);
    // const [ports, setPorts] = React.useState([]); // Manages the list of available ports
    // const [selectedPort, setSelectedPort] = React.useState(new PortPair); // Manages the current port that the user has selected
    // const [connected, setConnected] = React.useState(false); // Manages the current port that the user has selected

    // let temp_port_pair = new PortPair();
    // const available_ports = new StateObj([]);
    // const selected_port = new StateObj(temp_port_pair);
    // const port_connected = new StateObj(false);

    return (
      <RecoilRoot>
        <RecoilNexus />
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
                <AppBar/>
                <Layout.Main>
                    <JSONPrint/>
                </Layout.Main>
            </Layout.Root>
        </CssVarsProvider>
      </RecoilRoot>
    );
}
