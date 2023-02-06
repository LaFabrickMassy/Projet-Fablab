// Complete project details: https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function onLoad(event) {
    console.log('onLoad function');
    initWebSocket();
    initButton();
}

function getValues(){
    websocket.send("getValues");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    var speed;
    var turn;
    var pos_x;
    var pos_y;
    var heading;
    var enc_L;
    var enc_R;
    
	console.log("Index onMessage");
    console.log(event.data);
}

function initButton() {
    console.log("function initButton");
    document.getElementById('but_menu_run').addEventListener('click', click_simple_run);
    document.getElementById('but_menu_pid').addEventListener('click', click_pid_cal);
}

function click_simple_run(){
	console.log('click_simple_run');
    websocket.send('IDX_SR');
	window.open("/simple_run.html", "_self");
}

function click_pid_cal(){
	console.log('click_pid_cal');
    websocket.send('IDX_PC');
	window.open("pid_cal.html", "_self");
}

