// import * as ReactDOM from 'react-dom';
import {createRoot} from "react-dom/client";
import {useState} from "react";

// const filePath = await window.electronAPI.openFile()

function func() {
    // window.SerialIPC.ListIPC().then((port_names) => console.log(port_names));
    let ports;
    window.SerialIPC.ListIPC().then((port_names) => console.log(port_names));

    return ports;
}

let port_data = [
    {
        path: 'COM1',
        manufacturer: '(Standard port types)',
        serialNumber: undefined,
        pnpId: 'ACPI\\PNP0501\\0',
        locationId: undefined,
        friendlyName: 'Communications Port (COM1)',
        vendorId: undefined,
        productId: undefined
    },
    {
        path: 'COM4',
        manufacturer: 'Microsoft',
        serialNumber: undefined,
        pnpId: 'BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_VID&000100CD_PID&0123\\7&AD7B2F8&0&0006668D9155_C00000000',
        locationId: undefined,
        friendlyName: 'Standard Serial over Bluetooth link (COM4)',
        vendorId: undefined,
        productId: undefined
    },
    {
        path: 'COM3',
        manufacturer: 'Microsoft',
        serialNumber: undefined,
        pnpId: 'BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&0000\\7&AD7B2F8&0&000000000000_00000000',
        locationId: undefined,
        friendlyName: 'Standard Serial over Bluetooth link (COM3)',
        vendorId: undefined,
        productId: undefined
    }
]


class SerialListManager {
    static ports = []
    constructor() {
        SerialListManager.UpdatePorts()
    }

    static async UpdatePorts() {
        SerialListManager.ports = await window.SerialIPC.ListIPC();
    }

    static ListPorts() {
        return SerialListManager.ports;
    }
}

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


// function Index() {
//     const [ports, setPorts] = useState([]);
//     //
//     async function handlePortUpdate(e) {
//         e.preventDefault();
//         try {
//             const ret_ports = await window.SerialIPC.ListIPC();
//             setPorts(ret_ports);
//         } catch (err) {
//         }
//     }
//
//     function SerialList(){
//         const list_items = ports.map(item =>
//             <li key={item.path}>
//                 {item.friendlyName}
//             </li>
//         );
//         console.log("New port data: ");
//         console.log(ports);
//         return (
//             <ul>{list_items}</ul>
//         );
//     }
//
//     return (<div>
//         <h2>Hello from React!</h2>
//         <button onClick={() => SerialListManager.UpdatePorts()}>Hi</button>
//
//     </div>);
// }

export default function Form() {
    const [answer, setAnswer] = useState('');
    const [ports, setPorts] = useState([]);
    const [error, setError] = useState(null);
    const [status, setStatus] = useState('typing');

    if (status === 'success') {
        return <h1>That's right!</h1>
    }

    function SerialList(){
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
        <>
            <h2>City quiz</h2>
            <p>
                In which city is there a billboard that turns air into drinkable water?
            </p>
            <button onClick={updatePorts} >Update Port List!</button>
            <SerialList />
            <form onSubmit={handleSubmit}>
        <textarea
            value={answer}
            onChange={handleTextareaChange}
            disabled={status === 'submitting'}
        />
                <br />
                <button disabled={
                    answer.length === 0 ||
                    status === 'submitting'
                }>
                    Submit
                </button>
                {error !== null &&
                    <p className="Error">
                        {error.message}
                    </p>
                }
            </form>
        </>
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