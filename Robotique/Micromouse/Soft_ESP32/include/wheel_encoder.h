

#ifndef WHEEL_ENCODER_H
#define WHEEL_ENCODER_H

typedef struct {
	int pinA;
    int pinB;
    int flag_reverse;
	long count;
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