

#ifndef _WHEEL_ENCODER_H
#define _WHEEL_ENCODER_H

typedef struct {
	int pinA;
    int pinB;
    int flag_reverse;
	long count;
    long last_count;
    long last_micros;
    long elapsed_micros;
    double speed;
    boolean speed_available;
    double resolution;
}
encoder_t;

extern encoder_t encoderL;
extern encoder_t encoderR;

void setupEncoderL(int pinA, int pinB, int flag_reverse);
void setupEncoderR(int pinA, int pinB, int flag_reverse);
void resetEncoderL();
void resetEncoderR();
long countEncoderL();
long countEncoderR();
void tachometerInit();
void IRAM_ATTR tachometer();
#endif