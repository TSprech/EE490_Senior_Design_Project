// import * as ReactDOM from 'react-dom';
// import * as React from "react";
import {useState} from "react";
import './Deposits'
import {createTheme, ThemeProvider} from "@mui/material/styles";
import {AppBar, FormControl, InputLabel, MenuItem, Select} from "@mui/material";
import Toolbar from "@mui/material/Toolbar";
import Typography from "@mui/material/Typography";

const mdTheme = createTheme();


// class SerialListManager {
//     static ports = []
//     constructor() {
//         SerialListManager.UpdatePorts()
//     }
//
//     static async UpdatePorts() {
//         SerialListManager.ports = await window.SerialIPC.ListIPC();
//     }
//
//     static ListPorts() {
//         return SerialListManager.ports;
//     }
// }

// function SerialList(){
//     // const data = await window.SerialIPC.ListIPC();
//     //     const list_items = port_data.map(item =>
//     const port_data = SerialListManager.ListPorts();
//     const list_items = port_data.map(item =>
//         <li key={item.path}>
//             {item.friendlyName}
//         </li>
//     );
//     console.log("New port data: ");
//     console.log(port_data);
//     return (
//         <ul>{list_items}</ul>
//     );
// }

export default function Index() {
    const [answer, setAnswer] = useState('');
    const [ports, setPorts] = useState([]);
    const [error, setError] = useState(null);
    const [status, setStatus] = useState('typing');

    if (status === 'success') {
        return <h1>That's right!</h1>
    }

    function SerialList() {
        const list_items = ports.map(item =>
            <li key={item.path}>
                {item.friendlyName}
            </li>
        );
        console.log("New port data: ");
        console.log(ports);
        return (
            <ul>{list_items}</ul>
        );
    }

    async function handleSubmit(e) {
        e.preventDefault();
        setStatus('submitting');
        try {
            await submitForm(answer);
            setStatus('success');
        } catch (err) {
            setStatus('typing');
            setError(err);
        }
    }

    async function updatePorts(e) {
        e.preventDefault();
        try {
            const ret_ports = await window.SerialIPC.ListIPC();
            setPorts(ret_ports);
        } catch (err) {
            setError(err);
        }
    }

    function handleTextareaChange(e) {
        setAnswer(e.target.value);
    }

    return (
        <ThemeProvider theme={mdTheme}>
            {/*<Box sx={{ display: 'flex' }}>*/}
            {/*  <CssBaseline />*/}
            <AppBar position="static">
                <Toolbar>
                    <Typography variant="h6" component="div" sx={{flexGrow: 1}}>
                        News
                    </Typography>
                    <FormControl variant="standard" sx={{m: 1, minWidth: 120}}>
                        <InputLabel id="demo-simple-select-standard-label">Age</InputLabel>
                        <Select
                            labelId="demo-simple-select-standard-label"
                            id="demo-simple-select-standard"
                            value='10'
                            // onChange={}
                            label="Age"
                        >
                            <MenuItem value="">
                                <em>None</em>
                            </MenuItem>
                            <MenuItem value={10}>Ten</MenuItem>
                            <MenuItem value={20}>Twenty</MenuItem>
                            <MenuItem value={30}>Thirty</MenuItem>
                        </Select>
                    </FormControl>
                </Toolbar>
            </AppBar>
        </ThemeProvider>
    );
}

function submitForm(answer) {
    // Pretend it's hitting the network.
    return new Promise((resolve, reject) => {
        setTimeout(() => {
            let shouldError = answer.toLowerCase() !== 'lima'
            if (shouldError) {
                reject(new Error('Good guess but a wrong answer. Try again!'));
            } else {
                resolve();
            }
        }, 1500);
    });
}