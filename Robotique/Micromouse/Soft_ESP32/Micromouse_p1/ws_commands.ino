//******************************************************
// commands.ino
// functions to handle commands from controller
//******************************************************

#define SPEED_MIN 0.1
#define SPEED_FACTOR 1.4
#define TURN_STEP 0.1

//******************************************************
// cmd_speedup
//******************************************************
void cmd_speedup()
{
    if(speed < SPEED_MIN)
        speed = SPEED_MIN;
    else
        speed = speed * SPEED_FACTOR;
    
    if(speed > 1.)
        speed = 1.;
}

//******************************************************
// cmd_speeddown
//******************************************************
void cmd_speeddown()
{
    speed = speed / SPEED_FACTOR;
    if(speed < SPEED_MIN)
        speed = 0.;
}

//******************************************************
// cmd_turnleft
//******************************************************
void cmd_turnleft()
{
    if (turn <= (1.-TURN_STEP))
        turn += TURN_STEP;
}

//******************************************************
// cmd_turnright
//******************************************************
void cmd_turnright()
{
    if (turn >= (-1.+TURN_STEP))
        turn -= TURN_STEP;
}

//******************************************************
// cmd_stop
//******************************************************
void cmd_stop()
{
    speed = 0.0;
    turn = 0.0;
}

//******************************************************
// cmd_home
//******************************************************
void cmd_home()
{

}


