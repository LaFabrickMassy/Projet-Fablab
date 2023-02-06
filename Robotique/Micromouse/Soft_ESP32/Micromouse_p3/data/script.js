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
    
    // console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        if (key == "speed"){
            speed = myObj[key];
			document.getElementById("speed").innerHTML = speed;
        }
        if (key == "turn"){
            turn = myObj[key];
			document.getElementById("turn").innerHTML = turn;
        }
        if (key == "x"){
            pos_x = myObj[key];
			document.getElementById("pos_x").innerHTML = "x: "+pos_x;
        }
        if (key == "y"){
            pos_y = myObj[key];
			document.getElementById("pos_y").innerHTML = "y: "+pos_y;
        }
        if (key == "h"){
            heading = myObj[key];
			document.getElementById("heading").innerHTML = "h: "+heading;
        }
        if (key == "eL"){
            enc_L = myObj[key];
			document.getElementById("enc_L").innerHTML = "EncL: "+enc_L;
        }
        if (key == "eR"){
            enc_R = myObj[key];
			document.getElementById("enc_R").innerHTML = "EncR: "+enc_R;
        }
        if (key == "sL"){
            sens_L = myObj[key];
			document.getElementById("sens_L").innerHTML = "dL: <br>"+sens_L;
        }
        if (key == "sSL"){
            sens_SL = myObj[key];
			document.getElementById("sens_SL").innerHTML = "dSL: <br>"+sens_SL;
        }
        if (key == "sF"){
            sens_F = myObj[key];
			document.getElementById("sens_F").innerHTML = "dF: <br>"+sens_F;
        }
        if (key == "sSR"){
            sens_SR = myObj[key];
			document.getElementById("sens_SR").innerHTML = "dSR: <br>"+sens_SR;
        }
        if (key == "sR"){
            sens_R = myObj[key];
			document.getElementById("sens_R").innerHTML = "dR: <br>"+sens_R;
        }
        if (key == "kp"){
			document.getElementById("kp").innerHTML = "kp:<br>"+myObj["kp"];
        }
        if (key == "ki"){
			document.getElementById("ki").innerHTML = "ki:<br>"+myObj["ki"];
        }
        if (key == "kd"){
			document.getElementById("kd").innerHTML = "kd:<br>"+myObj["kd"];
        }
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
    document.getElementById('but_menu_run').addEventListener('click', click_but_menu_run);
    document.getElementById('but_menu_pid').addEventListener('click', click_but_menu_pid);
    document.getElementById('but_kp_up').addEventListener('click', click_but_kp_up);
    document.getElementById('but_ki_up').addEventListener('click', click_but_ki_up);
    document.getElementById('but_kd_up').addEventListener('click', click_but_kd_up);
    document.getElementById('but_kp_dn').addEventListener('click', click_but_kp_dn);
    document.getElementById('but_ki_dn').addEventListener('click', click_but_ki_dn);
    document.getElementById('but_kd_db').addEventListener('click', click_but_kd_dn);

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

function click_but_kp_up(){
    websocket.send('kpup');
}

function click_but_ki_up(){
    websocket.send('kiup');
}

function click_but_kd_up(){
    websocket.send('kdup');
}

function click_but_kp_dn(){
    websocket.send('kpdn');
}

function click_but_ki_dn(){
    websocket.send('kidn');
}

function click_but_kd_dn(){
    websocket.send('kddn');
}
