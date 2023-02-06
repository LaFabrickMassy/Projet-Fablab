// Javascript for PID calibration

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
	var val_kp;
	var val_ki;
	var val_kd;
    
	console.log("PID_Cal onMessage");
    console.log(event.data);

    // console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        if (key == "kp") {
            val = myObj[key];
			document.getElementById("val_"+key).innerHTML = val;
        }
        if (key == "ki") {
            val = myObj[key];
			document.getElementById("val_"+key).innerHTML = val;
        }
        if (key == "kd") {
            val = myObj[key];
			document.getElementById("val_"+key).innerHTML = val;
        }
    }
}

function initButton() {
    console.log("function initButton");
    document.getElementById('but_start').addEventListener('click', (e) => {
		websocket.send("PC_start");
	});
    document.getElementById('but_stop').addEventListener('click', (e) => {
		websocket.send("PC_stop");
	});
    document.getElementById('but_index').addEventListener('click', (e) => {
		window.open("/index.html", "_self");
		websocket.send("PC_home");
	});
    document.getElementById('but_kp_up').addEventListener('click', (e) => {
		websocket.send("PC_kp_up");
	});
    document.getElementById('but_ki_up').addEventListener('click', (e) => {
		websocket.send("PC_ki_up");
	});
    document.getElementById('but_kd_up').addEventListener('click', (e) => {
		websocket.send("PC_kd_up");
	});
    document.getElementById('but_kp_dn').addEventListener('click', (e) => {
		websocket.send("PC_kp_dn");
	});
    document.getElementById('but_ki_dn').addEventListener('click', (e) => {
		websocket.send("PC_ki_dn");
	});
    document.getElementById('but_kd_dn').addEventListener('click', (e) => {
		websocket.send("PC_kd_dn");
	});
}

