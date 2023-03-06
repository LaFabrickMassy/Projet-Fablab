#include <Arduino.h>
#include "motor.h"
#include "wheel_encoder.h"
#include "logutils.h"
#include "parameters.h"
#include "micromouse.h"

long start_encvalueL;
long start_encvalueR;

long nbticks_step1L;
long nbticks_step1R;
long nbticks_step2L;
long nbticks_step2R;
long nbticks_targetL;
long nbticks_targetR;

// Speed up and speed down parameters
double alpha_upL;
double alpha_upR;
double beta_upL;
double beta_upR;
double alpha_dnL;
double alpha_dnR;
double beta_dnL;
double beta_dnR;

int dirL;
int dirR;
double speed_targetL;
double speed_targetR;

#ifdef DEBUG
double debug_nbticksL;
double debug_nbticksR;
#endif

//*****************************************************************************
//
// initRotation
//
// pre-compute rotation parameters
// 
//*****************************************************************************
void rotationInit(double degrees, double speed) {

    double ticks_to_runL;
    double ticks_to_runR;

    // encoder values at initialisation
    start_encvalueL = encoderL.count;
    start_encvalueR = encoderR.count;

    // nb of encoder ticks to reach the target
    ticks_to_runL = WHEEL_DISTANCE/2.*degrees*PI/180./ENCL_RESOL;
    ticks_to_runR = WHEEL_DISTANCE/2.*degrees*PI/180./ENCR_RESOL;

    if (degrees > 0) { 
        // counterclockwise rotation
        dirL = -1;
        dirR = 1;
    }
    else { 
        // clockwise rotation
        dirL = 1;
        dirR = -1;
        // nb of encoder ticks to reach the target
        ticks_to_runL = -ticks_to_runL;
        ticks_to_runR = -ticks_to_runL;
    }


    // target encoder values at end
    nbticks_targetL = (long)(ticks_to_runL);
    nbticks_targetR = (long)(ticks_to_runR);

    // target encoder values at 1st step (end of accelleration)
    nbticks_step1L = (long)(SPEEDUP_PART*ticks_to_runL);
    nbticks_step1R = (long)(SPEEDUP_PART*ticks_to_runR);

    // target encoder values at 2nd step (beginning of decelleration)
    nbticks_step2L = (long)((1.-SPEEDUP_PART)*ticks_to_runL);
    nbticks_step2R = (long)((1.-SPEEDUP_PART)*ticks_to_runR);

    // speed-up parameters
    alpha_upL = (speed - SPEED_MIN)/(SPEEDUP_PART*ticks_to_runL);
    alpha_upR = (speed - SPEED_MIN)/(SPEEDUP_PART*ticks_to_runR);
    beta_upL = SPEED_MIN;
    beta_upR = SPEED_MIN;
    // speed-down parameters
    alpha_dnL = (SPEED_MIN-speed)/(SPEEDUP_PART*ticks_to_runL);
    alpha_dnR = (SPEED_MIN-speed)/(SPEEDUP_PART*ticks_to_runR);
    beta_dnL= (speed-(1.-SPEEDUP_PART)*SPEED_MIN)/SPEEDUP_PART;
    beta_dnR= (speed-(1.-SPEEDUP_PART)*SPEED_MIN)/SPEEDUP_PART;

    speed_targetL = speed;
    speed_targetR = speed;

    #ifdef DEBUG
    debug_nbticksL = 0;
    debug_nbticksR = 0;
    #endif
}

//*****************************************************************************
//
// x degrees rotation to the left
//
// turn on place, counterclockwize, until encoders reach target values
// of wheel_distance/2*degrees*PI/180/encoder_resolution
//
// return 1 if rotation finished, 0 else
//*****************************************************************************
int rotationStep() {
    
    long nbticksL;
    long nbticksR;
    double speedL;
    double speedR;

    if (dirL > 0)
        nbticksL = encoderL.count - start_encvalueL;
    else
        nbticksL = start_encvalueL - encoderL.count;
    if (dirR > 0)
        nbticksR = encoderR.count - start_encvalueR;
    else
        nbticksR = start_encvalueR - encoderR.count;

    #ifdef DEBUG
    nbticksL = (long)debug_nbticksL;
    nbticksR = (long)debug_nbticksR;
    #endif

    debug_message = " mL=";

    if (nbticksL <= 0) {
        debug_message += "-";
        speedL = SPEED_MIN;
    }
    else if (nbticksL < nbticks_step1L) {
        // speed_up
        debug_message += "A";
        speedL = alpha_upL*(double)nbticksL+beta_upL;
    }
    else if (nbticksL < nbticks_step2L) {
        // normal rotation
        debug_message += "B";
        speedL = speed_targetL;
    }
    else if (nbticksL < nbticks_targetL) {
        // speed down
        debug_message += "C";
        speedL = alpha_dnL*(double)nbticksL+beta_dnL;
    }
    else {
        debug_message += "+";
        speedL = 0;
    }
    debug_message += " eL=";
    debug_message += String(nbticksL);
    debug_message += "/";
    debug_message += String(nbticks_targetL);
    debug_message += " sL=";
    debug_message += String(dirL*speedL);
    debug_message += " mR=";

    if (nbticksR <= 0) {
        debug_message += "-";
        speedR = SPEED_MIN;
    }
    else if (nbticksR < nbticks_step1R) {
        // speed_up
        debug_message += "A";
        speedR = alpha_upR*(double)nbticksL+beta_upR;
    }
    else if (nbticksR < nbticks_step2R) {
        // normal rotation
        debug_message += "B";
        speedR = speed_targetR;
    }
    else if (nbticksR < nbticks_targetR) {
        // speed down
        debug_message += "C";
        speedR = alpha_dnR*(double)nbticksL+beta_dnR;
    }
    else {
        debug_message += "+";
        speedR = 0;
    }
    debug_message += " eR=";
    debug_message += String(nbticksR);
    debug_message += "/";
    debug_message += String(nbticks_targetR);
    debug_message += " sR=";
    debug_message += String(dirR*speedR);

    if (nbticksL >= nbticks_targetL || nbticksR >= nbticks_targetR) {
        motorSetSpeed(&motorL, 0);
        motorSetSpeed(&motorR, 0);
        return 1;
    }
    else {
        motorSetSpeed(&motorL, dirL*speedL);
        motorSetSpeed(&motorR, dirR*speedR);
    }
    return 0;
}