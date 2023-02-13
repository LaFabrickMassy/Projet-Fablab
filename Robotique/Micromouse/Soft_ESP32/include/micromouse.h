#ifndef _MICROMOUSE_H_

#define _MICROMOUSE_H_

extern int current_mode;
extern int current_state;

extern motor_t motorL; // left motor
extern motor_t motorR; // right motor

extern const char* ssid;
extern const char* password;

extern double speed;
extern double turn;
extern double pos_x;
extern double pos_y;
extern double heading;
extern double mov_x, mov_y;
extern double mov_h; 


#endif