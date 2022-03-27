//******************************************************
// Motor : library to handle motors
//******************************************************

#include "motor.h"

#define MOTOR_DIR_FWD 1
#define MOTOR_DIR_REV 0

#define MOTOR_PWM_FREQ 5000
#define MOTOR_PWM_RES  10

//********************************************************************
//
//********************************************************************
Motor::Motor()
{
}

//********************************************************************
// Setup motor : 
//
// @param pin_speed ID of pin connected to PWM input of controler
// @param pin_speed ID of pin connected to DIR input of controler
// @param flag_reverse 1 to reverse motor direction, 0 otherwise
// @param channel PWM channel used (0..15)
//
// @return 0 if no error
int Motor::Setup(int pin_speed, int pin_dir, int flag_reverse, int channel)
{
	double ledc_retval;
	
	_pin_speed = pin_speed;
	_pin_dir = pin_dir;
	_flag_reverse = flag_reverse;
	_channel = channel;
	
	// Set pin mode
	pinMode(pin_speed, OUTPUT);
	pinMode(pin_dir, OUTPUT);
	digitalWrite(pin_dir, flag_reverse);
	
	// initialise PWM
	ledc_retval = ledcSetup(channel, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
	
	#ifdef DEBUG
	Serial.print("ledcSetup return value : ");
	Serial.println(ledc_retval);
	#endif
	
	if(ledc_retval == 0)
	{
		#ifdef DEBUG
		Serial.println("Error while initialising PWM for motor");
		#endif
		
		return 1; // error while ledcSetup
	}
	
	ledcAttachPin(pin_speed, channel);
	
	_speed_full_res_coeff = (1<<MOTOR_PWM_RES) -1;
	
	return 0;
}

//********************************************************************
// Set speed of motor 
//
// @param relspeed speed between 0 and 1
void Motor::SetSpeed(float relspeed)
{
	uint32_t val;
	
	_cur_speed = relspeed;
	
	if (val > 1)
		val = 1;
	else if (val < 0)
		val = 0;
	
	val = (uint32_t)(relspeed * _speed_full_res_coeff);
	
	#ifdef DEBUG
	Serial.print("Motor set to pwm=");
	Serial.print(val);
	Serial.print(" dir=");
	Serial.println(_cur_dir);
	#endif
	ledcWrite(_channel, val);
}

//********************************************************************
// Change directon of motor. Speed shall be 0.
// 
// @return 1 if motor speed was not 0
int Motor::ChangeDir()
{
	if (_cur_speed == 0)
	{
		_cur_dir = _cur_dir ^ 1;
		digitalWrite(_pin_dir, _cur_dir ^ _flag_reverse);
		#ifdef DEBUG
		Serial.print("ChangeDir OK, dir =");
		Serial.print(_cur_dir);
		#endif
		return 0;
	}
	else
	{
		#ifdef DEBUG
		Serial.print("ChangeDir cancelled, speed = ");
		Serial.print(_cur_speed);
		#endif
		return 1; // change dir allowed only when stopped
	}
}
