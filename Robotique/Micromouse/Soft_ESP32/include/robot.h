#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <Arduino.h>
#include "parameters.h"
#include "robot_hw.h"
#include "robot_controller.h"


// Dynamic modes
#define ROBOT_DSTATE_STOP 0
#define ROBOT_DSTATE_RUN 1
#define ROBOT_DSTATE_RUN_END 2
#define ROBOT_DSTATE_ROTATE 3
#define ROBOT_DSTATE_ROTATE_END 4
#define ROBOT_DSTATE_TURN 5
#define ROBOT_DSTATE_TURN_END 6
#define ROBOT_DSTATE_CRASH 99


class Robot {
    public:
        Robot();
        void Reset();
        int Init();
        void InitMotors();
        void Stop();
        void Crash();
        void SetMotorsSpeed(double speedL, double speedR);
        void UpdateState();
        
        String JSON_State();
        String String_State();
        // state : x,y,h,speed,...
        RobotHW robot_hw;
        RobotController controller;
        // Dynamic mode : stop, run, run_end, rotate, rotate_end, turn, turn_end
        int dynamic_state;
    private:
        // (x,y) position
        double x;
        double y;
        // heading
        double h;
        // speed
        double speed;
        // direction : north, east, south, west
        int direction;
        // current cell
        int cur_cell_i, cur_cell_j;

        long encoderL_lastcount;
        long encoderR_lastcount;
};

#endif