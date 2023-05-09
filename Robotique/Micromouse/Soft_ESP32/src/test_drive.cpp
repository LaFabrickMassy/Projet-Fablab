//*****************************************************************************
//*****************************************************************************
//
// Simple run
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "parameters.h"
#include "webserver.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "distance_sensor.h"
#include "robot_controller.h"
#include "robot.h"
#include "hmi.h"
#include "micromouse.h"
#include "logutils.h"
#include "test_drive.h"

#define TD_STATE_STOP 0
#define TD_STATE_START 98
#define TD_STATE_STEP1 1 // run 500 mm
#define TD_STATE_STEP2 2 // rotate CW
#define TD_STATE_STEP3 3 // run 500 mm
#define TD_STATE_STEP4 4 // rotate CCW
#define TD_STATE_CRASH 99

#define PARAMMODE_KP 1
#define PARAMMODE_KI 2
#define PARAMMODE_KD 3

int td_state;
double speed;
double turn;
double target_speed;

static boolean flagAuto;
static int paramMode;


//*****************************************************************************
//
// Init
//
//*****************************************************************************
void testDriveInit() {
    logWrite("testDriveInit() START");
    td_state = TD_STATE_STOP;
    robot.Reset();
    robot.controller.Stop();
    notifyClients(robot.controller.JSON_SpeedPID_Params());

    target_speed = 250.;
    

    logWrite("testDriveInit() END");
}

//*****************************************************************************
//
// Step
//
//*****************************************************************************
void testDriveStop() {
    td_state = TD_STATE_STOP;
    robot.controller.Stop();
    notifyClients(robot.controller.JSON_ControlHistory());
}

//*****************************************************************************
//
// Start
//
//*****************************************************************************
void testDriveStart() {
    td_state = TD_STATE_START;
}

//*****************************************************************************
//
// Step
//
//*****************************************************************************
void testDriveStep() {
    switch(td_state) {
        case TD_STATE_STOP:
            // Stopped, do nothing
            break;
        case TD_STATE_CRASH:
            testDriveStop();
            break;
        case TD_STATE_START:
            robot.controller.RunInit(target_speed, 1000);
            td_state = TD_STATE_STEP1;
            break;
        case TD_STATE_STEP1:
            if (robot.controller.RunStep()) {
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP2;
                td_state = TD_STATE_STOP;
                robot.controller.RotateInit(target_speed, -180);
                delay(500);
            }
            break;
        case TD_STATE_STEP2:
            if (robot.controller.RotateStep()) {
                robot.controller.Stop();
                td_state = TD_STATE_STEP3;
                robot.controller.RunInit(target_speed, 500);
                delay(500);
            }
            break;
        case TD_STATE_STEP3:
            if (robot.controller.RunStep()) {
                robot.controller.Stop();
                td_state = TD_STATE_STEP4;
                robot.controller.RotateInit(target_speed, 180);
                delay(500);
            }
            break;
        case TD_STATE_STEP4:
            if (robot.controller.RotateStep()) {
                robot.controller.Stop();
                td_state = TD_STATE_STOP;
                robot.controller.Stop();
                delay(500);
            }
            break;
        default:
            break;
    }
}

//*****************************************************************************
//
// Step
//
//*****************************************************************************
void testDriveHandleMessage(String message) {
    logWrite("TEST_DRIVE handle message "+message);
    if (message == "index") {
        logWrite("TD: Back to index");
        // Set mode to stop
        robot.controller.Stop();
        hmi_mode = HMI_MODE_INDEX;
    }
    else if (message == "start") {
        // start run
        logWrite("TD: Start");
        testDriveStart();
    }
    else if (message == "uturn") {
        ;
    }
    else if (message == "stop") {
        logWrite("TD: Stop");
        // stop PID run
        testDriveStop();
    }
    else if (message == "auto_on") {
        logWrite("TD: Auto on");
        flagAuto = true;
    }
    else if (message == "auto_off") {
        logWrite("TD: Auto off");
        flagAuto = false;
    }
    else if (message == "sel_kp") {
        paramMode = PARAMMODE_KP;
    }
    else if (message == "sel_ki") {
        paramMode = PARAMMODE_KI;
    }
    else if (message == "sel_kd") {
        paramMode = PARAMMODE_KD;
    }
    else if (message == "c+") {
        robot.controller.cmd_motorL += 0.01;
        robot.controller.cmd_motorR += 0.01;
        notifyClients(robot.controller.JSON_ControllerStatus());     
        logWrite(robot.controller.String_ControllerStatus());
    }
    else if (message == "c-") {
        robot.controller.cmd_motorL -= 0.01;
        robot.controller.cmd_motorR -= 0.01;
        notifyClients(robot.controller.JSON_ControllerStatus());     
        logWrite(robot.controller.String_ControllerStatus());
    }
    else if (message == "s+10") {
        target_speed += 10;
        notifyClients(robot.controller.JSON_ControllerStatus());     
        logWrite(robot.controller.String_ControllerStatus());
    }
    else if (message == "s-10") {
        target_speed -= 10;
        notifyClients(robot.controller.JSON_ControllerStatus());     
        logWrite(robot.controller.String_ControllerStatus());
    }
    else if (message == "+3") {
        if (paramMode == PARAMMODE_KD)
            robot.controller.speedpid_kd += 0.001;
        else if (paramMode == PARAMMODE_KI)
            robot.controller.speedpid_ki += 0.001;
        else
            robot.controller.speedpid_kp += 0.001;
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.String_SpeedPID_Params());
    }
    else if (message == "+2") {
        if (paramMode == PARAMMODE_KD)
            robot.controller.speedpid_kd += 0.0001;
        else if (paramMode == PARAMMODE_KI)
            robot.controller.speedpid_ki += 0.0001;
        else
            robot.controller.speedpid_kp += 0.0001;
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "+1") {
        if (paramMode == PARAMMODE_KD)
            robot.controller.speedpid_kd += 0.00001;
        else if (paramMode == PARAMMODE_KI)
            robot.controller.speedpid_ki += 0.00001;
        else
            robot.controller.speedpid_kp += 0.00001;
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "-3") {
        if (paramMode == PARAMMODE_KD)
            robot.controller.speedpid_kd -= 0.001;
        else if (paramMode == PARAMMODE_KI)
            robot.controller.speedpid_ki -= 0.001;
        else
            robot.controller.speedpid_kp -= 0.001;
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "-2") {
        if (paramMode == PARAMMODE_KD)
            robot.controller.speedpid_kd -= 0.0001;
        else if (paramMode == PARAMMODE_KI)
            robot.controller.speedpid_ki -= 0.0001;
        else
            robot.controller.speedpid_kp -= 0.0001;
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "-1") {
        if (paramMode == PARAMMODE_KD)
            robot.controller.speedpid_kd -= 0.00001;
        else if (paramMode == PARAMMODE_KI)
            robot.controller.speedpid_ki -= 0.00001;
        else
            robot.controller.speedpid_kp -= 0.00001;
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else {
        logWrite("WS: command \'"+String(message)+"\' not available");
    }
}

#ifdef __OLD
//*****************************************************************************
//
// Reset simple run :
// set origin to current robot position
//
//*****************************************************************************
void testDriveRunInit() {

    current_state = ROBOT_STATE_RUN;

}


//*****************************************************************************
//
// Initialise simple run
//
//*****************************************************************************
void testDriveRunStep() {
    // relative left and right speeds from turn value
    float rel_speedL;
    float rel_speedR;
    int   reverseL;
    int   reverseR;
    
    // compute relative left and right speeds from turn values
    if (turn < 0) // turn left
    {
        rel_speedR = 1.;
        rel_speedL = 1. + 2.*turn;
    }
    else if (turn > 0) // turn right
    {
        rel_speedL = 1.;
        rel_speedR = 1. - 2.*turn;
    }
    else // ahead
    {
        rel_speedL = 1.;
        rel_speedR = 1.;
    }

    /*
    if (rel_speedL < 0)
    {
        reverseL = 1;
        rel_speedL = -rel_speedL;
    }
    else
        reverseL = 0;
    
    if (rel_speedR < 0)
    {
        reverseR = 1;
        rel_speedR = -rel_speedR;
    }
    else
        reverseR = 0;
    */
   
    motorSetSpeed(&motorL, SPEED_BASE*rel_speedL);
    motorSetSpeed(&motorR, SPEED_BASE*rel_speedR);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void testDriveRotateStep() {
    int status;

    status = rotationStep();

    if (status) {
        // target angle reached, stop rotation
        current_state = ROBOT_STATE_ROTATE_END;
    }
}

//*****************************************************************************
//
// Stop robot
//
//*****************************************************************************
void testDriveStop() {

    robot.Stop()

}

#endif