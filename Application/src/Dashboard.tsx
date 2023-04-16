import * as React from 'react';
import {createTheme, ThemeProvider} from '@mui/material/styles';
import Toolbar from '@mui/material/Toolbar';
import Typography from '@mui/material/Typography';
// import Chart from './Chart';
import {AppBar, FormControl, InputLabel, MenuItem, Select} from "@mui/material";

const mdTheme = createTheme();

function DashboardContent() {
  const [open, setOpen] = React.useState(true);

  return (
    <ThemeProvider theme={mdTheme}>
      {/*<Box sx={{ display: 'flex' }}>*/}
      {/*  <CssBaseline />*/}
        <AppBar position="static">
          <Toolbar>
            <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
              News
            </Typography>
          <FormControl variant="standard" sx={{ m: 1, minWidth: 120 }}>
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

export default function Dashboard() {
  return <DashboardContent />;
}
