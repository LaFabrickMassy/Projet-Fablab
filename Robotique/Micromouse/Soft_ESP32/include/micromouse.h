#ifndef _MICROMOUSE_H_

#define _MICROMOUSE_H_

extern const char* ssid;
extern const char* password;

#ifdef _MAZE_H_
extern Maze maze;
#endif
#ifdef _ROBOT_H_
extern Robot robot;
#endif

#endif