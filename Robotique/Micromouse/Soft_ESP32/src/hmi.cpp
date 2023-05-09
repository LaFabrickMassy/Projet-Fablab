#include <Arduino.h>
#include "test_drive.h"
#include "parameters_calibration.h"
#include "explore.h"
#include "logutils.h"
#include "hmi.h"

int hmi_mode;

//********************************************************************
// 
//********************************************************************
void hmiInit() {
    hmi_mode =  HMI_MODE_INDEX;

}

//********************************************************************
// 
//********************************************************************
void hmiHandleWSMessage(String message) {

    logWrite("HMI handle message "+message+", hmi mode="+String(hmi_mode));
    // handle page reload
    if (message == "page_index") {
        logWrite("page INDEX");
        hmi_mode = HMI_MODE_INDEX;
        testDriveStop();
        paramCalStop();
        exploreStop();
    }
    else if (message == "page_td") {
        logWrite("page Test Drive");
        hmi_mode = HMI_MODE_TD;
        testDriveInit();
    }
    else if (message == "page_pc") {
        logWrite("page Parameters Calibration");
        hmi_mode = HMI_MODE_PC;
    }
    else if (message == "page_ex") {
        logWrite("page Exploration");
        hmi_mode = HMI_MODE_TD;
    }
    else if( hmi_mode == HMI_MODE_INDEX ) {
        // index -> Test Drive ****************************
        if (message == "IDX_TD") {
            // set mode to simple run
            hmi_mode = HMI_MODE_TD;
            testDriveInit();
        }

        // index -> Parameter Calibration *****************
        else if ( message == "IDX_PC" ) {
            hmi_mode = HMI_MODE_PC;
            paramCalInit();
        }

        else {
            logWrite("HMI mode index, unknown message "+String(message)+"");
        }
    }

    // Simple run ********************************
    else if ( hmi_mode == HMI_MODE_TD ) {
        testDriveHandleMessage(message);

    } // Simple run commands

    // Parameters calibration *****************************
    else if ( hmi_mode == HMI_MODE_PC) {
        paramCalHandleMessage(message);
    }

    else {
        logWrite("Unknown hmi_mode "+String(hmi_mode));
    } 
}

//********************************************************************
// 
//********************************************************************
void hmiStep() {

    // get WS message

    // manage message

    switch(hmi_mode) {
        case HMI_MODE_INDEX:
            break;
        case HMI_MODE_TD:
            testDriveStep();
            break;
        case HMI_MODE_PC:
            paramCalStep();
            break;
        case HMI_MODE_EX:
            exploreStep();
            break;
        default:
            #if TRACE_LEVEL >= 2
            logWrite("loop: Unknown mode: "+String(current_mode));
            #endif
            break;
    }
}