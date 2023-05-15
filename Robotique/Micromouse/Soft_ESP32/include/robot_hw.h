#ifndef _ROBOT_STATE_H_
#define _ROBOT_STATE_H_

#include "motor.h"

#define ROBOT_DIRECTION_UNDEF 0
#define ROBOT_DIRECTION_NORTH 1
#define ROBOT_DIRECTION_EAST  2
#define ROBOT_DIRECTION_SOUTH 3
#define ROBOT_DIRECTION_WEST  4

#define ROBOT_STATE 


class RobotHW {
    public:
        RobotHW();
        int Init();
        void Reset();
        void ComputeMove();

        // Motors 
        void MotorsStop();
        void MotorsSetSpeed(double, double);

        // Wheel encoders

        // distance sensors

        // motors
        motor_t motorL;
        motor_t motorR;




    private:
        long encoderL_lastcount, encoderR_lastcount;

};

#endif