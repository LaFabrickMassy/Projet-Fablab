//*****************************************************************************
//*****************************************************************************
//
// exploration
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include <math.h>
#include "parameters.h"
#include "maze.h"
#include "robot.h"
#include "logutils.h"
#include "explore.h"
#include "micromouse.h"

int run_stabilised;

int ex_state;

//*****************************************************************************
//
// Initialisation of exploration :
//
//*****************************************************************************
void exploreInit() {
    robot.Stop();
    ex_state = EX_STATE_STOP;
    run_stabilised = 0;

}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void exploreStop() {
    robot.Stop();
    ex_state = EX_STATE_STOP;
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void exploreCrash() {
    robot.Crash();
    ex_state = EX_STATE_CRASH;
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void exploreStep() {
    switch(ex_state) {
        case EX_STATE_STOP:
            break;
        case EX_STATE_CRASH:
            break;
        case EX_STATE_RUN:
            exploreRunStep();
            break;
        case EX_STATE_RUN_END:
            exploreStop();
            break;

    }

}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void exploreRunStep() {

    // Update robot state
    robot.UpdateState();

    if (ex_state == EX_STATE_RUN) {
        // Check for walls

        // Update maze knowledge

        // define movement

    }
    // Update control command
















    // front distance
    double fwall_dist; 
    // left and right distance
    double lwall_dist, rwall_dist;

    // if no sensor update, terminate here
    if (!sensor1.newvalue && !sensor2.newvalue && !sensor3.newvalue) {
        return;
    }

    fwall_dist = distanceSensor2();

    // Check front wall distance for crash
    if ( fwall_dist < WALL_DISTANCE_FMIN ) {

        // stop robot and set state
        robot.Crash();
        return;
    }


    if (fwall_dist < 300) {
        // update position
    }
    if (!run_stabilised) {
        // not stabilised : use PID with distance sensors
        ;
    }
    else {
        // stabilised, check if between walls or not
        ;
    }
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void exploreHandleMessage(char *message) {
    ;
}

