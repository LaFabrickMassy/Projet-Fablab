//*****************************************************************************
//*****************************************************************************
//
// PID calibration
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include <math.h>
#include "parameters.h"
#include "motor.h"
#include "distance_sensor.h"
#include "wheel_encoder.h"
#include "pid.h"
#include "robot_controller.h"
#include "webserver.h"
#include "logutils.h"
#include "parameters_calibration.h"
#include "robot.h"
#include "maze.h"
#include "micromouse.h"

#define STAB_ERRTAB_SIZE 5
#define DISP_ERRTAB_SIZE 1000

#define ENCODER_RESOLUTION_MEASURE 0

// transition state
int pc_state;

// error buffer to compute smoothed error
#define ERRBUF_SIZE 1
int errorBufferNb;
double errorBuffer[ERRBUF_SIZE];
// error tab used to check if stabilised
double stabErrorsTab[STAB_ERRTAB_SIZE];
double total_error;
// error tab used to display error
double dispErrorsTab[DISP_ERRTAB_SIZE];
int fwallDistsTab[DISP_ERRTAB_SIZE];
double encResTab[DISP_ERRTAB_SIZE];
int dispErrorsNb;
// sensor stats data
#define SENSORS_DATA_SIZE 10000
int sensorsData [SENSORS_DATA_SIZE];
int sensorsDataIdx = 0;

int step;
int stabilised;
int fwall_found;
int wall_detection_count; // nb of wall detection after a hole
int FlagAuto = 0; // re-run after U-turn if set

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
double swall_dist_sum; // sum of l+r wall distance
int swall_dist_nb;

// encoder resolution 
long encL_sum;
long encR_sum;
long nb_runs;
long run_length;

// PID parameters tables

// IHM toggles
static boolean flagauto;
static int paramMode;

//*****************************************************************************
//
// Initialisation of parameter calibration :
//
//*****************************************************************************
void paramCalInit() {
    robot.Stop();
    // Init encoder resolution values
    encL_sum = 0;
    encR_sum = 0;
    run_length = 0;

    // reset nb of runs
    nb_runs = 0;

    //paramSensorsStatsInit();
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void paramCalStop() {
    robot.Stop();
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void paramCalStep() {
    switch(pc_state) {
        case PC_STATE_STOP:
            robot.Stop();
            break;
        case PC_STATE_CRASH:
            robot.Crash();
            break;
        case PC_STATE_RUN:
            paramCalRunStep();
            break;
        case PC_STATE_RUN_END:
            notifyClients(getDispErrorsTab());
            //logSensorsStats();
            paramCalRotateInit();
            break;
        case PC_STATE_ROTATE:
            paramCalRotateStep();
            break;
        case PC_STATE_ROTATE_END:
            if (FlagAuto) {
                paramCalRunInit();
            }
            else {
                paramSensorsStatsInit();
                paramCalStop();
                pc_state = PC_STATE_STOP;
            }
            break;
        default:
            #if TRACE_LEVEL >= 2
            logWrite("loop: Unknown state in mode PARAM: "+String(pc_state));
            #endif
            break;
    }
}
//*****************************************************************************
//
//
//
//*****************************************************************************
void paramCalHandleMessage(String message) {
    logWrite("WS: Mode PARAM");
    if (message == "index") {
        // Set mode to stop
        paramCalStop();
        #if TRACE_LEVEL >= 2
        logWrite("WS: PC Index, ParamCalStop()");
        logWrite(robot.String_State());
        #endif
    }
    else if (message == "start") {
        #if TRACE_LEVEL >= 2
        logSensorsStats();
        logWrite("WS: ParamCalRunInit() === START");
        logWrite(robot.String_State());
        #endif
        // start PID run
        paramCalRunInit();
        #if TRACE_LEVEL >= 2
        logWrite("WS: ParamCalRunInit() === DONE");
        logWrite(robot.String_State());
        #endif
    }
    else if (message == "uturn") {
        #if TRACE_LEVEL >= 2
        logWrite("WS: ParamCalRotateInit() === START");
        logWrite(robot.String_State());
        #endif
        // start PID run
        paramCalRotateInit();
        #if TRACE_LEVEL >= 2
        logWrite("WS: ParamCalRotateInit() === DONE");
        logWrite(robot.String_State());
        #endif
    }
    else if (message == "stop") {
        #if TRACE_LEVEL >= 2
        logWrite("WS: ParamCalStop() === START");
        logWrite(robot.String_State());
        #endif
        // stop PID run
        paramCalStop();
        #if TRACE_LEVEL >= 2
        logWrite("WS: ParamCalStop() === DONE");
        logWrite(robot.String_State());
        #endif
    }
    else if (message == "auto_on") {
        // stop PID run
        FlagAuto = 1;
        #if TRACE_LEVEL >= 2
        logWrite("WS: PC flag_auto set to "+String(FlagAuto)+" =======");
        logWrite(robot.String_State());
        #endif
    }
    else if (message == "auto_off") {
        // stop PID run
        FlagAuto = 0;
        #if TRACE_LEVEL >= 2
        logWrite("WS: PC flag_auto set to "+String(FlagAuto)+" =======");
        logWrite(robot.String_State());
        #endif
    }
    else if (message == "sel_kp") {
        paramMode = PC_SELPARAMMODE_KP;
    }
    else if (message == "sel_ki") {
        paramMode = PC_SELPARAMMODE_KI;
    }
    else if (message == "sel_kd") {
        paramMode = PC_SELPARAMMODE_KD;
    }
    else if (message == "+3") {
        if (paramMode == PC_SELPARAMMODE_KD)
            pidSensors_kd += 0.01;
        else if (paramMode == PC_SELPARAMMODE_KI)
            pidSensors_ki += 0.01;
        else
            pidSensors_kp += 0.01;
        notifyClients(getPIDStatus());     
        logWrite(getPIDStatus());
    }
    else if (message == "+2") {
        if (paramMode == PC_SELPARAMMODE_KD)
            pidSensors_kd += 0.001;
        else if (paramMode == PC_SELPARAMMODE_KI)
            pidSensors_ki += 0.001;
        else
            pidSensors_kp += 0.001;
        notifyClients(getPIDStatus());            
        logWrite(getPIDStatus());
    }
    else if (message == "+1") {
        if (paramMode == PC_SELPARAMMODE_KD)
            pidSensors_kd += 0.0001;
        else if (paramMode == PC_SELPARAMMODE_KI)
            pidSensors_ki += 0.0001;
        else
            pidSensors_kp += 0.0001;
        notifyClients(getPIDStatus());            
        logWrite(getPIDStatus());
    }
    else if (message == "-3") {
        if (paramMode == PC_SELPARAMMODE_KD)
            pidSensors_kd -= 0.01;
        else if (paramMode == PC_SELPARAMMODE_KI)
            pidSensors_ki -= 0.01;
        else
            pidSensors_kp -= 0.01;
        notifyClients(getPIDStatus());            
        logWrite(getPIDStatus());
    }
    else if (message == "-2") {
        if (paramMode == PC_SELPARAMMODE_KD)
            pidSensors_kd -= 0.001;
        else if (paramMode == PC_SELPARAMMODE_KI)
            pidSensors_ki -= 0.001;
        else
            pidSensors_kp -= 0.001;
        notifyClients(getPIDStatus());            
        logWrite(getPIDStatus());
    }
    else if (message == "-1") {
        if (paramMode == PC_SELPARAMMODE_KD)
            pidSensors_kd -= 0.0001;
        else if (paramMode == PC_SELPARAMMODE_KI)
            pidSensors_ki -= 0.0001;
        else
            pidSensors_kp -= 0.0001;
        notifyClients(getPIDStatus());            
        logWrite(getPIDStatus());
    }
    else {
        logWrite("WS: command \'"+String(message)+"\' not available");
    }
}       

//*****************************************************************************
//
//
//
//*****************************************************************************
void paramSensorsStatsInit() {
    int i;
    // reset sensors stats data
    sensorsDataIdx = 0;
    for (i=0;i<SENSORS_DATA_SIZE;i++)
        sensorsData[i] = 0;
}

//*****************************************************************************
//
// Initialisation of run
//
//*****************************************************************************
void paramCalRunInit() {
    int i;

    // initialise error buffer array to 0
    for (i=0;i<ERRBUF_SIZE;i++) {
        errorBuffer[i] = 0.;
    }
    errorBufferNb = 0;

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

    pc_state = PC_STATE_RUN;
    robot.robot_hw.MotorsSetSpeed(MOTOR_CMD_MAX, MOTOR_CMD_MAX);
}

//*****************************************************************************
//
// Continue a PID calibration run :
// Update motor speed from sensors
// Stop if wall distance is too low
//
//*****************************************************************************
void paramCalRunStep() {

    double error;
    double u;
    double speed_l, speed_r;
    int i;

    int meanError;

    // get wall distance
    fwall_dist = distanceSensor2();

    // Check front wall distance
    if ( fwall_dist < WALL_DISTANCE_FMIN ) {

        // stop robot and set state
        robot.Stop();
        logWrite("ParamCalRunStep : FWall found");

        #if ENCODER_RESOLUTION_MEASURE > 0
        // set final values for encoders resolution evaluation
        if (fwall_found == 1) {
            encL_sum += encoderL.count - fwall_refticks_Lwheel;
            encR_sum += encoderR.count - fwall_refticks_Rwheel;
            run_length += fwall_refdist - fwall_dist;
            logWrite("Finished: wall="+String(fwall_dist)+" enc="+String(encoderL.count)+"/"+String(encoderR.count));
            logWrite("Run "+String(fwall_dist - fwall_refdist)+" enc "+String(encoderL.count - fwall_refticks_Lwheel)+"/"+String(encoderR.count - fwall_refticks_Rwheel));
            logWrite("Encoder resolution : L="+String(1000.*(double)run_length/(double)encL_sum)+"µm/tck "+" R="+String(1000.*(double)run_length/(double)encR_sum)+"µm/tck ");
        }
        else
            logWrite("Finished not stabilised");
        #endif

        pc_state = PC_STATE_RUN_END;
        return;
    }

    // get wall distance
    if (!sensor1.newvalue && !sensor3.newvalue) {
        // no updates
        return;
    }
    lwall_dist = distanceSensor1();
    rwall_dist = distanceSensor3();

    // Check wall collision
    if ( (lwall_dist < WALL_DISTANCE_LMIN) || 
         (rwall_dist < WALL_DISTANCE_LMIN) ) {
        robot.Crash();
        logWrite("ParamCalRunStep : Wall crash");
        pc_state = PC_STATE_CRASH;
        return;
    }

    // if not stabilised, use distance sensors
    if (!stabilised) {
        // compute error
        error = lwall_dist - rwall_dist;

        // smooth error : update error buffer 
        if (ERRBUF_SIZE > 0) {
            if (errorBufferNb == 0) {
                // 1st step, fill errorBuffer with the current error
                for (i=0;i<ERRBUF_SIZE;i++) {
                    errorBuffer[i] = error;
                }
            }
            else {
                meanError -= errorBuffer[0];
                for (i=0;i<ERRBUF_SIZE-1;i++) {
                    errorBuffer[i] = errorBuffer[i+1];
                }
                errorBuffer[ERRBUF_SIZE-1] += error;
                meanError += error;
                error = meanError/(double)ERRBUF_SIZE;
            }
        }

        // update error array and compute total error
        total_error -= stabErrorsTab[0];
        for (i=0;i<STAB_ERRTAB_SIZE-1;i++) {
            stabErrorsTab[i] = stabErrorsTab[i+1];
        }
        stabErrorsTab[STAB_ERRTAB_SIZE-1] = abs(error);
        total_error += abs(error);

        // Sample errors to draw graph. 0x3FF -> 1 sample per 1024 loops
        if ((dispErrorsNb < DISP_ERRTAB_SIZE) && ((step & 0) == 0)) {
            dispErrorsTab[dispErrorsNb] = error;
            if (fwall_found) {
                fwallDistsTab[dispErrorsNb] = fwall_dist;
                encResTab[dispErrorsNb] = (encoderL.count - fwall_refticks_Lwheel);
            }
            else {
                fwallDistsTab[dispErrorsNb] = 1000;
                encResTab[dispErrorsNb] = 0.0;
            }
            dispErrorsNb++;
        }

        // check if now stabilised
        if ((step > STAB_ERRTAB_SIZE) && (total_error < STAB_ERRTAB_SIZE*PIDSENSORS_MAX_ERROR) ) {
            stabilised = 1;
            total_error = 0;
            wall_detection_count = 100;
        }


        #if ENCODER_RESOLUTION_MEASURE > 0
        // Set values for encoder resolution evaluation
        if (stabilised) {
            if ((fwall_found == 0) && (fwall_dist < 400) && (fwall_dist > 100)) {
                fwall_found = 1;
                fwall_refdist = fwall_dist;
                fwall_refticks_Lwheel = encoderL.count;
                fwall_refticks_Rwheel = encoderR.count;
                //logWrite("Stabilised: wall="+String(fwall_dist)+" enc="+String(fwall_refticks_Lwheel)+"/"+String(fwall_refticks_Rwheel));
            }
        }
        #endif

        // PID
        u = PIDSensors(error);

        if (u> 0) {
            speed_l = MOTOR_CMD_MAX * (1-u);
            speed_r = MOTOR_CMD_MAX;
        }
        else {
            speed_l = MOTOR_CMD_MAX;
            speed_r = MOTOR_CMD_MAX * (1+u);
        }

        robot.robot_hw.MotorsSetSpeed(speed_l, speed_r);

        step++;

    }
    // if stabilised, use encoders when holes
    else {

        // check if between walls
        if((lwall_dist + rwall_dist) < 2*WALL_DISTANCE_MAX) {
            // between walls
            error = lwall_dist - rwall_dist;
            if (!wall_detection_count) {
                // 1st detection after a hole

                // reset error buffer
                if (ERRBUF_SIZE > 0) {
                    if (errorBufferNb == 0) {
                        // 1st step, fill errorBuffer with the current error
                        for (i=0;i<ERRBUF_SIZE;i++) {
                            errorBuffer[i] = error;
                        }
                    }
                }
                wall_detection_count = 1;
                swall_dist_sum = lwall_dist + rwall_dist;
                swall_dist_nb = 1;
            }
            else {
                wall_detection_count++;
                swall_dist_sum += lwall_dist + rwall_dist;
                swall_dist_nb ++;
                if(wall_detection_count > 5) {
                    // center using distance sensors

                    // smooth error : update error buffer 
                    if (ERRBUF_SIZE > 0) {
                        if (errorBufferNb == 0) {
                            // 1st step, fill errorBuffer with the current error
                            for (i=0;i<ERRBUF_SIZE;i++) {
                                errorBuffer[i] = error;
                            }
                        }
                        else {
                            meanError -= errorBuffer[0];
                            for (i=0;i<ERRBUF_SIZE-1;i++) {
                                errorBuffer[i] = errorBuffer[i+1];
                            }
                            errorBuffer[ERRBUF_SIZE-1] += error;
                            meanError += error;
                            error = meanError/(double)ERRBUF_SIZE;
                        }
                    }

                    // PID
                    u = PIDSensors(error);

                    if (u> 0) {
                        speed_l = MOTOR_CMD_MAX * (1-u);
                        speed_r = MOTOR_CMD_MAX;
                    }
                    else {
                        speed_l = MOTOR_CMD_MAX;
                        speed_r = MOTOR_CMD_MAX * (1+u);
                    }

                    robot.robot_hw.MotorsSetSpeed(speed_l, speed_r);
                }
            }
        }
        else {
            wall_detection_count = 0;

            if (lwall_dist  < swall_dist_sum/(2.*0.9*swall_dist_nb)) {            
                // right hole
                logWrite("Right hole ooooooooooooooooo");
                error = lwall_dist - swall_dist_sum/(2.*swall_dist_nb);

                u = PIDSensors(error);

                if (u> 0) {
                    speed_l = MOTOR_CMD_MAX * (1-u);
                    speed_r = MOTOR_CMD_MAX;
                }
                else {
                    speed_l = MOTOR_CMD_MAX;
                    speed_r = MOTOR_CMD_MAX * (1+u);
                }

                robot.robot_hw.MotorsSetSpeed(speed_l, speed_r);

            }
            else if (rwall_dist  < swall_dist_sum/(2.*0.9*swall_dist_nb)) {            
                // left hole
                logWrite("Left hole ooooooooooooooooo");
                error = - rwall_dist + swall_dist_sum/(2.*swall_dist_nb);

                u = PIDSensors(error);

                if (u> 0) {
                    speed_l = MOTOR_CMD_MAX * (1-u);
                    speed_r = MOTOR_CMD_MAX;
                }
                else {
                    speed_l = MOTOR_CMD_MAX;
                    speed_r = MOTOR_CMD_MAX * (1+u);
                }

                robot.robot_hw.MotorsSetSpeed(speed_l, speed_r);

            }
            else {
                // between two holes, continue straight ahead
                    robot.robot_hw.MotorsSetSpeed(MOTOR_CMD_MAX, MOTOR_CMD_MAX);
            }
        }

    }

}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void paramCalRotateInit() {

    if (nb_runs & 1)
        robot.controller.RotateInit(170., 0.5);
    else
        robot.controller.RotateInit(-170., 0.5);
    pc_state = PC_STATE_ROTATE;
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void paramCalRotateStep() {
    int status;

    status = robot.controller.RotateStep();

    if (status) {
        logWrite("ParamCalrotationStep() : done ----------------");
        // target angle reached, stop rotation
        pc_state = PC_STATE_ROTATE_END;
    }
}

//*****************************************************************************
//
//
//
//*****************************************************************************
void sensorsStatsUpdate() {
    if (sensorsDataIdx < SENSORS_DATA_SIZE -1) {
        sensorsData[sensorsDataIdx] = distanceSensor3() - distanceSensor1();
        sensorsDataIdx++;
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
    jsonString += "\"mode\":\"PC\"";
    jsonString += ",";
    jsonString += "\"state\":\""+String(pc_state)+"\"";
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

    s  = "{\"dataGraph\":{";
    s += "\"errors\":[";
    for (i=0;i<dispErrorsNb-1;i++)
        s += "\""+ String(dispErrorsTab[i])+ "\",";
    s += "\""+ String(dispErrorsTab[i])+ "\"]";
    /*
    s += ",";
    s += "\"wall\":[";
    for (i=0;i<dispErrorsNb-1;i++)
        s += "\""+ String(fwallDistsTab[i])+ "\",";
    s += "\""+ String(fwallDistsTab[i])+ "\"]";
    s += ",";
    s += "\"encr\":[";
    for (i=0;i<dispErrorsNb-1;i++)
        s += "\""+ String(encResTab[i])+ "\",";
    s += "\""+ String(encResTab[i])+ "\"]";
    */
   s += "}}";
    return s;
}

//********************************************************************
// 
//********************************************************************
void logSensorsStats() {
    int i;
    double mean;
    double var;
    double sigma;

    if (sensorsDataIdx > 0) {
        for (i=0;i<sensorsDataIdx;i++)
            mean += (double)sensorsData[i];
        mean /= (double)sensorsDataIdx;
        for (i=0;i<sensorsDataIdx;i++)
            var += ((double)sensorsData[i]-mean)*((double)sensorsData[i]-mean);
        var /= (double)sensorsDataIdx;
    }
    else {
        mean = 0.0;
        var = 0.0;
    }
    sigma = sqrt(var);

    logWrite("n="+String(sensorsDataIdx)+
             "mean: "+String(mean)+
             "var: "+String(var)+
             "sigma: "+String(sigma)
            );
}
