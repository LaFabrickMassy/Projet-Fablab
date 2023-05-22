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
#define TD_STATE_STEP5 5 // rotate CCW
#define TD_STATE_STEP6 6 // rotate CCW
#define TD_STATE_STEP7 7 // rotate CCW
#define TD_STATE_CRASH 99

#define PARAMMODE_KP 1
#define PARAMMODE_KI 2
#define PARAMMODE_KD 3

int td_state;
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

    target_speed = 200.;
    

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
    target_speed = 200;
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
            robot.controller.StraightPrepare(target_speed, 1000);
            td_state = TD_STATE_STEP1;
            break;
        case TD_STATE_STEP1:
            if (robot.controller.Step()) {
                logWrite("Finished step 1");
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP2;
                //td_state = TD_STATE_STOP;
                robot.controller.RotatePrepare(target_speed/2, -PI);
                delay(500);
            }
            break;
        case TD_STATE_STEP2:
            if (robot.controller.Step()) {
                logWrite("Finished step 2");
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP3;
                robot.controller.StraightPrepare(target_speed, 1000);
                delay(500);
            }
            break;
        case TD_STATE_STEP3:
            if (robot.controller.Step()) {
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP4;
                robot.controller.RotatePrepare(target_speed/2, PI);
                delay(500);
            }
            break;
        case TD_STATE_STEP4:
            if (robot.controller.Step()) {
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP5;
                robot.controller.TurnPrepare(target_speed/2, 200, PI/2);
                delay(500);
            }
            break;
        case TD_STATE_STEP5:
            if (robot.controller.Step()) {
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP6;
                robot.controller.TurnPrepare(target_speed/2, -200, PI);
                delay(500);
            }
            break;
        case TD_STATE_STEP6:
            if (robot.controller.Step()) {
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
                td_state = TD_STATE_STEP7;
                robot.controller.TurnPrepare(target_speed/2, 100, PI/2);
                delay(500);
            }
            break;
        case TD_STATE_STEP7:
            if (robot.controller.Step()) {
                robot.controller.Stop();
                notifyClients(robot.controller.JSON_ControlHistory());
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
    #define MUL 0.01
    #define COEFF3 (100.*MUL)
    #define COEFF2 (10.*MUL)
    #define COEFF1 (1.*MUL)

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
        robot.controller.speed_controllerL.cmd_motor += 0.01;
        robot.controller.speed_controllerR.cmd_motor += 0.01;
        notifyClients(robot.controller.JSON_ControllerStatus());     
        logWrite(robot.controller.String_ControllerStatus());
    }
    else if (message == "c-") {
        robot.controller.speed_controllerL.cmd_motor -= 0.01;
        robot.controller.speed_controllerR.cmd_motor -= 0.01;
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
        if (paramMode == PARAMMODE_KD) {
            robot.controller.speed_controllerL.wheel_speed_pid_kd += COEFF3;
            robot.controller.speed_controllerR.wheel_speed_pid_kd += COEFF3;
        }
        else if (paramMode == PARAMMODE_KI) {
            robot.controller.speed_controllerL.wheel_speed_pid_ki += COEFF3;
            robot.controller.speed_controllerR.wheel_speed_pid_ki += COEFF3;
        }
        else {
            robot.controller.speed_controllerL.wheel_speed_pid_kp += COEFF3;
            robot.controller.speed_controllerR.wheel_speed_pid_kp += COEFF3;
        }
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.String_SpeedPID_Params());
    }
    else if (message == "+2") {
        if (paramMode == PARAMMODE_KD) {
            robot.controller.speed_controllerL.wheel_speed_pid_kd += COEFF2;
            robot.controller.speed_controllerR.wheel_speed_pid_kd += COEFF2;
        }
        else if (paramMode == PARAMMODE_KI) {
            robot.controller.speed_controllerL.wheel_speed_pid_ki += COEFF2;
            robot.controller.speed_controllerR.wheel_speed_pid_ki += COEFF2;
        }
        else {
            robot.controller.speed_controllerL.wheel_speed_pid_kp += COEFF2;
            robot.controller.speed_controllerR.wheel_speed_pid_kp += COEFF2;
        }
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "+1") {
        if (paramMode == PARAMMODE_KD) {
            robot.controller.speed_controllerL.wheel_speed_pid_kd += COEFF1;
            robot.controller.speed_controllerR.wheel_speed_pid_kd += COEFF1;
        }
        else if (paramMode == PARAMMODE_KI) {
            robot.controller.speed_controllerL.wheel_speed_pid_ki += COEFF1;
            robot.controller.speed_controllerR.wheel_speed_pid_ki += COEFF1;
        }
        else {
            robot.controller.speed_controllerL.wheel_speed_pid_kp += COEFF1;
            robot.controller.speed_controllerR.wheel_speed_pid_kp += COEFF1;
        }
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "-3") {
        if (paramMode == PARAMMODE_KD) {
            robot.controller.speed_controllerL.wheel_speed_pid_kd -= COEFF3;
            robot.controller.speed_controllerR.wheel_speed_pid_kd -= COEFF3;
        }
        else if (paramMode == PARAMMODE_KI) {
            robot.controller.speed_controllerL.wheel_speed_pid_ki -= COEFF3;
            robot.controller.speed_controllerR.wheel_speed_pid_ki -= COEFF3;
        }
        else {
            robot.controller.speed_controllerL.wheel_speed_pid_kp -= COEFF3;
            robot.controller.speed_controllerR.wheel_speed_pid_kp -= COEFF3;
        }
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "-2") {
        if (paramMode == PARAMMODE_KD) {
            robot.controller.speed_controllerL.wheel_speed_pid_kd -= COEFF2;
            robot.controller.speed_controllerR.wheel_speed_pid_kd -= COEFF2;
        }
        else if (paramMode == PARAMMODE_KI) {
            robot.controller.speed_controllerL.wheel_speed_pid_ki -= COEFF2;
            robot.controller.speed_controllerR.wheel_speed_pid_ki -= COEFF2;
        }
        else {
            robot.controller.speed_controllerL.wheel_speed_pid_kp -= COEFF2;
            robot.controller.speed_controllerR.wheel_speed_pid_kp -= COEFF2;
        }
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else if (message == "-1") {
        if (paramMode == PARAMMODE_KD) {
            robot.controller.speed_controllerL.wheel_speed_pid_kd -= COEFF1;
            robot.controller.speed_controllerR.wheel_speed_pid_kd -= COEFF1;
        }
        else if (paramMode == PARAMMODE_KI) {
            robot.controller.speed_controllerL.wheel_speed_pid_ki -= COEFF1;
            robot.controller.speed_controllerR.wheel_speed_pid_ki -= COEFF1;
        }
        else {
            robot.controller.speed_controllerL.wheel_speed_pid_kp -= COEFF1;
            robot.controller.speed_controllerR.wheel_speed_pid_kp -= COEFF1;
        }
        notifyClients(robot.controller.JSON_SpeedPID_Params());     
        logWrite(robot.controller.JSON_SpeedPID_Params());
    }
    else {
        logWrite("WS: command \'"+String(message)+"\' not available");
    }
}





