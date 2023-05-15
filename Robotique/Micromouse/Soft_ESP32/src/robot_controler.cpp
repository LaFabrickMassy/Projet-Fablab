#include <Arduino.h>
#include <Arduino_JSON.h>
#include "errors.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "logutils.h"
#include "parameters.h"
#include "robot.h"
#include "robot_hw.h"
#include "robot_speed_controller.h"
#include "robot_controller.h"

#ifdef DEBUG_ROBOT_CONTROLLER
// 100 = 33.4%
// 200 = 34.3%
#define DEBUG_TAB_SIZE 400
long elapsed[DEBUG_TAB_SIZE];
double tab_speedL[DEBUG_TAB_SIZE];
double tab_speedR[DEBUG_TAB_SIZE];
double tab_cmdL[DEBUG_TAB_SIZE];
double tab_cmdR[DEBUG_TAB_SIZE];
int debug_tab_index;
#endif

//*****************************************************************************
//
//*****************************************************************************
RobotController::RobotController() {
}

//*****************************************************************************
//
//*****************************************************************************
int RobotController::Init() {
    int err;

    err = robot_hw.Init();
    if (err)
        return err;

    Reset();

    // test motors
    delay(1000);
    robot_hw.MotorsSetSpeed(0.5, 0.5);
    delay(500);
    robot_hw.MotorsSetSpeed(0., 0.);

    speed_controllerL.Init(&encoderL, &(robot_hw.motorL), ROBOT_CONTROLLER_MOTOR_KL);
    speed_controllerR.Init(&encoderR, &(robot_hw.motorR), ROBOT_CONTROLLER_MOTOR_KR);

    return ERROR_NONE;
}

//*****************************************************************************
//
//*****************************************************************************
void RobotController::Reset() {
    robot_hw.Reset();
}

//*****************************************************************************
//
//*****************************************************************************
void RobotController::Stop() {
    //speed_controllerL.Stop();
    //speed_controllerR.Stop();
    //cmd_motorL = 0.;
    //cmd_motorR = 0.;
    robot_hw.MotorsSetSpeed(0., 0.);
}

//*****************************************************************************
//
// Compute data to launch a controlled staight run
// speed in mm/sec 
//
//*****************************************************************************
void RobotController::StraightPrepare(double speed, double distance) {

    logWrite("RobotController.StraightSetup() START");
    speed_controllerL.Prepare(speed, distance);
    speed_controllerR.Prepare(speed, distance);

    #ifdef DEBUG_ROBOT_CONTROLLER
    int i;
    for (i=0;i<DEBUG_TAB_SIZE;i++) {
        tab_speedL[i]=0;
        tab_speedR[i]=0;
        tab_cmdL[i]=0;
        tab_cmdR[i]=0;
    }
    debug_tab_index = 0;
    loop_step = 0;
    start_time = micros();
    #endif

    logWrite("RobotController.RunInit() END");
}

//*****************************************************************************
//
// Controlled straight run
// Set speed until distance is reached
// return false if distance not reached, true if reached
//
//*****************************************************************************
boolean RobotController::Step(){

    boolean finishedL, finishedR;

    finishedL = speed_controllerL.Step();
    finishedR = speed_controllerR.Step();

    if (finishedL && finishedR) {
        return true;
    }

    #ifdef DEBUG_ROBOT_CONTROLLER
    #define DEBUG_SAMPLE_STEP 10
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & ((1<<DEBUG_SAMPLE_STEP)-1)) == 0)) {
        elapsed_time = micros() - start_time;
        elapsed[debug_tab_index] = elapsed_time/1000;
        tab_speedL[debug_tab_index] = speed_controllerL.current_speed/speed_controllerL.target_mean_speed;
        tab_cmdL[debug_tab_index] = speed_controllerL.cmd_motor;
        tab_speedR[debug_tab_index] = speed_controllerR.current_speed/speed_controllerR.target_mean_speed;
        tab_cmdR[debug_tab_index] = speed_controllerR.cmd_motor;
        debug_tab_index++;
    }
    loop_step++;
    #endif

    //robot_hw.MotorsSetSpeed(cmd_motorL, cmd_motorR);

    return false;
}

//*****************************************************************************
//
//*****************************************************************************
void RobotController::TurnPrepare(double speed, double radius, double angle){
    double radiusL, radiusR;
    double distanceL, distanceR;

    if (radius > 0) {
        // right turn
        if (radius > ROBOT_WHEEL_DISTANCE/2.) {
            // center outside, both r > 0
            radiusL = radius + ROBOT_WHEEL_DISTANCE/2.;
            radiusR = radius - ROBOT_WHEEL_DISTANCE/2.;
        }
        else {
            // center inside
            // rL > 0, rR <0
            radiusL = ROBOT_WHEEL_DISTANCE/2. - radius;
            radiusR = ROBOT_WHEEL_DISTANCE/2. + radius;
        }
    }
    else {
        // left turn
        if (radius < -ROBOT_WHEEL_DISTANCE/2.) {
            // center outside
            // 0 < rL < rR
            radiusL = -radius - ROBOT_WHEEL_DISTANCE/2.;
            radiusR = -radius + ROBOT_WHEEL_DISTANCE/2.;
        }
        else {
            // center inside
            // rL <0, rR > 0
            radiusL = ROBOT_WHEEL_DISTANCE/2. - -radius;
            radiusR = ROBOT_WHEEL_DISTANCE/2. + -radius;
        }
    }
    distanceL = radiusL * angle;
    distanceR = radiusR * angle;

    if (angle > 0) {
        speed_controllerL.Prepare(speed*radiusL/radius, distanceL);
        speed_controllerR.Prepare(speed*radiusR/radius, distanceR);
    }
    else {
        speed_controllerL.Prepare(-speed*radiusL/radius, distanceL);
        speed_controllerR.Prepare(-speed*radiusR/radius, distanceR);

    }

    #ifdef DEBUG_ROBOT_CONTROLLER
    int i;
    for (i=0;i<DEBUG_TAB_SIZE;i++) {
        tab_speedL[i]=0;
        tab_speedR[i]=0;
        tab_cmdL[i]=0;
        tab_cmdR[i]=0;
    }
    debug_tab_index = 0;
    loop_step = 0;
    start_time = micros();
    #endif
}

//*****************************************************************************
//
// Compute data for a controlled U-turn.
// angle is CCW
//*****************************************************************************
void RobotController::RotatePrepare(double speed, double angle) {
    double distance;

    distance = angle*ROBOT_WHEEL_DISTANCE/2.;

    speed_controllerL.Prepare(speed, distance);
    speed_controllerR.Prepare(-speed, -distance);

    #ifdef DEBUG_ROBOT_CONTROLLER
    int i;
    for (i=0;i<DEBUG_TAB_SIZE;i++) {
        tab_speedL[i]=0;
        tab_speedR[i]=0;
        tab_cmdL[i]=0;
        tab_cmdR[i]=0;
    }
    debug_tab_index = 0;
    loop_step = 0;
    start_time = micros();
    #endif
}


//********************************************************************
// ComputeMove
//********************************************************************
void RobotController::ComputeMove(double h, double *dx, double *dy, double *dh) {

    #define W ROBOT_WHEEL_DISTANCE
    double dL, dR;

    double rL, rR, r; // curvature radius for left wheel, right wheel and center of robot
    double distance;   // distance run
    double q;          // quotient of left and right radius
    double temp;
    double ca, sa;     // cos(*dh), sin(*dh)
    double ch, sh;     // cos(h), sin(h)
    
    #define W ROBOT_WHEEL_DISTANCE
    

    if (encoderL.flag_reverse)
        dL = (double)(encoderL.count - cm_encoderL_lastcount) * ENCL_RESOL;
    else
        dL = -(double)(encoderL.count - cm_encoderL_lastcount) * ENCL_RESOL;
    cm_encoderL_lastcount = encoderL.count;
    if (encoderR.flag_reverse)
        dR = (double)(encoderR.count - cm_encoderR_lastcount) * ENCR_RESOL;
    else
        dR = -(double)(encoderR.count - cm_encoderR_lastcount) * ENCR_RESOL;
    cm_encoderR_lastcount = encoderR.count;
    
    
    if (dL > 0.)
    {
        if (dR > 0.)
        {
            if (dL > dR) // Case 1
            {
                q = dL/dR;
                rR = W/(q-1.);
                r = rR+W/2.;
                *dh = -dR / rR;      
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on right
                *dx = -r * (sh*(1.-ca) - ch*sa);
                *dy = -r * (ch*(1.-ca) - sh*sa);
				
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 01");
                Serial.print(" dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" q=");
                Serial.print(q);
				Serial.print(" rR=");
                Serial.print(rR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else if (dL == dR) // Case 2
            {
                *dh = 0.;
                r = dL;
                ch = cos(h);
                sh = sin(h);

                *dx = r*ch;
                *dy = r*sh;

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 02");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else // dL < dR Case 3
            {
                q = dR/dL;
                rL = W/(q-1.);
                r = rL+W/2.;
                *dh = dL / rL;      
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on left
                *dx = -r * (sh*(1.-ca) + ch*sa);
                *dy =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 03");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
        }
        else if (dR == 0.) // Case 4
        {
            r = W/2.;
            *dh = -dL/W;
            ch = cos(h);
            sh = sin(h);
            ca = cos(*dh);
            sa = sin(*dh);
            
            // O on right
            *dx = -r * (sh*(1.-ca) - ch*sa);
            *dy = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 042");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
        }
        else // dR < 0
        {
            if (dL < -dR) // Case 5
            {
                q = dL/dR;
                rR = W/(q+1.);
                r = W/2.-rR;
                *dh = -dR / rR;      
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on right
                *dx = -r * (sh*(1.-ca) - ch*sa);
                *dy = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 05");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else if (dL = -dR) // Case 6
            {
				r = 0.;
                *dh = -dL/(W/2.);
                *dx = 0.;
                *dy = 0.;
                
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 06");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif

            }
            else // dL > -dR Case 7
            {
                q = dR/dL;
                rL = W/(q+1.);
                r = W/2.-rL;
                *dh = -dL / rL;      
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on left
                *dx = -r * (sh*(1.-ca) + ch*sa);
                *dy =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 07");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
        }
    }
    else if (dL == 0)
    {
        if (dR > 0) // Case 8
        {
            r = W/2.;
            *dh = dR/W;
            ch = cos(h);
            sh = sin(h);
            ca = cos(*dh);
            sa = sin(*dh);
            
            // O on left
            *dx = -r * (sh*(1.-ca) + ch*sa);
            *dy =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 08");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
        }
        else if (dR == 0) // Case 9
        {
			r = 0.;
            *dh = 0.;
            *dx = 0.;
            *dy = 0.;
                
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 09");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
        }
        else // dR < 0 Case 10
        {
            r = W/2.;
            *dh = dR/W;
            
            ch = cos(h);
            sh = sin(h);
            ca = cos(*dh);
            sa = sin(*dh);
            
            // O on left
            *dx = -r * (sh*(1.-ca) + ch*sa);
            *dy =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 10");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
        }
    }
    else // dL < 0
    {
        if (dR > 0)
        {
            if (-dL < dR) // Case 11
            {
                q = dR/dL;
                rL = W/(q+1.);
                r = W/2. - rL;
                *dh = -dL / rL;      
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on left
                *dx = -r * (sh*(1.-ca) + ch*sa);
                *dy =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 11");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else if (-dL == dR) // Case 12
            {
                r = 0;
                *dh = -dL/(W/2.);
                *dx = 0.;
                *dy = 0.;
                
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 12");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else // -dL > dR Case 13
            {
                q = dL/dR;
                rR = W/(q+1.);
                r = W/2.-rR;
                *dh = -dR / rR;
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on right
                *dx = -r * (sh*(1.-ca) - ch*sa);
                *dy = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 13");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
        }
        else if (dR == 0) // Case 14
        {
            r = W/2.;
            *dh = -dL/W;
            ch = cos(h);
            sh = sin(h);
            ca = cos(*dh);
            sa = sin(*dh);
            
            // O on right
            *dx = -r * (sh*(1.-ca) - ch*sa);
            *dy = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 14");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
        }
        else // dR < 0 
        {
            if (dL < dR) // Case 15
            {
                q = dL/dR;
                rR = W/(q-1.);
                r = W/2.+rR;
                *dh = -dR / rR;
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on right
                *dx = -r * (sh*(1.-ca) - ch*sa);
                *dy = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 15");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else if (dL = dR) // Case 16
            {
                r = -dL;
                *dh = 0.;
                ch = cos(h);
                sh = sin(h);
                
                *dx = -r*ch;
                *dy = -r*sh;

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 16");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
            else // dL > dR Case 17
            {
                q = dR/dL;
                rL = W/(q-1.);
                r = rL+W/2.;
                *dh = dL / rL;      
                ch = cos(h);
                sh = sin(h);
                ca = cos(*dh);
                sa = sin(*dh);
                
                // O on left
                *dx = -r * (sh*(1.-ca) + ch*sa);
                *dy =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 17");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(*dh*180/PI);
				Serial.print(" mx=");
				Serial.print(*dx);
				Serial.print(" my=");
				Serial.print(*dy);
				Serial.println("");
                #endif
            }
        }
    }
    
	#ifdef DEBUG_MOVE
    Serial.println("########## Compute move");
    Serial.print(" h=");
    Serial.print(heading*180/PI);
    Serial.print(" Pos : x=");
    Serial.print(x);
    Serial.print(" y=");
    Serial.print(y);
    Serial.println("");
	#endif

    #undef W
}






//*****************************************************************************
//
//*****************************************************************************
String RobotController::JSON_SpeedPID_Params() {

    JSONVar json_object;

    json_object["kp"] = (double)((int)(speed_controllerL.wheel_speed_pid_kp*10000.))/100.;
    json_object["ki"] = (double)((int)(speed_controllerL.wheel_speed_pid_ki*10000.))/100.;
    json_object["kd"] = (double)((int)(speed_controllerL.wheel_speed_pid_kd*10000.))/100.;

    return JSON.stringify(json_object);

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::String_SpeedPID_Params() {

    String s;
    
    s = "";
    s += "kp:"+String( (double)((int)(speed_controllerL.wheel_speed_pid_kp*10000.))/100.);
    s += " / ";
    s += "ki:"+String( (double)((int)(speed_controllerL.wheel_speed_pid_ki*10000.))/100.);
    s += " / ";
    s += "kd:"+String( (double)((int)(speed_controllerL.wheel_speed_pid_kd*10000.))/100.);

    return s;

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::JSON_ControllerStatus() {
    JSONVar json_object;

    json_object["mean_speedL"] = (double)((int)speed_controllerL.current_speed*10)/10.;
    json_object["mean_speedR"] = (double)((int)speed_controllerR.current_speed*10)/10.;
    json_object["target_speedL"] = speed_controllerL.target_mean_speed;
    json_object["target_speedR"] = speed_controllerR.target_mean_speed;
    json_object["cmd_motorL"] = speed_controllerL.cmd_motor;
    json_object["cmd_motorR"] = speed_controllerR.cmd_motor;

    return JSON.stringify(json_object);

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::String_ControllerStatus() {
    String s;

    s = "";
    s += "mean_speedL:" + String(speed_controllerL.current_speed);
    s += " / ";
    s += "mean_speedR:" + String(speed_controllerR.current_speed);
    s += " / ";
    s +="target_speedL:" + String(speed_controllerL.cmd_motor);
    s += " / ";
    s +="target_speedR:" + String(speed_controllerR.cmd_motor);
    s += " / ";
    s += "cmd_motorL:" + String(speed_controllerL.cmd_motor);
    s += " / ";
    s += "cmd_motorR:" + String(speed_controllerR.cmd_motor);

    return s;

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::JSON_ControlHistory() {
    String s;
    int i;
    s = "";
    s += "{\"controller_data\":";
    s += "{";
    s += "\"tab_elapsed\":[";
    for (i=0;i<debug_tab_index-1;i++) {
        s+=String(elapsed[i])+",";
    }
    s += String(elapsed[i])+"]";
    s += ",";
    s += "\"tab_speedL\":[";
    for (i=0;i<debug_tab_index-1;i++) {
        s+=String(tab_speedL[i])+",";
    }
    s += String(tab_speedL[i])+"]";
    s += ",";
    s += "\"tab_speedR\":[";
    for (i=0;i<debug_tab_index-1;i++) {
        s+=String(tab_speedR[i])+",";
    }
    s += String(tab_speedR[i])+"]";
    s += ",";
    s += "\"tab_cmdL\":[";
    for (i=0;i<debug_tab_index-1;i++) {
        s+=String(100.*tab_cmdL[i])+",";
    }
    s += String(100.*tab_cmdL[i])+"]";
    s += ",";
    s += "\"tab_cmdR\":[";
    for (i=0;i<debug_tab_index-1;i++) {
        s+=String(100.*tab_cmdR[i])+",";
    }
    s += String(100.*tab_cmdR[i])+"]";
    s += "}";
    s += "}";

    return s;
}


