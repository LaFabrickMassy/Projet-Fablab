#include <Arduino.h>
#include <Arduino_JSON.h>
#include "errors.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "logutils.h"
#include "parameters.h"
#include "robot.h"
#include "robot_hw.h"
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

// PID variables
double sPID_error;
double sPID_old_error;

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

    speedpid_kp = ROBOT_CONTROLLER_SPEEDPID_KP;
    speedpid_ki = ROBOT_CONTROLLER_SPEEDPID_KI;
    speedpid_ki = ROBOT_CONTROLLER_SPEEDPID_KD;
    Reset();

    // test motors
    delay(1000);
    robot_hw.MotorsSetSpeed(0.5, 0.5);
    delay(500);
    robot_hw.MotorsSetSpeed(0., 0.);

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
    robot_hw.MotorsStop();
    //cmd_motorL = 0.;
    //cmd_motorR = 0.;
}

//*****************************************************************************
//
// Control speed
// returns target motor speed
//
//*****************************************************************************
double RobotController::SpeedPID(double target_speed, double current_speed) {
    double up, ui, ud;
    double dt;
    dt = (double)(loop_time - last_loop_time);
    sPID_error = target_speed - current_speed;
    up = 0.001* speedpid_kp * sPID_error;
    ui = 0.0000001* speedpid_ki * (sPID_error - sPID_old_error) * dt;
    ud = 0.0000001* speedpid_kd * (sPID_error - sPID_old_error) / dt;
    sPID_old_error = sPID_error;

    return up+ui+ud;
}

//*****************************************************************************
//
// Compute data to launch a controlled staight run
// speed in mm/sec 
//
//*****************************************************************************
void RobotController::RunInit(double speed, double distance) {

    logWrite("RobotController.RunInit() START");
    target_speedL = speed;
    target_speedR = speed;
    target_distanceL = distance;
    target_distanceR = distance;
    cmd_motorL = target_speedL*ROBOT_CONTROLLER_MOTOR_KL;
    cmd_motorR = target_speedR*ROBOT_CONTROLLER_MOTOR_KR;
    run_distanceL = 0.;
    run_distanceR = 0.;
    init_encvalueL = encoderL.count;
    init_encvalueR = encoderR.count;
    old_run_distanceL = 0.;
    old_run_distanceR = 0.;

    #ifdef DEBUG_ROBOT_CONTROLLER
    int i;
    for (i=0;i<DEBUG_TAB_SIZE;i++) {
        tab_speedL[i]=0;
        tab_speedR[i]=0;
        tab_cmdL[i]=0;
        tab_cmdR[i]=0;
    }
    debug_tab_index = 0;
    #endif
    loop_step = 0;
    start_time = micros();
    logWrite("RobotController.RunInit() END");
}

//*****************************************************************************
//
// Compute data to launch a controlled staight run
// speed in mm/sec 
//
//*****************************************************************************
void RobotController::RunUpdateDistance(double distance) {
    target_distanceL = distance;
    target_distanceR = distance;
}

//*****************************************************************************
//
// Controlled straight run
// Set speed until distance is reached
// return false if distance not reached, true if reached
//
//*****************************************************************************
boolean RobotController::RunStep(){

    #define DEBUG_SAMPLE_STEP 0x7F

    //logWrite("RobotController.RunStep() START");
    // compute run distance
    run_distanceL = ((double)(encoderL.count - init_encvalueL))*ENCL_RESOL;
    run_distanceR = ((double)(encoderR.count - init_encvalueR))*ENCR_RESOL;
    if ((run_distanceL + run_distanceR) >= (target_distanceL + target_distanceR)) {
        // done
        return true;
    }

    //robot_hw.MotorsSetSpeed(MOTOR_CMD_MAX, MOTOR_CMD_MAX);
    
    // force start
    if ((run_distanceL == 0 ) || (run_distanceR == 0)) {
        #ifdef TOTO
        if (target_speedL > 0)
            cmd_motorL = MOTOR_CMD_MAX/2.;
        else
            cmd_motorL = -MOTOR_CMD_MAX/2.;
        if (target_speedR > 0)
            cmd_motorR = MOTOR_CMD_MAX/2.;
        else
            cmd_motorR = -MOTOR_CMD_MAX/.2;
        #endif
        robot_hw.MotorsSetSpeed(cmd_motorL, cmd_motorR);
        #ifdef DEBUG_ROBOT_CONTROLLER
            elapsed[debug_tab_index] = elapsed_time/1000;
            tab_speedL[debug_tab_index] = 0.;
            tab_speedR[debug_tab_index] = 0.;
            tab_cmdL[debug_tab_index] = cmd_motorL;
            tab_cmdR[debug_tab_index] = cmd_motorR;
            debug_tab_index++;
        #endif
        return false;
    }

    // if no encoder change, do nothing
    if ((run_distanceL == old_run_distanceL) || (run_distanceR == old_run_distanceR))
        return false;

    // mean speed
    loop_time = micros();
    elapsed_time = loop_time - start_time;
    
    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        elapsed[debug_tab_index] = elapsed_time/1000;
    }
    #endif

    mean_speedL = 1000000.*run_distanceL/(double)elapsed_time;
    
    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_speedL[debug_tab_index] = mean_speedL;
    }
    #endif
    if (mean_speedL < 0)
        mean_speedL = 0;
    cmd_motorL += SpeedPID(target_speedL, mean_speedL);
    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_cmdL[debug_tab_index] = cmd_motorL;
    }
    #endif
    if (cmd_motorL > MOTOR_CMD_MAX)
        cmd_motorL = MOTOR_CMD_MAX;
    if (cmd_motorL < -MOTOR_CMD_MAX)
        cmd_motorL = -MOTOR_CMD_MAX;
    if (cmd_motorL > 0 && cmd_motorL < MOTOR_CMD_MIN)
        cmd_motorL = MOTOR_CMD_MIN;
    if (cmd_motorL < 0 && cmd_motorL > -MOTOR_CMD_MIN)
        cmd_motorL = -MOTOR_CMD_MIN;

    mean_speedR = 1000000.*run_distanceR/(double)elapsed_time;
    
    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_speedR[debug_tab_index] = mean_speedR;
    }
    #endif
    
    if (mean_speedR < 0)
        mean_speedR = 0;

    cmd_motorR += SpeedPID(target_speedR, mean_speedR);
    
    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_cmdR[debug_tab_index] = cmd_motorR;
    }
    #endif

    if (cmd_motorR > MOTOR_CMD_MAX)
        cmd_motorR = MOTOR_CMD_MAX;
    if (cmd_motorR < -MOTOR_CMD_MAX)
        cmd_motorR = -MOTOR_CMD_MAX;
    if (cmd_motorR > 0 && cmd_motorR < MOTOR_CMD_MIN)
        cmd_motorR = MOTOR_CMD_MIN;
    if (cmd_motorR < 0 && cmd_motorR > -MOTOR_CMD_MIN)
        cmd_motorR = -MOTOR_CMD_MIN;

    //motorSetSpeed(p_motorL, cmd_motorL);
    //motorSetSpeed(p_motorR, cmd_motorR);
    robot_hw.MotorsSetSpeed(cmd_motorL, cmd_motorR);

    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        debug_tab_index++;
    }
    loop_step++;
    #endif

    old_run_distanceL = run_distanceL;
    old_run_distanceR = run_distanceR;
    last_loop_time = loop_time;

    //logWrite("RobotController.RunStep() END");
    //delay(100);
    return false;
}

//*****************************************************************************
//
//*****************************************************************************
void RobotController::TurnInit(double speed, double radius, double angle){
    if (radius > ROBOT_WHEEL_DISTANCE/2.) {
        // center of circle is outside the robot
        ;

    }
    else {
        // centor of circle between the two wheels
        ;
    }
}

//*****************************************************************************
//
//*****************************************************************************
boolean RobotController::TurnStep(){
    return false;
}

//*****************************************************************************
//
// Compute data for a controlled U-turn.
// angle is CCW
//*****************************************************************************
void RobotController::RotateInit(double speed, double angle) {
    if (angle > 0) { // CCW rotation
        target_speedL = -speed;
        target_speedR = speed;
        target_distanceL = -PI/180.*ROBOT_WHEEL_DISTANCE/2.;
        target_distanceR = PI/180.*ROBOT_WHEEL_DISTANCE/2.; 
    }
    else if (angle <0) { // CW rotation}
        target_speedL = speed;
        target_speedR = -speed;
        target_distanceL = PI/180.*ROBOT_WHEEL_DISTANCE/2.; 
        target_distanceR = -PI/180.*ROBOT_WHEEL_DISTANCE/2.;
    }
    cmd_motorL = target_speedL/ROBOT_CONTROLLER_MOTOR_KL;
    cmd_motorR = target_speedR/ROBOT_CONTROLLER_MOTOR_KR;
    run_distanceL = 0.;
    run_distanceR = 0.;
    init_encvalueL = encoderL.count;
    init_encvalueR = encoderR.count;
    old_run_distanceL = 0.;
    old_run_distanceR = 0.;

    #ifdef DEBUG_ROBOT_CONTROLLER
    int i;
    for (i=0;i<DEBUG_TAB_SIZE;i++) {
        tab_speedL[i]=0;
        tab_speedR[i]=0;
        tab_cmdL[i]=0;
        tab_cmdR[i]=0;
    }
    debug_tab_index = 0;
    #endif
    loop_step = 0;
    start_time = micros();
}

//*****************************************************************************
//
// return 0 if U-turn not finished, 1 if finished
//
//*****************************************************************************
boolean RobotController::RotateStep() {
    double mean_speedL, mean_speedR;
    long elapsed_time;

    // compute run distance
    run_distanceL = ((double)(encoderL.count - init_encvalueL))/ENCL_RESOL;
    run_distanceR = ((double)(encoderR.count - init_encvalueR))/ENCR_RESOL;
    if ((abs(run_distanceL) >= abs(target_distanceL)) || (abs(run_distanceR) >= abs(target_distanceR)))
        return true;

    // force start
    if ((run_distanceL == 0 ) || (run_distanceR == 0)) {
        robot_hw.MotorsSetSpeed(cmd_motorL, cmd_motorR);
        return false;
    }

    // if no encoder change, do nothing
    if ((run_distanceL == old_run_distanceL) || (run_distanceR == old_run_distanceR))
        return false;

    // mean speed
    elapsed_time = micros() - start_time;

    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        elapsed[debug_tab_index] = elapsed_time/1000;
    }
    #endif

    mean_speedL = 1000000.*run_distanceL/(double)elapsed_time;

    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_speedL[debug_tab_index] = mean_speedL;
    }
    #endif

    cmd_motorL += SpeedPID(target_speedL, mean_speedL);
    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_cmdL[debug_tab_index] = cmd_motorL;
    }
    #endif
    if (cmd_motorL > SPEED_MAX)
        cmd_motorL = SPEED_MAX;
    if (cmd_motorL < -SPEED_MAX)
        cmd_motorL = -SPEED_MAX;
    if (cmd_motorL > 0 && cmd_motorL < MOTOR_CMD_MIN)
        cmd_motorL = MOTOR_CMD_MIN;
    if (cmd_motorL < 0 && cmd_motorL > -MOTOR_CMD_MIN)
        cmd_motorL = -MOTOR_CMD_MIN;

    mean_speedR = 1000000.*run_distanceL/(double)elapsed_time;

    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_speedR[debug_tab_index] = mean_speedR;
    }
    #endif

    cmd_motorR += SpeedPID(target_speedR, mean_speedR);

    #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        tab_cmdR[debug_tab_index] = cmd_motorR;
    }
    #endif

    if (cmd_motorR > SPEED_MAX)
        cmd_motorR = SPEED_MAX;
    if (cmd_motorR < -SPEED_MAX)
        cmd_motorR = -SPEED_MAX;

    robot_hw.MotorsSetSpeed(cmd_motorL, cmd_motorR);
    //motorSetSpeed(motorL, cmd_motorL);
    //motorSetSpeed(motorR, cmd_motorR);

     #ifdef DEBUG_ROBOT_CONTROLLER
    if ((debug_tab_index < DEBUG_TAB_SIZE) && ((loop_step & DEBUG_SAMPLE_STEP) == 0)) {
        debug_tab_index++;
    }
    loop_step++;
    #endif

    old_run_distanceL = run_distanceL;
    old_run_distanceR = run_distanceR;

    return false;
}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::JSON_SpeedPID_Params() {

    JSONVar json_object;

    json_object["kp"] = (double)((int)(speedpid_kp*100000.))/100.;
    json_object["ki"] = (double)((int)(speedpid_ki*100000.))/100.;
    json_object["kd"] = (double)((int)(speedpid_kd*100000.))/100.;

    return JSON.stringify(json_object);

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::String_SpeedPID_Params() {

    String s;
    
    s = "";
    s += "k:p"+String( (double)((int)(speedpid_kp*10000.))/100.);
    s += " / ";
    s += "ki:"+String( (double)((int)(speedpid_ki*10000.))/100.);
    s += " / ";
    s += "kd:"+String( (double)((int)(speedpid_kd*10000.))/100.);

    return s;

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::JSON_ControllerStatus() {
    JSONVar json_object;

    json_object["mean_speedL"] = (double)((int)mean_speedL*10)/10.;
    json_object["mean_speedR"] = (double)((int)mean_speedR*10)/10.;
    json_object["target_speedL"] = target_speedL;
    json_object["target_speedR"] = target_speedR;
    json_object["cmd_motorL"] = cmd_motorL;
    json_object["cmd_motorR"] = cmd_motorR;

    return JSON.stringify(json_object);

}

//*****************************************************************************
//
//*****************************************************************************
String RobotController::String_ControllerStatus() {
    String s;

    s = "";
    s += "mean_speedL:" + String(mean_speedL);
    s += " / ";
    s += "mean_speedR:" + String(mean_speedL);
    s += " / ";
    s +="target_speedL:" + String(target_speedL);
    s += " / ";
    s +="target_speedR:" + String(target_speedR);
    s += " / ";
    s += "cmd_motorL:" + String( cmd_motorL);
    s += " / ";
    s += "cmd_motorR:" + String( cmd_motorR);

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
    s += String(tab_speedL[i])+"]";
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



