//*****************************************************************************
//*****************************************************************************
//
// PID calibration
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "parameters.h"
#include "motor.h"
#include "distance_sensor.h"
#include "wheel_encoder.h"
#include "pid.h"
#include "robot_controller.h"
#include "webserver.h"
#include "logutils.h"
#include "parameters_calibration.h"
#include "micromouse.h"

#define SPEED_BASE 0.4

#define STAB_ERRTAB_SIZE 10
#define DISP_ERRTAB_SIZE 1000

// error tab used to check if stabilised
double stabErrorsTab[STAB_ERRTAB_SIZE];
double total_error;
// error tab used to display error
double dispErrorsTab[DISP_ERRTAB_SIZE];
int dispErrorsNb;

int step;
int stabilised;
int fwall_found;
int pcFlagAuto = 0; // re-run after U-turn if set

long ticks_Lwheel;
long ticks_Rwheel;
long refticks_Lwheel;
long refticks_Rwheel;
double fwall_refdist; 
long fwall_refticks_Lwheel;
long fwall_refticks_Rwheel;
double ticksL_per_mm;
double ticksR_per_mm;

double lwall_dist;
double rwall_dist;
double fwall_dist;

// encoder resolution 
long encL_sum;
long encR_sum;
long nb_runs;
long run_length;

// PID parameters tables


//*****************************************************************************
//
// Initialisation of parameter calibration :
//
//*****************************************************************************
void ParamCalInit() {
    current_state = ROBOT_STATE_STOP;

    // Init PID table
    

    // Init encoder resolution values
    encL_sum = 0;
    encR_sum = 0;
    run_length = 0;

    // reset nb of runs
    nb_runs = 0;
}

//*****************************************************************************
//
// Initialisation of run
//
//*****************************************************************************
void ParamCalStop() {
    current_state = ROBOT_STATE_STOP;
    motorSetSpeed(&motorL, 0);
    motorSetSpeed(&motorR, 0);

}

//*****************************************************************************
//
// Initialisation of run
//
//*****************************************************************************
void ParamCalRunInit() {
    int i;

    // Initialise stabilisation error array to 0
    for (i=0;i<STAB_ERRTAB_SIZE;i++)
        stabErrorsTab[i] = 0.0;
    // Initialise display error array
    dispErrorsNb = 0;

    step = 0;
    stabilised = 0;
    total_error = 0.;
    ticks_Lwheel = 0.;
    ticks_Rwheel = 0.;
    fwall_found = 0;
    nb_runs++;

    initPIDSensors();
    initPIDMotors();

    current_state = ROBOT_STATE_RUN;
    motorSetSpeed(&motorL, SPEED_BASE);
    motorSetSpeed(&motorR, SPEED_BASE);
}

//*****************************************************************************
//
// Continue a PID calibration run :
// Update motor speed from sensors
// Stop if wall distance is too low
//
//*****************************************************************************
void ParamCalRunStep() {

    double error;
    double u;
    double speed_l, speed_r;
    int i;

    // get wall distance
    lwall_dist = distanceSensor1();
    fwall_dist = distanceSensor2();
    rwall_dist = distanceSensor3();

    // Check front wall distance
    if ( fwall_dist < WALL_DISTANCE_FMIN ) {

        // stop robot and set state
        motorSetSpeed(&motorL, 0);
        motorSetSpeed(&motorR, 0);
        logWrite("ParamCalRunStep : FWall found");
        current_state = ROBOT_STATE_RUN_END;

        // update encoders parameter values
        encL_sum += encoderL.count - fwall_refticks_Lwheel;
        encR_sum += encoderR.count - fwall_refticks_Rwheel;
        nb_runs++;
        run_length += fwall_dist - fwall_refdist;
        //notifyClients("{\"trailtext\":\""+String(getParamCalEncoderResolution())+"\"}");

        return;
    }

    // Checl wall collision
    if ( (lwall_dist < WALL_DISTANCE_LMIN) || 
         (fwall_dist < WALL_DISTANCE_LMIN) ) {
        motorSetSpeed(&motorL, 0);
        motorSetSpeed(&motorR, 0);
        logWrite("ParamCalRunStep : Wall crash");
        current_state = ROBOT_STATE_CRASH;
        return;
    }

    // compute error
    error = lwall_dist - rwall_dist;

    // update error array and compute total error
    total_error -= stabErrorsTab[0];
    for (i=0;i<STAB_ERRTAB_SIZE-1;i++)
        stabErrorsTab[i] = stabErrorsTab[i+1];
    stabErrorsTab[STAB_ERRTAB_SIZE-1] = abs(error);
    total_error += abs(error);

    // Sample errors to draw graph. 0x3FF -> 1 sample per 1024 loops
    if ((dispErrorsNb < DISP_ERRTAB_SIZE) && ((step & 0x3FF) == 0)) {
        dispErrorsTab[dispErrorsNb] = error;
        dispErrorsNb++;
    }

    // check if now stabilised
    if ((step > STAB_ERRTAB_SIZE) && (total_error < STAB_ERRTAB_SIZE*PIDSENSORS_MAX_ERROR) && (stabilised == 0)) {
        stabilised = 1;
        total_error = 0;
    }

    step++;

    if (stabilised) {
        if ((fwall_found == 0) && (fwall_dist < 1000)) {
            fwall_found = 1;
            fwall_refdist = fwall_dist;
            fwall_refticks_Lwheel = encoderL.count;
            fwall_refticks_Rwheel = encoderR.count;
        }
        /*
        if (fwall_found == 1) {
            ticks_Lwheel = -encoderL.count - fwall_refticks_Lwheel;
            ticks_Rwheel =  encoderR.count - fwall_refticks_Rwheel;
            ticksL_per_mm = 100*(fwall_refdist-fwall_dist) / ticks_Lwheel;
            ticksR_per_mm = 100*(fwall_refdist-fwall_dist) / ticks_Rwheel;
        }
        */
    }
    

    // PID
    u = PIDSensors(error);

    if (u> 0) {
        speed_l = SPEED_BASE * (1-u);
        speed_r = SPEED_BASE;
    }
    else {
        speed_l = SPEED_BASE;
        speed_r = SPEED_BASE * (1+u);
    }

    motorSetSpeed(&motorL, speed_l);
    motorSetSpeed(&motorR, speed_r);

}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void ParamCalRotateInit() {

    if (nb_runs & 1)
        rotationInit(170., 0.5);
    else
        rotationInit(-170., 0.5);
    current_state = ROBOT_STATE_ROTATE;
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void ParamCalRotateStep() {
    int status;

    status = rotationStep();

    if (status) {
        // target angle reached, stop rotation
        current_state = ROBOT_STATE_ROTATE_END;
    }
}

//*****************************************************************************
//
// Return PID values in json format
//
//*****************************************************************************
String getPIDStatus() {
    String jsonString;
    
    jsonString = "{";
    jsonString += "\"mode\":\""+String(current_mode)+"\"";
    jsonString += ",";
    jsonString += "\"state\":\""+String(current_state)+"\"";
    jsonString += ",";
    jsonString += "\"kp\":\""+String(100*pidSensors_kp)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":\""+String(100*pidSensors_ki)+"\"";
    jsonString += ",";
    jsonString += "\"kd\":\""+String(100*pidSensors_kd)+"\"";
    jsonString += ",";
    jsonString += "\"sens_L\":\""+String(distanceSensor1())+"\"";
    #if NB_OF_SENSORS >= 2
    jsonString += ",";
    jsonString += "\"sens_F\":\""+String(distanceSensor2())+"\"";
    #endif
    #if NB_OF_SENSORS >= 3
    jsonString += ",";
    jsonString += "\"sens_R\":\""+String(distanceSensor3())+"\"";
    #endif
    jsonString += ",";
    jsonString += "\"ticks_L\":\""+String(ticks_Lwheel)+"\"";
    jsonString += ",";
    jsonString += "\"ticks_R\":\""+String(ticks_Rwheel)+"\"";
    jsonString += ",";
    jsonString += "\"err\":\""+String(total_error)+"\"";
    jsonString += ",";
    jsonString += "\"enc_L\":\""+String(encoderL.count)+"\"";
    jsonString += ",";
    jsonString += "\"enc_R\":\""+String(encoderR.count)+"\"";
    jsonString += ",";
    jsonString += "\"fdist\":\""+String(fwall_dist)+"\"";
    jsonString += ",";
    jsonString += "\"ref_fdist\":\""+String(fwall_refdist)+"\"";
    jsonString += ",";
    jsonString += "\"tickL_pmm\":\""+String(ticksL_per_mm)+"\"";
    jsonString += ",";
    jsonString += "\"tickR_pmm\":\""+String(ticksR_per_mm)+"\"";
    jsonString += "}";

    return jsonString;
}

//*****************************************************************************
//
// Return PID values in json format
//
//*****************************************************************************
String getTicksStatus() {

    double distance; // distance of measure

    String jsonString;
    
    distance = fwall_refdist - distanceSensor2();
    ticks_Lwheel = encoderL.count - fwall_refticks_Lwheel;
    ticks_Rwheel = encoderR.count - fwall_refticks_Rwheel;
    ticksL_per_mm = 100*distance / ticks_Lwheel;
    ticksR_per_mm = 100*distance / ticks_Rwheel;

    jsonString = "{";
    jsonString += "\"trailtext\":\"";
    jsonString += " encL=";
    jsonString += String(encoderL.count);
    jsonString += " encR=";
    jsonString += String(encoderR.count);
    jsonString += " dist=";
    jsonString += String(distance);
    jsonString += " ticksL=";
    jsonString += String(ticks_Lwheel);
    jsonString += " ticksR=";
    jsonString += String(ticks_Rwheel);
    jsonString += " resL=";
    jsonString += String(ticksL_per_mm);
    jsonString += " resR=";
    jsonString += String(ticksR_per_mm);
    jsonString += "\"";
    jsonString += "}";

    return jsonString;

}
//********************************************************************
// 
//********************************************************************
String getPIDCalParameters() {
    String jsonString;

    jsonString =  "{";
    jsonString += "\"kp\":"+String(pidSensors_kp)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":"+String(pidSensors_ki)+"\"";
    jsonString += ",";
    jsonString += "\"ki\":"+String(pidSensors_ki)+"\"";

    jsonString += "}";
    return jsonString;
}

//********************************************************************
// 
//********************************************************************
String getParamCalEncoderResolution() {
    String s;

    s = "";
    s+= "dist=";
    s+= String(run_length);
    s+= "("+String(nb_runs)+") - ";
    s+= "EncL=";
    s+= String(encL_sum);
    s+= "/";
    s+= String((double)run_length/(double)encL_sum);
    s+= "mm/t - ";
    s+= "EncR=";
    s+= String(encR_sum);
    s+= "/";
    s+= String((double)run_length/(double)encR_sum);
    s+= "mm/t - ";

    return s;
}

//********************************************************************
// 
//********************************************************************
String getDispErrorsTab() {
    int i;

    String s;

    logWrite("### getDispErrorsTab START ########################");
    s = "{\"errGraph\":[";
    for (i=0;i<dispErrorsNb-1;i++)
        s += "\""+ String(dispErrorsTab[i])+ "\",";
    s += "\""+ String(dispErrorsTab[i])+ "\"]}";
    logWrite("### getDispErrorsTab END ########################");
    return s;
}