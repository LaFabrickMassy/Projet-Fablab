
#include <Arduino.h>
#include <math.h>
#include "motor.h"
#include "parameters.h"
#include "wheel_encoder.h"
#include "micromouse.h"


//********************************************************************
// Left Encoder
//********************************************************************
encoder_t encoderL;
static long micL; 

//********************************************************************
void IRAM_ATTR isrEncoderL() {
    micL = micros();
    encoderL.elapsed_micros = micL - encoderL.last_micros;
    encoderL.last_micros = micL;
    if (digitalRead(encoderL.pinB)) {
        // B was up, reverse
        encoderL.count--; 
    }
    else {
        // B was not yet up, forward
        encoderL.count++;
    }
}

//********************************************************************
void setupEncoderL(int pinA, int pinB, int flag_reverse) {
	encoderL.pinA = pinA;
    encoderL.pinB = pinB;
    encoderL.flag_reverse = flag_reverse;
    encoderL.count = 0;
	pinMode(pinA, INPUT_PULLDOWN);
	pinMode(pinB, INPUT_PULLDOWN);
	attachInterrupt(pinA, isrEncoderL, RISING);
    encoderL.last_count = 0;
    encoderL.resolution = ENCL_RESOL;
}

//********************************************************************
long countEncoderL() {
	return encoderL.count;
}

//********************************************************************
void resetEncoderL() {
	encoderL.count = 0;
}

//********************************************************************
// Encoder 2
//********************************************************************
encoder_t encoderR;
long lastCountR;
static long micR;

//********************************************************************
//
//********************************************************************
void IRAM_ATTR isrEncoderR()  {
    micR = micros();
    encoderR.elapsed_micros = micR - encoderR.last_micros;
    encoderR.last_micros = micR;
    if (digitalRead(encoderR.pinB)) {
        // B was up, reverse
        encoderR.count--; 
    }
    else {
        // B was not yet up, forward
        encoderR.count++;
    }
}

//********************************************************************
//
//********************************************************************
void setupEncoderR(int pinA, int pinB, int flag_reverse) {
	encoderR.pinA = pinA;
	encoderR.pinB = pinB;
    encoderR.flag_reverse = flag_reverse;
    encoderR.count = 0;
	pinMode(pinA, INPUT_PULLDOWN);
	pinMode(pinB, INPUT_PULLDOWN);
	attachInterrupt(pinA, isrEncoderR, RISING);
    encoderR.last_count = 0;
    encoderR.resolution = ENCR_RESOL;
}

//********************************************************************
//
//********************************************************************
long countEncoderR() {
	return encoderR.count;
}

//********************************************************************
//
//********************************************************************
void resetEncoderR() {
	encoderR.count = 0;

}

//********************************************************************
//********************************************************************
//
// Tachometer
//
//********************************************************************
//********************************************************************

static long countL, countR;
static long delta_countL, delta_countR;
static double speedL, speedR;
static hw_timer_t *tachometer_timer = NULL;

//********************************************************************
//
//********************************************************************
void tachometerInit() {
    // Timer prescaled to 1 µs
    tachometer_timer = timerBegin(0, 80, true);
    // Attach interruption to tachometer
    timerAttachInterrupt(tachometer_timer, &tachometer, true);
    // Set timer period
    timerAlarmWrite(tachometer_timer, TACHOMETER_PERIOD_US, true);
    // enable timer
    timerAlarmEnable(tachometer_timer);

    encoderL.speed_available = false;
    encoderR.speed_available = false;
}

//********************************************************************
//  Compute L and R wheel speed from wheel encoders
//  This function is launched by a timer
//********************************************************************
void IRAM_ATTR tachometer() {
    // Compute distance from last interrupt
    countL = encoderL.count;
    delta_countL =  countL - encoderL.last_count;
    encoderL.last_count = countL;
    countR = encoderR.count;
    delta_countR =  countR - encoderR.last_count;
    encoderR.last_count = countR;

    encoderL.speed_available = true;
    encoderR.speed_available = true;
    
    encoderL.speed = (double)delta_countL * ENCL_RESOL / TACHOMETER_PERIOD_SEC;
    encoderR.speed = (double)delta_countR * ENCR_RESOL / TACHOMETER_PERIOD_SEC;
}
