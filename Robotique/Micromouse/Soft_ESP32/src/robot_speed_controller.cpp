#include <Arduino.h>
#include <Arduino_Json.h>
#include "errors.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "robot_speed_controller.h"
#include "logutils.h"
#include "parameters.h"

//*****************************************************************************
//
//*****************************************************************************
RobotSpeedController::RobotSpeedController() {
}

//*****************************************************************************
//
//*****************************************************************************
void RobotSpeedController::Init(encoder_t * encoder_p, motor_t * motor_p, double motor_speed_cmd_K_p) {
    p_encoder = encoder_p;
    p_motor = motor_p;
    motor_speed_cmd_K = motor_speed_cmd_K_p;

    // set PID parameters
    wheel_speed_pid_kp = ROBOT_CONTROLLER_WHEEL_SPEED_PID_KP;
    wheel_speed_pid_ki = ROBOT_CONTROLLER_WHEEL_SPEED_PID_KI;
    wheel_speed_pid_kd = ROBOT_CONTROLLER_WHEEL_SPEED_PID_KD;
    target_speed_pid_kp = ROBOT_CONTROLLER_TARGET_SPEED_PID_KP;
    target_speed_pid_ki = ROBOT_CONTROLLER_TARGET_SPEED_PID_KI;
    target_speed_pid_kd = ROBOT_CONTROLLER_TARGET_SPEED_PID_KD;

    }

//*****************************************************************************
//
//*****************************************************************************
double RobotSpeedController::WheelSpeedPID(double target_speed, double current_speed, double dt) {
    double up, ui, ud;
    sPID_error = target_speed - current_speed;
    up = 0.00001* wheel_speed_pid_kp * sPID_error;
    ui = 0.0000000001* wheel_speed_pid_ki * (sPID_error + sPID_old_error) * dt;
    ud = 0.0000000001* wheel_speed_pid_kd * (sPID_error - sPID_old_error) / dt;
    sPID_old_error = sPID_error;

    return up+ui+ud;

}

//*****************************************************************************
//
//*****************************************************************************
double RobotSpeedController::TargetSpeedPID(double target_speed, double current_speed, double dt) {
    double up, ui, ud;
    sPID_error = target_speed - current_speed;
    up = 0.001* target_speed_pid_kp * sPID_error;
    ui = 0.00000001* target_speed_pid_ki * (sPID_error - sPID_old_error) * dt;
    ud = 0.00000001* target_speed_pid_kd * (sPID_error - sPID_old_error) / dt;
    tPID_old_error = tPID_error;

    return up+ui+ud;

}

//*****************************************************************************
//
//*****************************************************************************
void RobotSpeedController::Prepare(double mean_speed_p, double distance_p) {

    // Set run objectives
    target_mean_speed = mean_speed_p;
    target_distance = distance_p;

    // motor control
    cmd_motor = target_mean_speed*motor_speed_cmd_K;

    // set initialisation values
    init_encoder_count = p_encoder->count;
    start_time = micros();
}

//*****************************************************************************
//
//*****************************************************************************
void RobotSpeedController::Stop() {

    #define MINIMAL_SPEED 50
    if(abs(p_encoder->speed) < MINIMAL_SPEED) {
        return;
    }

    last_step_time = micros();

    while (true) {
        if (!p_encoder->speed_available) {
            if(abs(p_encoder->speed) < MINIMAL_SPEED) {
                return;
            }
            current_speed = p_encoder->speed;
            p_encoder->speed_available = false;
            step_time = micros();
            dt = step_time - last_step_time;
            cmd_motor += WheelSpeedPID(0, current_speed, dt);
        }
    }
}

//*****************************************************************************
//
//*****************************************************************************
boolean RobotSpeedController::Step() {
    step_time = micros();
    elapsed_time = step_time - start_time;
    dt = step_time - last_step_time;
    run_distance = (double)(p_encoder->count - init_encoder_count)*p_encoder->resolution;

    // force start
    if (run_distance == 0) {
        motorSetSpeed(p_motor, cmd_motor);
        return false;
    }

    // stop if finished
    if (abs(run_distance) >= abs(target_distance)) {
        return true;
    }

    // if nothing new, abort step
    if (!p_encoder->speed_available) {
        return false;
    }

    // update target speed
    current_mean_speed = 1000000*run_distance/elapsed_time;
    //current_target_speed += TargetSpeedPID(target_mean_speed, current_mean_speed, dt);
    // #### DEBUG #####
    current_target_speed = target_mean_speed;
    
    // get current speed
    current_speed = p_encoder->speed;
    p_encoder->speed_available = false;
    
    // update motor command with speed PID
    cmd_motor += WheelSpeedPID(current_target_speed, current_speed, dt);
    // Chack motor command bounds
    if (cmd_motor > MOTOR_CMD_MAX)
        cmd_motor = MOTOR_CMD_MAX;
    else if (cmd_motor < -MOTOR_CMD_MAX)
        cmd_motor = -MOTOR_CMD_MAX;
    motorSetSpeed(p_motor, cmd_motor);

    // update step values
    last_step_time = step_time;
    last_run_distance = run_distance;

    return false;
}

//*****************************************************************************
//
//*****************************************************************************
String RobotSpeedController::String_SpeedPID_Params() {

    String s;
    
    s = "";
    s += "kp:"+String( (double)((int)(wheel_speed_pid_kp*10000.))/100.);
    s += " / ";
    s += "ki:"+String( (double)((int)(wheel_speed_pid_ki*10000.))/100.);
    s += " / ";
    s += "kd:"+String( (double)((int)(wheel_speed_pid_kd*10000.))/100.);

    return s;

}

