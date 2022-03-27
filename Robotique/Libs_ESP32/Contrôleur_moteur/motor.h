//******************************************************
// Motor : library to handle motors
//******************************************************

#ifndef MOTOR_H
#define MOTOR_H

//********************************************************************
//
//********************************************************************
class Motor
{
	public:
		Motor();
		int Setup(int pin_speed, int pin_dir, int flag_reverse, int channel);
		void SetSpeed(float relspeed);
		int ChangeDir();
		
	private:
		int _pin_speed;
		int _pin_dir;
		int _flag_reverse; 
		int _channel; // channel
		int _speed_full_res_coeff;
		float _cur_speed; // current speed
		int _cur_dir;   // current direction
		static const int _speed_min = 0.05;  // speed when stop -> run
		static const int _speed_step = 1.05; // multiplicator coefficient 
		static const int _speed_max = 0.25;
};

#endif
