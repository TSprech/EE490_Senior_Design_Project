import './index.css';

console.log('👋 This message is being logged by "renderer.js", included via webpack');

import { createRoot } from 'react-dom/client';
import RenderIndex from './App'

const container = document.getElementById('root') as HTMLElement;
const root = createRoot(container);
// root.render(<Form />);
root.render(<RenderIndex />);
// root.render(<><p>Test</p></>);

// calling IPC exposed from preload script
// window.electron.ipcRenderer.once('ipc-example', (arg) => {
    // eslint-disable-next-line no-console
    // console.log(arg);
// });
// window.electron.ipcRenderer.sendMessage('ipc-example', ['ping']);

// setTimeout(window.RendererIPC.DoneFirstRender, 2000); // Waits until root.render has fully rendered this is because recoil nexus requires a full first render before trying to alter any states https://github.com/luisanton-io/recoil-nexus/issues/20

// setTimeout(() => setRecoil(JSON_Data_RX, "HELLO"), 2000);