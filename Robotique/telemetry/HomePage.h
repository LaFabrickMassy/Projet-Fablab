R"<<<<(
<!doctype html><html lang=fr style=height:100%;overflow:hidden><meta charset=utf-8><meta http-equiv=x-ua-compatible content="IE=edge"><meta name=viewport content="width=device-width,initial-scale=1"><title>Robot C2</title><style>#bottom div{display:inline-block;vertical-align:text-top;height:100%;margin-right:5px}#bottom div p{text-align:right}#output div a{margin-right:24px}#CmdList a{display:block;margin:auto}canvas{display:block}.fill{fill:#000;fill-opacity:1;stroke:none}.stroke{fill:none;stroke:#000;stroke-width:.6;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;stop-color:#000}</style><script>'use strict';var DragOrigX,DragOrigY;const MAX_NORM=75;const MAX_MOTOR=100;const MAX_RATIO=MAX_MOTOR/MAX_NORM;const MAX_SPEED=1;const MS_PER_S=1000;const S1K=0.5;const S2K=0.25;const S3K=0.25;const SQRT2=Math.sqrt(2.0);const INTERVAL=0.250;const PIX_SCALE=100;const ROB_WIDTH=0.06;const ROBOT_PX_SIZE=16;const ROBOT_PX_UNIT=3;const CANVAS_SIZE=2048;const CANVAS_BORDER=8;const MIN_NORM2=5;const HALF_PI=0.5*Math.PI;const LXF=MAX_RATIO*(S1K*SQRT2-1);const LYF=MAX_RATIO;const RXF=MAX_RATIO*(S3K*SQRT2-1);const RYF=MAX_RATIO;const LXT=MAX_RATIO*S2K;const LYT=MAX_RATIO*(S1K*SQRT2-S2K);const RXT=-MAX_RATIO*S2K;const RYT=MAX_RATIO*(S2K+S3K*SQRT2);const COMMANDS=["Arrêt d'urgence",'Retour Maison','Commande 3','Commande 4','Commande 5'];let SpeedL;let SpeedR;let LocX;let LocY;let Frame;let Robot;let Saver;let Robot2DCtx;let Canvas2DCtx;let Angle=-HALF_PI;let CurTimer=null;let ScrollTimer=null;let LastTime;let MotorLeft;let MotorRight;let Ws;class Coordinate{constructor(){this.value=CANVAS_SIZE*0.5;this.dist0=0;this.dist1=0;}
SetOther(other){if(this.other==undefined){this.other=other;other.SetOther(this);}}
SetMaxScroll(clientSize){this.scrollMax=CANVAS_SIZE+2*CANVAS_BORDER-clientSize;if(this.scrollMax<0){this.scrollMax=0;this.scroll=0;}else{this.scroll=this.GetScrollTarget();}}
GetScrollTarget(){let X=this.value/CANVAS_SIZE;return this.scrollMax*X*X*(3-2*X);}
Scroll(){let dist2=this.dist1;this.dist1=this.dist0;this.dist0=this.GetScrollTarget()-this.scroll;this.scroll+=0.02*this.dist0+0.05*this.dist1+0.11*dist2;}
Move(length){this.value+=this.vector*length;}
SideMove(length){this.value+=this.other.vector*length;}
SetVector(angle){this.vector=Math.cos(angle);}}
class Position{constructor(){this.xCoord=new Coordinate(false);this.yCoord=new Coordinate(true);this.yCoord.SetOther(this.xCoord);}
Move(length){this.xCoord.Move(length);this.yCoord.Move(length);}
SideMove(length){this.xCoord.SideMove(-length);this.yCoord.SideMove(length);}
SetAngle(angle){this.xCoord.SetVector(angle);this.yCoord.SetVector(HALF_PI-angle);}
GetValue(){return[this.xCoord.value,this.yCoord.value]}
SetMaxScroll(clientWidth,clientHeight){this.xCoord.SetMaxScroll(clientWidth);this.yCoord.SetMaxScroll(clientHeight);}
Scroll(){this.xCoord.Scroll();this.yCoord.Scroll();}}
let Pos=new Position();Pos.SetAngle(Angle);function OnDragStart(event){document.onmouseup=OnDragStop;document.onmousemove=OnDragPosition;DragOrigX=event.clientX;DragOrigY=event.clientY;ScrollTimer=setInterval(function(){Pos.Scroll();CanvasScroll();},100);}
function OnDragStop(event){clearInterval(ScrollTimer);ScrollTimer=null;SetCursorPosition(0,0);UpdateMotor(0,0);if(CurTimer!=null){clearInterval(CurTimer);CurTimer=null;}
document.onmouseup=null;document.onmousemove=null;}
function OnDragPosition(event){let DeltaX=event.clientX-DragOrigX;let DeltaY=event.clientY-DragOrigY;let Norm=DeltaX*DeltaX+DeltaY*DeltaY;if(Norm<MIN_NORM2){DeltaX=0;DeltaY=0;}else if(Norm>MAX_NORM*MAX_NORM){Norm=MAX_NORM/Math.sqrt(Norm);DeltaX*=Norm;DeltaY*=Norm;}
SetCursorPosition(DeltaX,DeltaY);let NegX=(DeltaX<0);let NegY=(DeltaY<0);if(NegX)
DeltaX=-DeltaX;if(NegY)
DeltaY=-DeltaY;if(DeltaY<=DeltaX){MotorRight=RXT*DeltaX+RYT*DeltaY;MotorLeft=LXT*DeltaX+LYT*DeltaY;}else{MotorLeft=LXF*DeltaX+LYF*DeltaY;MotorRight=RXF*DeltaX+RYF*DeltaY;}
MotorLeft=Math.round(MotorLeft);MotorRight=Math.round(MotorRight);if(NegX)
[MotorLeft,MotorRight]=[MotorRight,MotorLeft];if(!NegY)
[MotorLeft,MotorRight]=[-MotorRight,-MotorLeft];UpdateMotor(MotorLeft,MotorRight);}
function FormatInt(Val,Digits){return Intl.NumberFormat('fr',{minimumIntegerDigits:Digits}).format(Val);}
function UpdateMotor(Left,Right){if((Left==0)&&(Right==0)){if(CurTimer){clearInterval(CurTimer);CurTimer=null;}}else{if(!CurTimer){LastTime=Date.now();CurTimer=setInterval(ComputeTraj,INTERVAL*MS_PER_S);}else
ComputeTraj();}
AddLog("Send:Motor(L= "+MotorLeft+", R= "+MotorRight+")");SpeedL.innerText=FormatInt(MotorLeft,3);SpeedR.innerText=FormatInt(MotorRight,3);Ws.send('M'+SpeedL.innerText+'|'+SpeedR.innerText);}
function SendCommand(Number){AddLog("Send:Command(N= "+Number+")");Ws.send('C'+Number);}
var LogText;var Cursor;function SetCursorPosition(x,y){Cursor.style.top=(MAX_NORM+Math.round(y))+"px";Cursor.style.left=(MAX_NORM+Math.round(x))+"px";}
function AddLog(Text){let Now=new Date();LogText.append(FormatInt(Now.getSeconds(),2)+"."+FormatInt(Now.getMilliseconds(),3)+": "+Text,document.createElement("br"));LogText.scrollTo(0,100000);}
function SaveLog(){let LineAry=[];for(const line in LogText.childNodes){let Elem=LogText.childNodes[line];if(Elem.nodeType==Node.TEXT_NODE){LineAry.push(Elem.textContent+'\n');}}
SaveFile(new Blob(LineAry,{type:'text/log',endings:'native'}),"Robot.log");return false;}
function ClearLog(){LogText.innerHTML='Log:<br/>';return false;}
function OnLoad(){let ElemAry=["LogText","Cursor","Frame","Robot","SpeedL","SpeedR","LocX","LocY","Saver","Canvas","CmdList"]
for(let i in ElemAry){ElemAry[i]=document.getElementById(ElemAry[i]);}
let Canvas,CmdList;[LogText,Cursor,Frame,Robot,SpeedL,SpeedR,LocX,LocY,Saver,Canvas,CmdList]=ElemAry
let WsHost=location.host;if(WsHost=="")WsHost='localhost';WsHost='ws://'+WsHost+':81/';Ws=new WebSocket(WsHost);Ws.binaryType='arraybuffer';Ws.onopen=function(e){AddLog("Etat: Socket est connecté");}
Ws.onmessage=function(Event){if(Event.data instanceof ArrayBuffer){const view=new DataView(Event.data);AddLog('Coord: ('+view.getInt16(0,true)
+', '+view.getInt16(2,true)
+') '+view.getInt16(4,true)
+'/ '+view.getInt16(6,true)
+'[ '+view.getInt16(8,true));}else{AddLog('Etat: '+Event.data);}}
Ws.onclose=function(e){AddLog('Etat: Déconnecté');}
Ws.onerror=function(Event){AddLog('Etat: Erreur ')+Event.data;}
Canvas.width=Canvas.height=CANVAS_SIZE;Canvas.style.borderWidth=CANVAS_BORDER;Canvas2DCtx=Canvas.getContext('2d');Robot.width=Robot.height=ROBOT_PX_SIZE;Robot2DCtx=Robot.getContext('2d');Canvas2DCtx.fillStyle="black";Canvas2DCtx.fillRect(0,0,CANVAS_SIZE,CANVAS_SIZE);Canvas2DCtx.beginPath()
Canvas2DCtx.strokeStyle="white";Canvas2DCtx.moveTo(Pos.xCoord.value,Pos.yCoord.value+1);Canvas2DCtx.lineTo(Pos.xCoord.value,Pos.yCoord.value);Canvas2DCtx.stroke();for(let i in COMMANDS){let CmdNode=document.createElement('a');CmdNode.innerHTML=COMMANDS[i];CmdNode.setAttribute('href','');CmdList.appendChild(CmdNode);CmdNode.onclick=function(){SendCommand(i);return false;}}
ClearLog();SetWindowsSize();DrawRobot();SetCursorPosition(0,0);}
function SetWindowsSize(){Pos.SetMaxScroll(Frame.clientWidth,Frame.clientHeight);CanvasScroll();}
function CanvasScroll(){Frame.scrollTo(Pos.xCoord.scroll,Pos.yCoord.scroll);}
let LastAngle=0;function DrawRobot(){if(LastAngle!=Angle){LastAngle=Angle;Robot2DCtx.clearRect(0,0,ROBOT_PX_SIZE,ROBOT_PX_SIZE)
let UnitX=Pos.xCoord.vector*ROBOT_PX_UNIT;let UnitY=Pos.yCoord.vector*ROBOT_PX_UNIT;let x=ROBOT_PX_SIZE*0.5-UnitX-UnitY;let y=ROBOT_PX_SIZE*0.5-UnitY+UnitX;Robot2DCtx.beginPath();Robot2DCtx.strokeStyle='#44FF88';Robot2DCtx.moveTo(x,y);x+=UnitX*2;y+=UnitY*2;Robot2DCtx.lineTo(x,y);x+=UnitX+UnitY;y+=UnitY-UnitX;Robot2DCtx.lineTo(x,y);x+=UnitY-UnitX;y-=UnitX+UnitY;Robot2DCtx.lineTo(x,y);x-=UnitX*2;y-=UnitY*2;Robot2DCtx.lineTo(x,y);Robot2DCtx.stroke();}
Robot.style.left=Math.round(Pos.xCoord.value)-ROBOT_PX_SIZE*0.5+CANVAS_BORDER+'px';Robot.style.top=Math.round(Pos.yCoord.value)-ROBOT_PX_SIZE*0.5+CANVAS_BORDER+'px';AddLog("Comp:Reach(x="+Pos.xCoord.value.toFixed(1)+", y="+Pos.yCoord.value.toFixed(1)+")");}
function ComputeTraj(){let NewTime=Date.now();LastTime-=NewTime;let Curve=(MotorLeft-MotorRight);let Speed=(MotorLeft+MotorRight)*(0.5*PIX_SCALE*MAX_SPEED/MAX_MOTOR);Canvas2DCtx.beginPath();Canvas2DCtx.strokeStyle='white';if(Curve!=0){Curve*=MAX_SPEED/(MAX_MOTOR*ROB_WIDTH);let Arc;if(Speed==0){Arc=Curve*LastTime*(-1/MS_PER_S);}else{let SignedRadius=Speed/Curve;Arc=Curve*LastTime*(-1/MS_PER_S);let Radius=SignedRadius;let SquareAngle=Math.PI*0.5;if(Radius<0){Radius=-Radius;SquareAngle=-SquareAngle;}
Pos.SideMove(SignedRadius);let Center=Pos.GetValue();Arc+=Angle;Pos.SetAngle(Arc);Pos.SideMove(-SignedRadius);Canvas2DCtx.arc(Center[0],Center[1],Radius,Angle-SquareAngle,Arc-SquareAngle,(Curve<0));Angle=Arc;}}else{Canvas2DCtx.moveTo(Pos.xCoord.value,Pos.yCoord.value);Pos.Move(Speed*LastTime*(-1/MS_PER_S));Canvas2DCtx.lineTo(Pos.xCoord.value,Pos.yCoord.value);}
Canvas2DCtx.stroke();DrawRobot();LastTime=NewTime;}
function SaveFile(Data,Filename){Saver.download=Filename;let DownloadURL=URL.createObjectURL(Data);Saver.href=DownloadURL;Saver.click();URL.revokeObjectURL(DownloadURL);}</script><body onload=OnLoad() style=height:100%;margin:0;overflow:hidden><h1 style=text-align:center;margin:0>Tableau de bord</h1><div id=Frame style="width:100%;height:70%;border:1px solid grey;overflow:auto;position:relative" onresize=SetWindowsSize()><canvas id=Canvas style="position:relative;border:8px solid red"></canvas>
<canvas id=Robot style=position:absolute></canvas></div><div id=bottom style=text-align:center><div id=CmdList><h2>Commandes</h2></div><div><h2>Vitesse</h2><p>Gauche = <span id=SpeedL>000</span><p>Droite = <span id=SpeedR>000</span></div><div><h2>Position</h2><p>X = <span id=LocX>0000</span><p>Y = <span id=LocY>0000</span></div><div title="Tirer le carré bleu pour déplacer le robot" id=drive style=position:relative><svg style="width:150px;height:150px;margin:5px" width="40.599998mm" height="40.599998mm" viewbox="0 0 40.599998 40.599998" id="svg5"><g id="layer1" transform="translate(-38.253444,-61.886462)"><path class="fill" d="m41.397461 83.505953-.886097 1.590662-.964235-1.590662z" /><circle class="stroke" cx="58.553444" cy="82.186462" r="20" /><path class="stroke" d="m-71.883886-83.228073a2.3809381 2.3368468.0 0 1-1.193179 2.025302 2.3809381 2.3368468.0 0 1-2.383642-.0046 2.3809381 2.3368468.0 0 1-1.185036-2.029902" transform="scale(-1)" /><path class="stroke" d="m76.644391 83.148752-.0298 1.705356" /><path class="fill" d="m75.728494 83.505949.886097 1.590662.964235-1.590662z" /><path class="stroke" d="m45.242068-83.228073a2.3809381 2.3368468.0 0 1-1.193179 2.025302 2.3809381 2.3368468.0 0 1-2.383642-.0046 2.3809381 2.3368468.0 0 1-1.185037-2.029902" transform="scale(1,-1)" /><path class="stroke" d="m40.481564 83.148756.0298 1.705356" /><path class="stroke" d="m58.817993 97.57407v0l-.02205 3.54936" /><path class="fill" d="m59.689834 99.775273-.886097 1.590657-.964235-1.590657z" /><path class="stroke" d="m58.817993 66.69577v0l-.02205-3.54936" /><path class="fill" d="M59.689834 64.494567 58.803737 62.90391 57.839502 64.494567z" /></g></svg><div onmousedown=OnDragStart(event) id=Cursor style=background-color:blue;position:absolute;width:10px;height:10px></div></div><div id=output style="border:1px solid grey;position:relative;height:150px;width:40%;text-align:left"><div id=LogText style=height:100%;width:100%;margin:0;overflow:auto></div><div style=position:absolute;right:0;top:0><a href onclick="return SaveLog()">Sauvegarder</a>
<a href onclick="return ClearLog()">Effacer</a></div></div></div><a id=Saver style=display:none href>
)<<<<"