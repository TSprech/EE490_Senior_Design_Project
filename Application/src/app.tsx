// import * as ReactDOM from 'react-dom';
import {useState} from "react";
import './Deposits'

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