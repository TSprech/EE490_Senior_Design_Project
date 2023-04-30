import * as React from 'react';
import {CssVarsProvider} from '@mui/joy/styles';
import CssBaseline from '@mui/joy/CssBaseline';
import Chip from '@mui/joy/Chip';
import IconButton from '@mui/joy/IconButton';
import List from '@mui/joy/List';
import ListSubheader from '@mui/joy/ListSubheader';
import ListItem from '@mui/joy/ListItem';
import ListItemButton from '@mui/joy/ListItemButton';
import ListItemDecorator from '@mui/joy/ListItemDecorator';
import ListItemContent from '@mui/joy/ListItemContent';
import RecoilNexus from "recoil-nexus";
import {RecoilRoot, useRecoilValue,} from 'recoil';

import './AtomRenderer'

// Icons import
import PeopleRoundedIcon from '@mui/icons-material/PeopleRounded';
import AssignmentIndRoundedIcon from '@mui/icons-material/AssignmentIndRounded';
import ArticleRoundedIcon from '@mui/icons-material/ArticleRounded';
import KeyboardArrowDownRoundedIcon from '@mui/icons-material/KeyboardArrowDownRounded';

// custom
import teamTheme from './Theme';
import Layout from './components/Layout';
// import useStateObj from './useStateObj';
import AppBar from './components/AppBar';
import {JSON_Data_RX} from "./Atoms";
import {AspectRatio, Button, Card, Divider, Stack, styled} from "@mui/joy";
import Typography from "@mui/joy/Typography";
import {BookmarkAdd} from "@mui/icons-material";
import Box from "@mui/joy/Box";
import Sheet from "@mui/joy/Sheet";

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
        // <p> {get_json_data_rx} </p>
        // <p> {JSON.parse('{"Bool":true,"Float":3.1415,"Int":2,"String":"Hello World!"}').Int} </p>
        <p> {JSON.parse(get_json_data_rx).Int} </p>
    );
}

const Item = styled(Sheet)(({theme}) => ({
    backgroundColor:
        theme.palette.mode === 'dark' ? theme.palette.background.level1 : '#fff',
    ...theme.typography.body2,
    padding: theme.spacing(1),
    textAlign: 'center',
    borderRadius: 4,
    color: theme.vars.palette.text.secondary,
}));


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
            <RecoilNexus/>
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
                        <Card variant="outlined" sx={{width: 420}}>
                            <Typography level="h2" fontSize="md" sx={{mb: 0.5}}>
                                Voltage In (V)
                            </Typography>
                            <AspectRatio minHeight="120px" maxHeight="200px" sx={{my: 2}}>
                                <img
                                    src="./dummy_200x120.png"
                                />
                            </AspectRatio>
                            {/*<Box sx={{display: 'flex'}} justifyContent={"center"}>*/}
                                <Stack direction={"row"} spacing={{ xs: 1, sm: 2 }} justifyContent="space-evenly" divider={<Divider orientation="vertical"/>}>
                                    <Item>
                                        <Typography level="body3">Current: </Typography>
                                        <Typography fontSize="lg" fontWeight="lg">
                                            $2,900
                                        </Typography>
                                    </Item>
                                    <Item>
                                        <Typography level="body3">Local Min: </Typography>
                                        <Typography fontSize="lg" fontWeight="lg">
                                            $2,900
                                        </Typography>
                                    </Item>
                                <Item>
                                    <Typography level="body3">Local Max: </Typography>
                                    <Typography fontSize="lg" fontWeight="lg">
                                        $2,900
                                    </Typography>
                                </Item>
                        </Stack>
                    {/*</Box>*/}
                </Card>
            </Layout.Main>
        </Layout.Root>
</CssVarsProvider>
</RecoilRoot>
)
    ;
}
