//*****************************************************************************
//*****************************************************************************
//
// robot state
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "errors.h"
#include "motor.h"
#include "distance_sensor.h"
#include "wheel_encoder.h"
#include "robot_controller.h"
#include "robot.h"

//*****************************************************************************
//
// robot state initialisation
//
//*****************************************************************************
Robot::Robot() {
    ;
}

//*****************************************************************************
//
//
//
//*****************************************************************************
int Robot::Init() {
    int err;

    Serial.println("Robot Init 1");

    // Initialise controller and state
    err = controller.Init();
    if (err)
        return err;
    
    Serial.println("Robot Init 2");

    // Reset internal state and variables
    Reset();

    Serial.println("Robot Init 3");

    return ERROR_NONE;
}

//********************************************************************
// 
//********************************************************************
void Robot::Reset() {
    //controller.Stop();
    controller.Reset();
    x = 0;
    y = 0;
    h = 0;

    // dynamic state
    dynamic_state = ROBOT_DSTATE_STOP;
}

//********************************************************************
// 
//********************************************************************
void Robot::Stop() {
    controller.Stop();
    dynamic_state = ROBOT_DSTATE_STOP;
}

//********************************************************************
// 
//********************************************************************
void Robot::Crash() {
    controller.Stop();
    dynamic_state = ROBOT_DSTATE_CRASH;
}

//********************************************************************
// 
//********************************************************************
void Robot::UpdateState() {
    double dx, dy, dh;
    
    controller.ComputeMove(h, &dx, &dy, &dh);
    
    x+=dx;
    y+=dy;
    h+=dh;
}

//********************************************************************
// 
//********************************************************************
String Robot::JSON_State() {
    String jsonString;
    
    jsonString = "{";
    /*
    jsonString += "\"dmode\":\""+String(dynamic_state)+"\"";
    jsonString += ",";
    jsonString += "\"speed\":\""+String(state.speed*100)+"\"";
    jsonString += ",";
    jsonString += "\"turn\":\""+String(state.turn*100)+"\"";
    jsonString += ",";
    jsonString += "\"x\":\""+String(state.x)+"\"";
    jsonString += ",";
    jsonString += "\"y\":\""+String(state.y)+"\"";
    jsonString += ",";
    jsonString += "\"h\":\""+String(180*state.h/PI)+"\"";
    */

    jsonString += "}";
    return jsonString;
}

//********************************************************************
// 
//********************************************************************
String Robot::String_State() {
    String s;
    
    s = "dstate="+String(dynamic_state);
    /*
    s+= " speed="+String(state.speed);
    s+= " x="+String(state.x);
    s+= " y="+String(state.y);
    s+= " h="+String(state.h*180./PI);
    */
    return s;
}


















