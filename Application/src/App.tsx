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
import {AspectRatio, Button, Card, Divider, Grid, Stack, styled} from "@mui/joy";
import Typography from "@mui/joy/Typography";
import {BookmarkAdd} from "@mui/icons-material";
import Box from "@mui/joy/Box";
import Sheet from "@mui/joy/Sheet";

import ReactApexChart from 'apexcharts'
import Chart from "react-apexcharts";
import {Component} from "react";

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


let lastDate = 0;
let data: {x: number, y: number}[] = []
// const TICKINTERVAL = 86400000 // Number of ms in a day (I think? It was a magic number)
const TICKINTERVAL = 1 // Number of ms in a day (I think? It was a magic number)
const NUMBEROFPOINTS = 20
const XAXISRANGE = TICKINTERVAL * NUMBEROFPOINTS

function resetData(){
    // Alternatively, you can also reset the data at certain intervals to prevent creating a huge series
    data = data.slice(data.length - NUMBEROFPOINTS, data.length);
}

function getNewSeries(baseval, yrange) {
    const newDate = baseval + TICKINTERVAL;
    lastDate = newDate

    for(var i = 0; i< data.length - NUMBEROFPOINTS; i++) {
        // IMPORTANT
        // we reset the x and y of the data which is out of drawing area
        // to prevent memory leaks
        data[i].x = newDate - XAXISRANGE - TICKINTERVAL
        data[i].y = 0
    }

    data.push({
        x: newDate,
        y: Math.floor(Math.random() * (yrange.max - yrange.min + 1)) + yrange.min
    })
}

class App extends Component {

    constructor(props: any) {
        super(props);

        this.updateCharts = this.updateCharts.bind(this);

        this.state = {

            series: [{
                data: data.slice()
            }],
            options: {
                chart: {
                    id: 'realtime',
                    type: 'line',
                    animations: {
                        enabled: true,
                        easing: 'linear',
                        dynamicAnimation: {
                            speed: 100
                        }
                    },
                    toolbar: {
                        show: false
                    },
                    zoom: {
                        enabled: false
                    }
                },
                dataLabels: {
                    enabled: false
                },
                stroke: {
                    curve: 'smooth'
                },
                markers: {
                    size: 0
                },
                xaxis: {
                    type: 'numeric',
                    range: XAXISRANGE,
                },
                yaxis: {
                    max: 100
                },
                legend: {
                    show: false
                },
            },


        };
    }

    updateCharts() {
        getNewSeries(lastDate, {
            min: 10,
            max: 90
        })

        ApexCharts.exec('realtime', 'updateSeries', [{
            data: data
        }])
    }

    render() {
        return (
            // <div className="app">
            //     <div className="row">
            //         <div className="mixed-chart">
            <>
                <button onClick={this.updateCharts}>Update!</button>
                <Chart options={this.state.options} series={this.state.series} type="line" />
            </>
            //         </div>
            //     </div>
            // </div>
        );
    }
}

function BasicCard() {
    return (
        // <Card variant="outlined" sx={{ width: 320 }}>
        <Card variant="outlined">
            <Typography level="h2" fontSize="md" sx={{ mb: 0.5 }}>
                Yosemite National Park
            </Typography>
            <Typography level="body2">April 24 to May 02, 2021</Typography>
            <IconButton
                aria-label="bookmark Bahamas Islands"
                variant="plain"
                color="neutral"
                size="sm"
                sx={{ position: 'absolute', top: '0.5rem', right: '0.5rem' }}
            >
                <BookmarkAdd />
            </IconButton>
            {/*<AspectRatio minHeight="200px" maxHeight="200px" sx={{ my: 2 }}>*/}
            {/*<AspectRatio ratio={'1618 / 1000'} sx={{ my: 2 }}> /!* Ratio as defined in ApexChart's docs: https://apexcharts.com/docs/options/chart/height/*!/*/}
                <App/>
            {/*</AspectRatio>*/}
            <Box sx={{ display: 'flex' }}>
                <div>
                    <Typography level="body3">Total price:</Typography>
                    <Typography fontSize="lg" fontWeight="lg">
                        $2,900
                    </Typography>
                </div>
                <Button
                    variant="solid"
                    size="sm"
                    color="primary"
                    aria-label="Explore Bahamas Islands"
                    sx={{ ml: 'auto', fontWeight: 600 }}
                >
                    Explore
                </Button>
            </Box>
        </Card>
    );
}

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
                        <Grid
                            container
                            spacing={{ xs: 2, md: 3 }}
                            columns={{ xs: 2, sm: 8, md: 12 }}
                            sx={{ flexGrow: 1 }}
                        >
                            {Array.from(Array(6)).map((_, index) => (
                                <Grid xs={2} sm={4} md={4} key={index}>
                                    {/*<Item>xs=2</Item>*/}
                                    <BasicCard/>
                                </Grid>
                            ))}
                        </Grid>
                {/*        /!*<Card variant="outlined" sx={{width: 420}}>*!/*/}
                {/*        <Card variant="outlined" sx={{width: 420}}>*/}
                {/*            <Typography level="h2" fontSize="md" sx={{mb: 0.5}}>*/}
                {/*                Voltage In (V)*/}
                {/*            </Typography>*/}
                {/*            <AspectRatio minHeight="15rem" maxHeight="16rem" sx={{my: 2}}>*/}
                {/*                /!*<ApexChart/>*!/*/}
                {/*                <App/>*/}
                {/*            </AspectRatio>*/}
                {/*            /!*<Box sx={{display: 'flex'}} justifyContent={"center"}>*!/*/}
                {/*                <Stack direction={"row"} spacing={{ xs: 1, sm: 2 }} justifyContent="space-evenly" divider={<Divider orientation="vertical"/>}>*/}
                {/*                    <Item>*/}
                {/*                        <Typography level="body3">Current: </Typography>*/}
                {/*                        <Typography fontSize="lg" fontWeight="lg">*/}
                {/*                            $2,900*/}
                {/*                        </Typography>*/}
                {/*                    </Item>*/}
                {/*                    <Item>*/}
                {/*                        <Typography level="body3">Local Min: </Typography>*/}
                {/*                        <Typography fontSize="lg" fontWeight="lg">*/}
                {/*                            $2,900*/}
                {/*                        </Typography>*/}
                {/*                    </Item>*/}
                {/*                <Item>*/}
                {/*                    <Typography level="body3">Local Max: </Typography>*/}
                {/*                    <Typography fontSize="lg" fontWeight="lg">*/}
                {/*                        $2,900*/}
                {/*                    </Typography>*/}
                {/*                </Item>*/}
                {/*        </Stack>*/}
                {/*    /!*</Box>*!/*/}
                {/*</Card>*/}
            </Layout.Main>
        </Layout.Root>
</CssVarsProvider>
</RecoilRoot>
)
    ;
}
