// import './index.css';
// import {createRoot} from 'react-dom/client';


// var lastDate = 0;
// var data = []
// var TICKINTERVAL = 86400000
// let XAXISRANGE = 777600000
// function getDayWiseTimeSeries(baseval, count, yrange) {
//     var i = 0;
//     while (i < count) {
//         var x = baseval;
//         var y = Math.floor(Math.random() * (yrange.max - yrange.min + 1)) + yrange.min;
//
//         data.push({
//             x, y
//         });
//         lastDate = baseval
//         baseval += TICKINTERVAL;
//         i++;
//     }
// }
//
// getDayWiseTimeSeries(new Date('11 Feb 2017 GMT').getTime(), 10, {
//     min: 10,
//     max: 90
// })
//
// function getNewSeries(baseval, yrange) {
//     var newDate = baseval + TICKINTERVAL;
//     lastDate = newDate
//
//     for(var i = 0; i< data.length - 10; i++) {
//         // IMPORTANT
//         // we reset the x and y of the data which is out of drawing area
//         // to prevent memory leaks
//         data[i].x = newDate - XAXISRANGE - TICKINTERVAL
//         data[i].y = 0
//     }
//
//     data.push({
//         x: newDate,
//         y: Math.floor(Math.random() * (yrange.max - yrange.min + 1)) + yrange.min
//     })
// }
//
// function resetData(){
//     // Alternatively, you can also reset the data at certain intervals to prevent creating a huge series
//     data = data.slice(data.length - 10, data.length);
// }
//
// // Replace Math.random() with a pseudo-random number generator to get reproducible results in e2e tests
// // Based on https://gist.github.com/blixt/f17b47c62508be59987b
// var _seed = 42;
// Math.random = function() {
//     _seed = _seed * 16807 % 2147483647;
//     return (_seed - 1) / 2147483646;
// };
//
// class ApexChart extends React.Component {
//     constructor(props) {
//         super(props);
//
//         this.state = {
//
//             series: [{
//                 data: data.slice()
//             }],
//             options: {
//                 chart: {
//                     id: 'realtime',
//                     height: 350,
//                     type: 'line',
//                     animations: {
//                         enabled: true,
//                         easing: 'linear',
//                         dynamicAnimation: {
//                             speed: 1000
//                         }
//                     },
//                     toolbar: {
//                         show: false
//                     },
//                     zoom: {
//                         enabled: false
//                     }
//                 },
//                 dataLabels: {
//                     enabled: false
//                 },
//                 stroke: {
//                     curve: 'smooth'
//                 },
//                 title: {
//                     text: 'Dynamic Updating Chart',
//                     align: 'left'
//                 },
//                 markers: {
//                     size: 0
//                 },
//                 xaxis: {
//                     type: 'datetime',
//                     range: XAXISRANGE,
//                 },
//                 yaxis: {
//                     max: 100
//                 },
//                 legend: {
//                     show: false
//                 },
//             },
//
//
//         };
//     }
//
//
//     componentDidMount() {
//         window.setInterval(() => {
//             getNewSeries(lastDate, {
//                 min: 10,
//                 max: 90
//             })
//
//             ApexCharts.exec('realtime', 'updateSeries', [{
//                 data: data
//             }])
//         }, 1000)
//     }
//
//
//     render() {
//         return (
//             <div>
//                 <div id="chart">
//                     <ReactApexChart options={this.state.options} series={this.state.series} type="line" height={350} />
//                 </div>
//                 <div id="html-dist"></div>
//             </div>
//         );
//     }
// }

// import RenderIndex from "./App";

console.log('👋 This message is being logged by "renderer.js", included via webpack');

const { invoke, handle } = window.api

invoke.getPing('ping')
handle.getPong()

// const container = document.getElementById('root') as HTMLElement;
// const root = createRoot(container);
// root.render(<RenderIndex />);


// calling IPC exposed from preload script
// window.electron.ipcRenderer.once('ipc-example', (arg) => {
    // eslint-disable-next-line no-console
    // console.log(arg);
// });
// window.electron.ipcRenderer.sendMessage('ipc-example', ['ping']);

// setTimeout(window.RendererIPC.DoneFirstRender, 2000); // Waits until root.render has fully rendered this is because recoil nexus requires a full first render before trying to alter any states https://github.com/luisanton-io/recoil-nexus/issues/20

// setTimeout(() => setRecoil(JSON_Data_RX, "HELLO"), 2000);