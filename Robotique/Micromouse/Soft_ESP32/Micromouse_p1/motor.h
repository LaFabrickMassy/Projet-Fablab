//******************************************************
// Motor : library to handle motors
//******************************************************

#ifndef MOTOR_H
#define MOTOR_H

typedef struct
{
    int pin_speed;
    int pin_dir;
    int flag_reverse;
    int channel;
    int speed_full_res_coeff;
    float cur_speed;
    int cur_dir;
}
motor_t;


int motorSetup(motor_t *motor, int pin_speed, int pin_dir, int flag_reverse, int channel);

#endif