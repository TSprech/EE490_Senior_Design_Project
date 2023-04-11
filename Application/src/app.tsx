// import * as ReactDOM from 'react-dom';
import {createRoot} from "react-dom/client";

function render() {
    const root = createRoot(document.getElementById("root") as HTMLElement); // Thanks: https://stackoverflow.com/questions/71668256/deprecation-notice-reactdom-render-is-no-longer-supported-in-react-18
    root.render(
        <div>
            <h2>Hello from React!</h2>
            <button onClick={() => window.LED.TurnOn()}>Hi</button>
        </div>
    );
}

render();