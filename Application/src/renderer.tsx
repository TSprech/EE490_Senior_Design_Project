import './index.css';

console.log('👋 This message is being logged by "renderer.js", included via webpack');

import Form from './app';
import { createRoot } from 'react-dom/client';

const container = document.getElementById('root') as HTMLElement;
const root = createRoot(container);
root.render(<Form />);

// calling IPC exposed from preload script
// window.electron.ipcRenderer.once('ipc-example', (arg) => {
    // eslint-disable-next-line no-console
    // console.log(arg);
// });
// window.electron.ipcRenderer.sendMessage('ipc-example', ['ping']);
