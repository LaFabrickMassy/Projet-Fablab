//*****************************************************************************
//*****************************************************************************
//
// Simple run
//
//*****************************************************************************
//*****************************************************************************

#include "parameters.h"
#include "motor.h"
#include "micromouse.h"
#include "simple_run.h"

//*****************************************************************************
//
// Initialise simple run
//
//*****************************************************************************
void initSimpleRun() {
    ;
}

void stepSimpleRun() {
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

//*****************************************************************************
//
// Reset simple run :
// set origin to current robot position
//
//*****************************************************************************
void resetSimpleRun() {
    pos_x = 0.;
    pos_y = 0.;
    heading = 0.;
}

//*****************************************************************************
//
// Stop robot
//
//*****************************************************************************
void stopSimpleRun() {
    speed = 0.0;
    turn = 0.0;
    motorSetSpeed(&motorL, 0);
    motorSetSpeed(&motorR, 0);
}
