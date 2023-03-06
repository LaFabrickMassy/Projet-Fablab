

#ifndef _WHEEL_ENCODER_H
#define _WHEEL_ENCODER_H

typedef struct {
	int pinA;
    int pinB;
    int flag_reverse;
	long count;
    long last_micros;
    long elapsed_micros;
}
encoder_t;

extern encoder_t encoderL;
extern long lastCountL;
extern encoder_t encoderR;
extern long lastCountR;

void setupEncoderL(int pinA, int pinB, int flag_reverse);
void setupEncoderR(int pinA, int pinB, int flag_reverse);
void resetEncoderL();
void resetEncoderR();
void computeMove();

#endif