import * as React from 'react';
import { CssVarsProvider, useColorScheme } from '@mui/joy/styles';
import CssBaseline from '@mui/joy/CssBaseline';
import Autocomplete from '@mui/joy/Autocomplete';
import Avatar from '@mui/joy/Avatar';
import Box from '@mui/joy/Box';
import Chip from '@mui/joy/Chip';
import ChipDelete from '@mui/joy/ChipDelete';
import Typography from '@mui/joy/Typography';
import Input from '@mui/joy/Input';
import IconButton from '@mui/joy/IconButton';
import Button from '@mui/joy/Button';
import List from '@mui/joy/List';
import ListSubheader from '@mui/joy/ListSubheader';
import Divider from '@mui/joy/Divider';
import ListItem from '@mui/joy/ListItem';
import ListItemButton from '@mui/joy/ListItemButton';
import ListItemDecorator from '@mui/joy/ListItemDecorator';
import ListItemContent from '@mui/joy/ListItemContent';
import RadioGroup from '@mui/joy/RadioGroup';
import Radio from '@mui/joy/Radio';
import Slider from '@mui/joy/Slider';
import Sheet from '@mui/joy/Sheet';

// Icons import
import PeopleRoundedIcon from '@mui/icons-material/PeopleRounded';
import AssignmentIndRoundedIcon from '@mui/icons-material/AssignmentIndRounded';
import ArticleRoundedIcon from '@mui/icons-material/ArticleRounded';
import SearchRoundedIcon from '@mui/icons-material/SearchRounded';
import DarkModeRoundedIcon from '@mui/icons-material/DarkModeRounded';
import LightModeRoundedIcon from '@mui/icons-material/LightModeRounded';
import GridViewRoundedIcon from '@mui/icons-material/GridViewRounded';
import KeyboardArrowDownRoundedIcon from '@mui/icons-material/KeyboardArrowDownRounded';
import KeyboardArrowUpRoundedIcon from '@mui/icons-material/KeyboardArrowUpRounded';
import MenuIcon from '@mui/icons-material/Menu';
import KeyboardArrowRightRoundedIcon from '@mui/icons-material/KeyboardArrowRightRounded';
import GroupRoundedIcon from '@mui/icons-material/GroupRounded';
import LinkIcon from '@mui/icons-material/Link';
import RefreshIcon from '@mui/icons-material/Refresh';

// custom
import teamTheme from './theme';
import Menu from './components/Menu';
import Layout from './components/Layout';
import {Select, Option} from "@mui/joy";

function ColorSchemeToggle() {
  const { mode, setMode } = useColorScheme();
  const [mounted, setMounted] = React.useState(false);
  React.useEffect(() => {
    setMounted(true);
  }, []);
  if (!mounted) {
    return <IconButton size="sm" variant="outlined" color="primary" />;
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
      {mode === 'light' ? <DarkModeRoundedIcon /> : <LightModeRoundedIcon />}
    </IconButton>
  );
}

function TeamNav() {
  return (
    <List size="sm" sx={{ '--ListItem-radius': '8px', '--List-gap': '4px' }}>
      <ListItem nested>
        <ListSubheader>
          Browse
          <IconButton
            size="sm"
            variant="plain"
            color="primary"
            sx={{ '--IconButton-size': '24px', ml: 'auto' }}
          >
            <KeyboardArrowDownRoundedIcon fontSize="small" color="primary" />
          </IconButton>
        </ListSubheader>
        <List
          aria-labelledby="nav-list-browse"
          sx={{
            '& .JoyListItemButton-root': { p: '8px' },
          }}
        >
          <ListItem>
            <ListItemButton variant="soft" color="primary">
              <ListItemDecorator sx={{ color: 'inherit' }}>
                <PeopleRoundedIcon fontSize="small" />
              </ListItemDecorator>
              <ListItemContent>People</ListItemContent>
            </ListItemButton>
          </ListItem>
          <ListItem>
            <ListItemButton>
              <ListItemDecorator sx={{ color: 'neutral.500' }}>
                <AssignmentIndRoundedIcon fontSize="small" />
              </ListItemDecorator>
              <ListItemContent>Managing accounts</ListItemContent>
            </ListItemButton>
          </ListItem>
          <ListItem>
            <ListItemButton>
              <ListItemDecorator sx={{ color: 'neutral.500' }}>
                <ArticleRoundedIcon fontSize="small" />
              </ListItemDecorator>
              <ListItemContent>Policies</ListItemContent>
              <Chip
                variant="soft"
                color="info"
                size="sm"
                sx={{ borderRadius: 'sm' }}
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

export default function TeamExample() {
  const [drawerOpen, setDrawerOpen] = React.useState(false);
  const [ports, setPorts] = React.useState([]);

  async function updatePorts(e) {
    // e.preventDefault();
    try {
      const ret_ports = await window.SerialIPC.ListIPC();
      setPorts(ret_ports);
    } catch (err) {
      alert(err);
    }
  }

  function SerialList(){
    const list_items = ports.map(item =>
        <Option value={item.path} key={item.path}>
          {item.friendlyName}
        </Option>
    );
    return (
        <Select
            color="primary"
            placeholder="Select Port"
            variant="outlined"
        >
          {list_items}
        </Select>
    );
  }


  return (
    <CssVarsProvider disableTransitionOnChange theme={teamTheme}>
      <CssBaseline />
      {drawerOpen && (
        <Layout.SideDrawer onClose={() => setDrawerOpen(false)}>
          {/*<TeamNav />*/}
        </Layout.SideDrawer>
      )}
      <Layout.Root
        sx={{
          ...(drawerOpen && {
            height: '100vh',
            overflow: 'hidden',
          }),
        }}
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
              variant="outlined"
              size="sm"
              onClick={() => setDrawerOpen(true)}
              sx={{ display: { sm: 'none' } }}
            >
              <MenuIcon />
            </IconButton>
            <IconButton
              size="sm"
              variant="solid"
              sx={{ display: { xs: 'none', sm: 'inline-flex' } }}
            >
              <GroupRoundedIcon />
            </IconButton>
            <Typography component="h1" fontWeight="xl">
              Electron Joy
            </Typography>
          </Box>
          {/*<Input*/}
          {/*  size="sm"*/}
          {/*  placeholder="Search anything…"*/}
          {/*  startDecorator={<SearchRoundedIcon color="primary" />}*/}
          {/*  endDecorator={*/}
          {/*    <IconButton variant="outlined" size="sm" color="neutral">*/}
          {/*      <Typography fontWeight="lg" fontSize="sm" textColor="text.tertiary">*/}
          {/*        /*/}
          {/*      </Typography>*/}
          {/*    </IconButton>*/}
          {/*  }*/}
          {/*  sx={{*/}
          {/*    flexBasis: '500px',*/}
          {/*    display: {*/}
          {/*      xs: 'none',*/}
          {/*      sm: 'flex',*/}
          {/*    },*/}
          {/*  }}*/}
          {/*/>*/}
          <Box sx={{ display: 'flex', flexDirection: 'row', gap: 1.5 }}>
            <IconButton
              size="sm"
              variant="outlined"
              color="primary"
              sx={{ display: { xs: 'inline-flex', sm: 'none' } }}
            >
              <SearchRoundedIcon />
            </IconButton>

            <SerialList />

            <IconButton
              size="sm"
              variant="outlined"
              color="primary"
              component="a"
              onClick={updatePorts}
            > <RefreshIcon /> </IconButton>
            {/*<Menu*/}
            {/*  id="app-selector"*/}
            {/*  control={*/}
            {/*    <IconButton*/}
            {/*      size="sm"*/}
            {/*      variant="outlined"*/}
            {/*      color="primary"*/}
            {/*      aria-label="Apps"*/}
            {/*    >*/}
            {/*      <GridViewRoundedIcon />*/}
            {/*    </IconButton>*/}
            {/*  }*/}
            {/*  menus={[*/}
            {/*    {*/}
            {/*      label: 'Email',*/}
            {/*      href: '/joy-ui/getting-started/templates/email/',*/}
            {/*    },*/}
            {/*    {*/}
            {/*      label: 'Team',*/}
            {/*      active: true,*/}
            {/*      href: '/joy-ui/getting-started/templates/team/',*/}
            {/*      'aria-current': 'page',*/}
            {/*    },*/}
            {/*    {*/}
            {/*      label: 'Files',*/}
            {/*      href: '/joy-ui/getting-started/templates/files/',*/}
            {/*    },*/}
            {/*  ]}*/}
            {/*/>*/}
            <ColorSchemeToggle />
          </Box>
        </Layout.Header>
        <Layout.Main>
        </Layout.Main>
      </Layout.Root>
    </CssVarsProvider>
  );
}
