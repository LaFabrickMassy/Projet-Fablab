//******************************************************
// motorcontrol : functions to control motors from speed and turn ratio
//
// Uses following global variables
// motor_t l_motor : left motor
// motor_t r_motor : right motor
// float speed : relative speed, from 0 to 1
// float turn : relative turn, from -1 to 1
//      -1 : turn on place, to left. left wheel full reverse speed, right wheel full speed
//    -0.5 : turn left with left wheel stopped, right wheel full speed
//       0 : straight ahead
//     0.5 : turn right with right wheel stopped, left wheel full speed
//       1 : turn on place, to right. right wheel full reverse speed, left wheel full speed
//******************************************************


void motorControl()
{
    // relative left and right speeds from turn value
    float rel_speedL;
    float rel_speedR;
    int   reverseL;
    int   reverseR;
    
    // compute relative left and right speeds from turn values
    if (turn < 0) // turn left
    {
        rel_speedR = 1.;
        rel_speedL = 1. + 2.*turn;
    }
    else if (turn > 0) // turn right
    {
        rel_speedL = 1.;
        rel_speedR = 1. - 2.*turn;
    }
    else // ahead
    {
        rel_speedL = 1.;
        rel_speedR = 1.;
    }

    if (rel_speedL < 0)
    {
        reverseL = 1;
        rel_speedL = -rel_speedL;
    }
    else
        reverseL = 0;
    
    if (rel_speedR < 0)
    {
        reverseR = 1;
        rel_speedR = -rel_speedR;
    }
    else
        reverseR = 0;
    
    motorSetDir(&motorL, reverseL);
    motorSetSpeed(&motorL, speed*rel_speedL);
    motorSetDir(&motorR, reverseR);
    motorSetSpeed(&motorR, speed*rel_speedR);
}
