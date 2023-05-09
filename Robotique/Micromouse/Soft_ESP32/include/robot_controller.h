#ifndef _ROBOT_CONTROLLER_H_
#define _ROBOT_CONTROLLER_H_

#define DEBUG_ROBOT_CONTROLLER 1

#include "distance_sensor.h"
#include "motor.h"
#include "wheel_encoder.h"
#include "robot_hw.h"

#define ROBOT_SPEED_MAX 250 // max robot speed in mm/s

// Coefficient motor control -> speed
#define ROBOT_CONTROLLER_MOTOR_KL 0.0025
#define ROBOT_CONTROLLER_MOTOR_KR 0.0025

#define ROBOT_CONTROLLER_SPEEDPID_KP 0.
#define ROBOT_CONTROLLER_SPEEDPID_KI 0.
#define ROBOT_CONTROLLER_SPEEDPID_KD 0.

// Speed -> motor cmd
// 35 :  0.1/0.1
// 38 :  0.11/0.11
// 43 :  0.12/0.12
// 47 :  0.13/0.13
// 52 :  0.14/0.14
// 57 :  0.15/0.15
// 61 :      /0.16
// 62 :  0.16
// 65 :      /0.17
// 66 :  0.17
// 70 :      /0.18
// 71 :  0.18
// 78 :      /0.20
// 80 :  0.20
// 100 :     /0.25
// 103 : 0.25
// 120 :     /0.30
// 124 : 0.30
// 143 :     /0.35
// 148 : 0.35
// 164 :     /0.40
// 171 : 0.40
// 185 :     /0.45
// 192 : 0.45
// 207 :     /0.50
// 217 : 0.50
// 227 :     /0.55
// 237 : 0.55
// 248 :     /0.60
// 258 : 0.60
// 268 :     /0.65
// 280 : 0.65
// 291 :     /0.7
// 298 : 0.70
class RobotController{
    public:
        RobotController();
        int Init();
        void Reset();
        void Stop();
        double SpeedPID(double speed_target, double speed_current);
        void RunInit(double speed, double distance);
        void RunUpdateDistance(double distance);
        boolean RunStep();
        void TurnInit(double speed, double radius, double angle);
        boolean TurnStep();
        void RotateInit(double speed, double angle);
        boolean RotateStep();
        void ComputeMove();
        String JSON_SpeedPID_Params(); 
        String String_SpeedPID_Params();
        String JSON_ControllerStatus();
        String String_ControllerStatus();
        String JSON_ControlHistory();

        RobotHW robot_hw;
        double speedpid_kp;
        double speedpid_ki;
        double speedpid_kd;
        // target speed during move
        double target_speedL, target_speedR;
        // motors commands
        double cmd_motorL, cmd_motorR;
    private:
        // state 
        double state_x, state_y, state_h, state_speed;
        // sequence times
        unsigned long start_time; // beginning of sequence
        unsigned long loop_time; // current loop
        unsigned long last_loop_time; // last loop
        long elapsed_time;
        // target and run distance
        double target_distanceL, target_distanceR;
        double run_distanceL, run_distanceR;
        double old_run_distanceL, old_run_distanceR;
        long init_encvalueL, init_encvalueR;
        // target and run angle
        double target_angle, run_angle;
        // 
        double mean_speedL, mean_speedR;
        // 
        #ifdef DEBUG_ROBOT_CONTROLLER
        int loop_step;
        #endif

};

#endif