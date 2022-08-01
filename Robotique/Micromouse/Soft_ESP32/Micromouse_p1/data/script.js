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
    
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        if (key == "speed"){
            speed = myObj[key];
        }
        if (key == "turn"){
            turn = myObj[key];
        }
        if (key == "x"){
            pos_x = myObj[key];
        }
        if (key == "y"){
            pos_y = myObj[key];
        }
        if (key == "h"){
            heading = myObj[key];
        }
        if (key == "eL"){
            enc_L = myObj[key];
        }
        if (key == "eR"){
            enc_R = myObj[key];
        }
        if (key == "sL"){
            sens_L = myObj[key];
        }
        if (key == "sSL"){
            sens_SL = myObj[key];
        }
        if (key == "sF"){
            sens_F = myObj[key];
        }
        if (key == "sSR"){
            sens_SR = myObj[key];
        }
        if (key == "sR"){
            sens_R = myObj[key];
        }
        document.getElementById("speed").innerHTML = speed;
        document.getElementById("turn").innerHTML = turn;
        document.getElementById("pos_y").innerHTML = "x: "+pos_x;
        document.getElementById("pos_x").innerHTML = "y: "+pos_y;
        document.getElementById("heading").innerHTML = "h: "+heading;
        document.getElementById("enc_L").innerHTML = "EncL: "+enc_L;
        document.getElementById("enc_R").innerHTML = "EncR: "+enc_R;
        document.getElementById("sens_L").innerHTML = "dL: "+sens_L;
        document.getElementById("sens_SL").innerHTML = "dSL: "+sens_SL;
        document.getElementById("sens_F").innerHTML = "dF: "+sens_F;
        document.getElementById("sens_SR").innerHTML = "dSR: "+sens_SR;
        document.getElementById("sens_R").innerHTML = "dR: "+sens_R;
    }
}

function initButton() {
    console.log("function initButton");
    document.getElementById('button_up').addEventListener('click', click_button_up);
    document.getElementById('button_down').addEventListener('click', click_button_down);
    document.getElementById('button_left').addEventListener('click', click_button_left);
    document.getElementById('button_right').addEventListener('click', click_button_right);
    document.getElementById('button_stop').addEventListener('click', click_button_stop);
    document.getElementById('button_home').addEventListener('click', click_button_home);
}

function click_button_up(){
    websocket.send('bup');
}

function click_button_down(){
    websocket.send('bdn');
}

function click_button_left(){
    websocket.send('blt');
}

function click_button_right(){
    websocket.send('brt');
}

function click_button_stop(){
    websocket.send('bst');
}

function click_button_home(){
    websocket.send('bhm');
}

