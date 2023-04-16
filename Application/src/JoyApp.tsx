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

// custom
import teamTheme from './theme';
import Layout from './components/Layout';
import {Option, Select} from "@mui/joy";

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

export default function TeamExample() {
    const [drawerOpen, setDrawerOpen] = React.useState(false);
    const [ports, setPorts] = React.useState([]);
    const [selectedPort, setSelectedPort] = React.useState(new PortPair);

    async function updatePorts(e) {
        // e.preventDefault();
        try {
            const ret_ports = await window.SerialIPC.ListIPC();
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
                onChange={(event, selected_key: PortPair) => setSelectedPort(selected_key)} // When a new option is selected, change the state to reflect what port was selected
            >
                {list_items}
            </Select>
        );
    }

    return (
        <CssVarsProvider disableTransitionOnChange theme={teamTheme}>
            <CssBaseline/>
            {/*{drawerOpen && (*/}
            {/*    <Layout.SideDrawer onClose={() => setDrawerOpen(false)}>*/}
            {/*        /!*<TeamNav />*!/*/}
            {/*    </Layout.SideDrawer>*/}
            {/*)}*/}
            <Layout.Root
                // sx={{
                //     ...(drawerOpen && {
                //         height: '100vh',
                //         overflow: 'hidden',
                //     }),
                // }}
            >
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
                        <SerialList/>

                        <IconButton
                            size="sm"
                            variant="outlined"
                            color="primary"
                            component="a"
                            onClick={updatePorts}
                        > <RefreshIcon/> </IconButton>

                        <IconButton
                            size="sm"
                            variant="outlined"
                            color="primary"
                            component="a"
                            // onClick={window.SerialIPC.ConnectIPC()}
                        > <LinkIcon/> </IconButton>
                        <ColorSchemeToggle/>
                    </Box>
                </Layout.Header>
                {/*<Layout.Main>*/}
                {/*</Layout.Main>*/}
            </Layout.Root>
        </CssVarsProvider>
    );
}
