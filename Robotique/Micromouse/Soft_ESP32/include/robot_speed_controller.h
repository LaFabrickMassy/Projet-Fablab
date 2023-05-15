#ifndef _ROBOT_SPEED_CONTROLLER_H_
#define _ROBOT_SPEED_CONTROLLER_H_

#include <Arduino.h>
#include "motor.h"
#include "wheel_encoder.h"

#define ROBOT_CONTROLLER_WHEEL_SPEED_PID_KP 1.5
#define ROBOT_CONTROLLER_WHEEL_SPEED_PID_KI -0.1
#define ROBOT_CONTROLLER_WHEEL_SPEED_PID_KD 0.

#define ROBOT_CONTROLLER_TARGET_SPEED_PID_KP 1.
#define ROBOT_CONTROLLER_TARGET_SPEED_PID_KI 0.1
#define ROBOT_CONTROLLER_TARGET_SPEED_PID_KD 0.

class RobotSpeedController{
    public:
        RobotSpeedController();
        void Init(encoder_t*, motor_t*, double);
        void Prepare(double target_speed, double target_distance);

        void Stop();
        boolean Step();
        String String_SpeedPID_Params();

        double wheel_speed_pid_kp;
        double wheel_speed_pid_ki;
        double wheel_speed_pid_kd;
        double target_speed_pid_kp;
        double target_speed_pid_ki;
        double target_speed_pid_kd;

        motor_t *p_motor;
        encoder_t *p_encoder;

        // target mean speed and distance on the whole move
        double target_mean_speed;
        double target_distance;
        // target speed during move, can be updated by control
        double current_target_speed;
        // current mean speed
        double current_mean_speed;
        // current speed
        double current_speed;
        // motors commands
        double cmd_motor;

    private:
        double WheelSpeedPID(double target_speed, double current_speed, double dt);
        double TargetSpeedPID(double target_speed, double current_speed, double dt);
        // PID errors
        double sPID_error, tPID_error;
        double sPID_old_error, tPID_old_error;
        // time of run initialisation, in µs
        unsigned long start_time;
        // encoder value at run initialisation
        long init_encoder_count;
        // motor control parameter
        double motor_speed_cmd_K;

        //
        unsigned long step_time, last_step_time;
        unsigned long elapsed_time;
        unsigned long dt;
        double run_distance, last_run_distance;
};

#endif
