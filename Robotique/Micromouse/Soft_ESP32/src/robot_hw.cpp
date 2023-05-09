//*****************************************************************************
//*****************************************************************************
//
// robot state
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "errors.h"
#include "parameters.h"
#include "wheel_encoder.h"
#include "motor.h"
#include "distance_sensor.h"
#include "parameters.h"
#include "robot_hw.h"

//*****************************************************************************
//
// robot state initialisation
//
//*****************************************************************************
RobotHW::RobotHW() {
}

//*****************************************************************************
//
//
//
//*****************************************************************************
int RobotHW::Init() {
    // Motor setup 
    int err;
    
    // motor intialisation
  	err = motorSetup(&motorL, PIN_L_MOTOR_SPEED, PIN_L_MOTOR_DIR, 1, PWMCH_L_MOTOR);
	if (err) {
        return ERROR_INIT_MOTOR;
    }

	err = motorSetup(&motorR, PIN_R_MOTOR_SPEED, PIN_R_MOTOR_DIR, 1, PWMCH_R_MOTOR);
	if (err) {
        return ERROR_INIT_MOTOR;
    }
	
    // set motors to 0
    motorSetSpeed(&motorL, 0.);
    motorSetSpeed(&motorR, 0.);

    // Encoders initialisation
    setupEncoderR(PIN_L_MOTOR_SA, PIN_L_MOTOR_SB, 1);
    setupEncoderL(PIN_R_MOTOR_SA, PIN_R_MOTOR_SB, 0);

    // Distance sensors setup
	setupSensor1(PIN_SENSORL); // Left sensor
	setupSensor2(PIN_SENSORF); // Front sensor
	setupSensor3(PIN_SENSORR); // Right sensor

    // Wheel encoders setup

    Reset();

    return ERROR_NONE;
}

//*****************************************************************************
//
//
//
//*****************************************************************************
void RobotHW::Reset() {

    // Stop motors

    // reset distance sensors saved values
    resetSensor1();
    resetSensor2();
    resetSensor3();

    // reset encoders saved values
    resetEncoderL();
    resetEncoderR();

}

//*****************************************************************************
//*****************************************************************************
// MOTORS
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//
//*****************************************************************************
void RobotHW::MotorsStop() {
    motorSetSpeed(&motorL, 0.);
    motorSetSpeed(&motorR, 0.);
}

//*****************************************************************************
//
//*****************************************************************************
void RobotHW::MotorsSetSpeed(double motor_speedL, double motor_speedR) {
    motorSetSpeed(&motorL, motor_speedL);
    motorSetSpeed(&motorR, motor_speedR);
}

//*****************************************************************************
//*****************************************************************************
// WHEEL ENCODERS
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//*****************************************************************************
// DISTANCE SENSORS
//*****************************************************************************
//*****************************************************************************



