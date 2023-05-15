// TSprech 15/05/2023

import {createInterprocess} from "interprocess";

export const {ipcMain, ipcRenderer, exposeApiToGlobalWindow} =
    createInterprocess({
        main: {
            async getPing(_, data: 'ping') {
                const message = `from renderer: ${data} on main process`

                console.log(message)

                return message
            },
        },

        renderer: {
            async getPong(_, data: 'pong') {
                const message = `from main: ${data} on renderer process`

                console.log(message)

                return message
            },
        },
    })