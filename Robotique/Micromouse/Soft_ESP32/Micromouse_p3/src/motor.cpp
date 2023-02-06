//******************************************************
// Motor : library to handle motors
//******************************************************

#include <Arduino.h>
#include "motor.h"

#define MOTOR_DIR_FWD 1
#define MOTOR_DIR_REV 0

#define MOTOR_PWM_FREQ 5000
#define MOTOR_PWM_RES  10

//********************************************************************
// Setup motor : 
//
// @param pin_speed ID of pin connected to PWM input of controler
// @param pin_speed ID of pin connected to DIR input of controler
// @param flag_reverse 1 to reverse motor direction, 0 otherwise
// @param channel PWM channel used (0..15)
//
// @return 0 if no error
int motorSetup(motor_t *motor, int pin_speed, int pin_dir, int flag_reverse, int channel)
{
	double ledc_retval;
	
	motor->pin_speed = pin_speed;
	motor->pin_dir = pin_dir;
	motor->flag_reverse = flag_reverse;
	motor->channel = channel;
	
	// Set pin mode
	pinMode(pin_speed, OUTPUT);
	pinMode(pin_dir, OUTPUT);
	digitalWrite(pin_dir, flag_reverse);
	
	// initialise PWM
	ledc_retval = ledcSetup(channel, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
	
	if(ledc_retval == 0)
	{
		return 1; // error while ledcSetup
	}
	
	ledcAttachPin(pin_speed, channel);
    ledcWrite(channel, (uint32_t)0);
	
	motor->speed_full_res_coeff = (1<<MOTOR_PWM_RES) -1;
	
	return 0;
}

//********************************************************************
// Set speed of motor 
//
// @param relspeed speed between 0 and 1
void motorSetSpeed(motor_t *motor, float relspeed)
{
	uint32_t val;
	
	motor->cur_speed = relspeed;
	
	val = (uint32_t)(relspeed * motor->speed_full_res_coeff);
	
	ledcWrite(motor->channel, val);
}

//********************************************************************
// Set directon of motor. Speed shall be 0 if direction changes.
// 
// @param dir direction (0 ahead, 1 reverse)
// @return 1 if motor speed was not 0
int motorSetDir(motor_t *motor, int reverse)
{
    if (reverse != motor->cur_dir)
        if (motor->cur_speed == 0)
        {
            motor->cur_dir = reverse;
            Serial.print("Motor dir: ");
            Serial.println(reverse);
            digitalWrite(motor->pin_dir, reverse ^ motor->flag_reverse);
            return 0;
        }
        else
        {
            #ifdef DEBUG
            Serial.print("motorSetDir cancelled, speed = ");
            Serial.println(motor->cur_speed);
            #endif
            return 1; // change dir allowed only when stopped
        }
    else
        return 0;
}

//********************************************************************
// Change directon of motor. Speed shall be 0.
// 
// @return 1 if motor speed was not 0
int motorChangeDir(motor_t *motor)
{
	if (motor->cur_speed == 0)
	{
		motor->cur_dir = motor->cur_dir ^ 1;
		digitalWrite(motor->pin_dir, motor->cur_dir ^ motor->flag_reverse);
		#ifdef DEBUG
		Serial.print("ChangeDir OK, dir =");
		Serial.print(motor->cur_dir);
		#endif
		return 0;
	}
	else
	{
		#ifdef DEBUG
		Serial.print("ChangeDir cancelled, speed = ");
		Serial.print(motor->cur_speed);
		#endif
		return 1; // change dir allowed only when stopped
	}
}
