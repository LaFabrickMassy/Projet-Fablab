

#ifndef WHEEL_ENCODER_H
#define WHEEL_ENCODER_H

typedef struct {
	int pinA;
    int pinB;
	long count;
}
encoder_t;

extern encoder_t encoderL;
extern long lastCountL;
extern encoder_t encoderR;
extern long lastCountR;

#endif