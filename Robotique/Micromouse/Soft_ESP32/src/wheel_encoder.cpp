
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
long lastCountL;
long micL;

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
    lastCountL = 0;
}

//********************************************************************
int countEncoderL() {
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
long micR;

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
void setupEncoderR(int pinA, int pinB, int flag_reverse) {
	encoderR.pinA = pinA;
	encoderR.pinB = pinB;
    encoderR.flag_reverse = flag_reverse;
    encoderR.count = 0;
	pinMode(pinA, INPUT_PULLDOWN);
	pinMode(pinB, INPUT_PULLDOWN);
	attachInterrupt(pinA, isrEncoderR, RISING);
    lastCountR = 0;
}

//********************************************************************
int countEncoderR() {
	return encoderR.count;
}

//********************************************************************
void resetEncoderR() {
	encoderR.count = 0;

}

//********************************************************************
// ComputeMove
//********************************************************************
void computeMove() {
    double dL, dR;

    double rL, rR, r; // curvature radius for left wheel, right wheel and center of robot
    double alpha;      // angle along circle
    double distance;   // distance run
    double q;          // quotient of left and right radius
    double temp;
    double ca, sa;     // cos(alpha), sin(alpha)
    double ch, sh;     // cos(h), sin(h)
    
    #define W WHEEL_DISTANCE
    

    if (encoderL.flag_reverse)
        dL = (double)(encoderL.count - lastCountL) * ENCL_RESOL;
    else
        dL = -(double)(encoderL.count - lastCountL) * ENCL_RESOL;
    lastCountL = encoderL.count;
    if (encoderR.flag_reverse)
        dR = (double)(encoderR.count - lastCountR) * ENCR_RESOL;
    else
        dR = -(double)(encoderR.count - lastCountR) * ENCR_RESOL;
    lastCountR = encoderR.count;
    
    
    if (dL > 0.)
    {
        if (dR > 0.)
        {
            if (dL > dR) // Case 1
            {
                q = dL/dR;
                rR = W/(q-1.);
                r = rR+W/2.;
                alpha = -dR / rR;      
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on right
                mov_x = -r * (sh*(1.-ca) - ch*sa);
                mov_y = -r * (ch*(1.-ca) - sh*sa);
				
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 01");
                Serial.print(" dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" q=");
                Serial.print(q);
				Serial.print(" rR=");
                Serial.print(rR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else if (dL == dR) // Case 2
            {
                alpha = 0.;
                r = dL;
                ch = cos(heading);
                sh = sin(heading);

                mov_x = r*ch;
                mov_y = r*sh;

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 02");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else // dL < dR Case 3
            {
                q = dR/dL;
                rL = W/(q-1.);
                r = rL+W/2.;
                alpha = dL / rL;      
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on left
                mov_x = -r * (sh*(1.-ca) + ch*sa);
                mov_y =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 03");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
        }
        else if (dR == 0.) // Case 4
        {
            r = W/2.;
            alpha = -dL/W;
            ch = cos(heading);
            sh = sin(heading);
            ca = cos(alpha);
            sa = sin(alpha);
            
            // O on right
            mov_x = -r * (sh*(1.-ca) - ch*sa);
            mov_y = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 042");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
        }
        else // dR < 0
        {
            if (dL < -dR) // Case 5
            {
                q = dL/dR;
                rR = W/(q+1.);
                r = W/2.-rR;
                alpha = -dR / rR;      
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on right
                mov_x = -r * (sh*(1.-ca) - ch*sa);
                mov_y = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 05");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else if (dL = -dR) // Case 6
            {
				r = 0.;
                alpha = -dL/(W/2.);
                mov_x = 0.;
                mov_y = 0.;
                
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 06");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif

            }
            else // dL > -dR Case 7
            {
                q = dR/dL;
                rL = W/(q+1.);
                r = W/2.-rL;
                alpha = -dL / rL;      
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on left
                mov_x = -r * (sh*(1.-ca) + ch*sa);
                mov_y =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 07");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
        }
    }
    else if (dL == 0)
    {
        if (dR > 0) // Case 8
        {
            r = W/2.;
            alpha = dR/W;
            ch = cos(heading);
            sh = sin(heading);
            ca = cos(alpha);
            sa = sin(alpha);
            
            // O on left
            mov_x = -r * (sh*(1.-ca) + ch*sa);
            mov_y =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 08");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
        }
        else if (dR == 0) // Case 9
        {
			r = 0.;
            alpha = 0.;
            mov_x = 0.;
            mov_y = 0.;
                
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 09");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
        }
        else // dR < 0 Case 10
        {
            r = W/2.;
            alpha = dR/W;
            
            ch = cos(heading);
            sh = sin(heading);
            ca = cos(alpha);
            sa = sin(alpha);
            
            // O on left
            mov_x = -r * (sh*(1.-ca) + ch*sa);
            mov_y =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 10");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
        }
    }
    else // dL < 0
    {
        if (dR > 0)
        {
            if (-dL < dR) // Case 11
            {
                q = dR/dL;
                rL = W/(q+1.);
                r = W/2. - rL;
                alpha = -dL / rL;      
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on left
                mov_x = -r * (sh*(1.-ca) + ch*sa);
                mov_y =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 11");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else if (-dL == dR) // Case 12
            {
                r = 0;
                alpha = -dL/(W/2.);
                mov_x = 0.;
                mov_y = 0.;
                
                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 12");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else // -dL > dR Case 13
            {
                q = dL/dR;
                rR = W/(q+1.);
                r = W/2.-rR;
                alpha = -dR / rR;
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on right
                mov_x = -r * (sh*(1.-ca) - ch*sa);
                mov_y = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 13");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
        }
        else if (dR == 0) // Case 14
        {
            r = W/2.;
            alpha = -dL/W;
            ch = cos(heading);
            sh = sin(heading);
            ca = cos(alpha);
            sa = sin(alpha);
            
            // O on right
            mov_x = -r * (sh*(1.-ca) - ch*sa);
            mov_y = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 14");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
        }
        else // dR < 0 
        {
            if (dL < dR) // Case 15
            {
                q = dL/dR;
                rR = W/(q-1.);
                r = W/2.+rR;
                alpha = -dR / rR;
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on right
                mov_x = -r * (sh*(1.-ca) - ch*sa);
                mov_y = -r * (ch*(1.-ca) - sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 15");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else if (dL = dR) // Case 16
            {
                r = -dL;
                alpha = 0.;
                ch = cos(heading);
                sh = sin(heading);
                
                mov_x = -r*ch;
                mov_y = -r*sh;

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 16");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
            else // dL > dR Case 17
            {
                q = dR/dL;
                rL = W/(q-1.);
                r = rL+W/2.;
                alpha = dL / rL;      
                ch = cos(heading);
                sh = sin(heading);
                ca = cos(alpha);
                sa = sin(alpha);
                
                // O on left
                mov_x = -r * (sh*(1.-ca) + ch*sa);
                mov_y =  r * (ch*(1.-ca) + sh*sa);

                #ifdef DEBUG_MOVE
				Serial.println("+++ Case 17");
				Serial.print("dL=");
				Serial.print(dL);
				Serial.print(" dR=");
				Serial.print(dR);
				Serial.print(" r=");
				Serial.print(r);
				Serial.print(" a=");
				Serial.print(alpha*180/PI);
				Serial.print(" mx=");
				Serial.print(mov_x);
				Serial.print(" my=");
				Serial.print(mov_y);
				Serial.println("");
                #endif
            }
        }
    }
    
    heading += alpha;

    pos_x += mov_x;
    pos_y += mov_y;

	#ifdef DEBUG_MOVE
    Serial.println("########## Compute move");
    Serial.print(" h=");
    Serial.print(heading*180/PI);
    Serial.print(" Pos : x=");
    Serial.print(pos_x);
    Serial.print(" y=");
    Serial.print(pos_y);
    Serial.println("");
	#endif
}

#undef W
