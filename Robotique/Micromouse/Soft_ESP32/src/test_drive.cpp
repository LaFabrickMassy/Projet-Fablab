//*****************************************************************************
//*****************************************************************************
//
// Simple run
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "parameters.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "distance_sensor.h"
#include "micromouse.h"
#include "robot_controller.h"
#include "test_drive.h"

//*****************************************************************************
//
// Initialise simple run
//
//*****************************************************************************
void testDriveInit() {

    speed = 0.0;
    turn = 0.0;
    pos_x = 0.;
    pos_y = 0.;
    heading = 0.;

    motorSetSpeed(&motorL, 0.);
    motorSetSpeed(&motorR, 0.);

    current_state = ROBOT_STATE_STOP;
}

//*****************************************************************************
//
// Reset simple run :
// set origin to current robot position
//
//*****************************************************************************
void testDriveRunInit() {

    speed = 0.0;
    turn = 0.0;

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
   
    motorSetSpeed(&motorL, speed*rel_speedL);
    motorSetSpeed(&motorR, speed*rel_speedR);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void testDriveRotateInitCW() {

    rotationInit(170., 0.5);
    current_state = ROBOT_STATE_ROTATE;
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void testDriveRotateInitCCW() {

    rotationInit(-170., 0.5);
    current_state = ROBOT_STATE_ROTATE;
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
    speed = 0.0;
    turn = 0.0;
    motorSetSpeed(&motorL, 0);
    motorSetSpeed(&motorR, 0);

    current_state = ROBOT_STATE_STOP;
}

//********************************************************************
// 
//********************************************************************
String getRobotStatus() {
    String jsonString;
    
    jsonString = "{";
    jsonString += "\"mode\":\""+String(current_mode)+"\"";
    jsonString += ",";
    jsonString += "\"state\":\""+String(current_state)+"\"";
    jsonString += ",";
    jsonString += "\"speed\":\""+String(speed*100)+"\"";
    jsonString += ",";
    jsonString += "\"turn\":\""+String(turn*100)+"\"";
    jsonString += ",";
    jsonString += "\"x\":\""+String(pos_x)+"\"";
    jsonString += ",";
    jsonString += "\"y\":\""+String(pos_y)+"\"";
    jsonString += ",";
    jsonString += "\"h\":\""+String(180*heading/PI)+"\"";
	
	// Encoders data
    jsonString += ",";
    #ifdef TOTO
    if (motorL.flag_reverse)
        jsonString += "\"eL\":\""+String(-encoderL.count)+"\"";
    else
        jsonString += "\"eL\":\""+String(encoderL.count)+"\"";
    jsonString += "\"eL\":\""+String(encoderL.count)+"\"";
    jsonString += ",";
    if (motorR.flag_reverse)
        jsonString += "\"eR\":\""+String(-encoderR.count)+"\"";
    else
        jsonString += "\"eR\":\""+String(encoderR.count)+"\"";
    #endif
    jsonString += "\"eL\":\""+String(encoderL.count)+"\"";
    jsonString += ",";
    jsonString += "\"eR\":\""+String(encoderR.count)+"\"";
	
	// Sensors data
    jsonString += ",";
    jsonString += "\"sens_L\":\""+String(distanceSensor1())+"\"";
    #if NB_OF_SENSORS >= 2
    jsonString += ",";
    jsonString += "\"sens_F\":\""+String(distanceSensor2())+"\"";
    #endif
    #if NB_OF_SENSORS >= 3
    jsonString += ",";
    jsonString += "\"sens_R\":\""+String(distanceSensor3())+"\"";
    #endif
    #if NB_OF_SENSORS >= 4
    jsonString += ",";
    jsonString += "\"sensor4\":\""+String(distanceSensor4())+"\"";
    #endif
    #if NB_OF_SENSORS >= 5
    jsonString += ",";
    jsonString += "\"sensor5\":\""+String(distanceSensor5())+"\"";
	#endif

    jsonString += "}";
    return jsonString;
}

