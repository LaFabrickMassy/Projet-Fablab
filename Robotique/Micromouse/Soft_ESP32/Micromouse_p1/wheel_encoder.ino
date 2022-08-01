
#include <math.h>
#include "parameters.h"
#include "wheel_encoder.h"

#define PI 3,1415926535897932384626433832795

//********************************************************************
// Left Encoder
//********************************************************************
encoder_t encoderL;
long lastCountL;

//********************************************************************
void IRAM_ATTR isrEncoderL() 
{
    if (digitalRead(encoderL.pinB))
    {
        // B was up, reverse
        encoderL.count--; 
        //encoderL.count++; 
    }
    else
    {
        // B was not yet up, forward
        encoderL.count++;
    }
}

//********************************************************************
void setupEncoderL(int pinA, int pinB)
{
	encoderL.pinA = pinA;
    encoderL.pinB = pinB;
    encoderL.count = 0;
	pinMode(pinA, INPUT_PULLDOWN);
	pinMode(pinB, INPUT_PULLDOWN);
	attachInterrupt(pinA, isrEncoderL, RISING);
    lastCountL = 0;
}

//********************************************************************
int countEncoderL()
{
	return encoderL.count;
}

//********************************************************************
void resetEncoderL()
{
	encoderL.count = 0;
}

//********************************************************************
// Encoder 2
//********************************************************************
encoder_t encoderR;
long lastCountR;

//********************************************************************
void IRAM_ATTR isrEncoderR() 
{
    if (digitalRead(encoderR.pinB))
    {
        // B was up, reverse
        encoderR.count--; 
        //encoderR.count++; 
    }
    else
    {
        // B was not yet up, forward
        encoderR.count++;
    }
}

//********************************************************************
void setupEncoderR(int pinA, int pinB)
{
	encoderR.pinA = pinA;
	encoderR.pinB = pinB;
    encoderR.count = 0;
	pinMode(pinA, INPUT_PULLDOWN);
	pinMode(pinB, INPUT_PULLDOWN);
	attachInterrupt(pinA, isrEncoderR, RISING);
    lastCountR = 0;
}

//********************************************************************
int countEncoderR()
{
	return encoderR.count;
}

//********************************************************************
void resetEncoderR()
{
	encoderR.count = 0;

}

//********************************************************************
// ComputeMove
//********************************************************************
void computeMove()
{
    long distanceL, distanceR;

    double rL, rR, rC; // curvature radius for left wheel, right wheel and center of robot
    double alpha;      // angle along circle
    double distance;   // distance run
    double q;          // quotient of left and right radius
    double temp;
    
    distanceL = (double)(encoderL.count - lastCountL) * MM_PER_TICK;
    encoderL.count = 0;
    distanceR = (double)(encoderR.count - lastCountR) * MM_PER_TICK;
    encoderR.count = 0;
    
    // rL, rR : radius of move circle for left and right wheels
    // q = rL/rR = distanceL/distanceR
    if (distanceL < 0)
    {
        // left wheel reverse
        if (distanceR < 0)
        {
            // left and right wheels reverse
            if (distanceL < distanceR)
            {
                // turn to right
                q = distanceL/distanceR; // q is >1
                rR = WHEEL_DISTANCE /(q-1.);
                rC = rR + WHEEL_DISTANCE/2.;
                temp = -distanceR/rR;
                alpha = PI+temp;
                distance = temp * rC;
            }
            else if (distanceL > distanceR)
            {
                // turn to left
                q = distanceR/distanceR;
                rL = WHEEL_DISTANCE /(q-1.);
                rC = rL + WHEEL_DISTANCE/2.;
                alpha = -distanceL/rL;
                distance = -alpha * rC;
            }
            else // distanceL == distanceR
            {
                // straight forward
                alpha = 0.;
                distance = distanceL;
            }
        }
        else if (distanceR > 0)
        {
            // left wheel reverse, right wheel forward
            if (-distanceL > distanceR)
            {
                // robot reverse, to the right
                q = -distanceL/distanceR;
                rL = WHEEL_DISTANCE/(1+q);
                rC = rL - WHEEL_DISTANCE/2.;
                temp = -distanceL/rL;
                alpha = PI+temp;
                distance = rC * temp;
            }
            else if (-distanceL < distanceR)
            {
                // robot forward, to the left
                q = -distanceR/distanceL;
                rR = WHEEL_DISTANCE/(1+q);
                rC = rR - WHEEL_DISTANCE/2.;
                alpha = distanceR/rR;
                distance = rC * alpha;
            }
            else
            {
                // robot turning on itself, counterclockwise
                rR = WHEEL_DISTANCE/2.;
                alpha = distanceR/rR;
                distance = 0.;
            }
        }
        else // distanceR = 0
        {
            // left wheel reverse, right wheel stopped
            alpha = PI-distanceL/WHEEL_DISTANCE;
            distance = -distanceL/2.;
        }
    }
    else if (distanceL > 0)
    {
        // left wheel forward
        if (distanceR < 0)
        {
            // left wheel forward, right wheel reverse
            if (distanceL > -distanceR)
            {
                // robot forward, to the right
                q = -distanceL/distanceR;
                rL = WHEEL_DISTANCE/(1+q);
                rC = rL - WHEEL_DISTANCE/2.;
                temp = distanceL/rL;
                alpha = -temp;
                distance = rC * temp;
            }
            else if (distanceL < -distanceR)
            {
                // robot reverse, to the left
                q = -distanceR/distanceL;
                rR = WHEEL_DISTANCE/(1+q);
                rC = rR - WHEEL_DISTANCE/2.;
                temp = -distanceR/rR;
                alpha = PI - temp;
                distance = rC * temp;
            }
            else // distanceL = -distanceR
            {
                // robot turning on itself, clockwise
                rR = WHEEL_DISTANCE/2.;
                alpha = -distanceR/rR;
                distance = 0.;
            }
        }
        else if (distanceR > 0)
        {
            // left and right wheel forward
            if (distanceL > distanceR)
            {
                // turn to right
                q = distanceL/distanceR;
                rR = WHEEL_DISTANCE /(q-1.);
                rC = rR + WHEEL_DISTANCE/2.;
                alpha = -distanceR/rR;
                distance = -alpha * rC;
            }
            else if (distanceL < distanceR)
            {
                // turn to left
                q = distanceR/distanceR;
                rL = WHEEL_DISTANCE /(q-1.);
                rC = rL + WHEEL_DISTANCE/2.;
                alpha = -distanceR/rL;
                distance = -alpha * rC;
            }
            else // distanceL == distanceR
            {
                // straight forward
                alpha = 0.;
                distance = distanceL;
            }
        }
        else // distanceR = 0
        {
            // left wheel forward, right wheel stopped
            alpha = -distanceL/WHEEL_DISTANCE;
            distance = distanceL/2.;
        }
    }
    else // distanceL = 0
    {
        // left wheel stopped
        if (distanceR < 0)
        {
            // left wheel stopped, right wheel revere
            alpha = PI - distanceR/WHEEL_DISTANCE;
            distance = -distanceR/2.;
        }
        else if (distanceR > 0)
        {
            // left wheel stopped, right wheel forward
            alpha = - distanceR/WHEEL_DISTANCE;
            distance = -distanceR/2.;
        }
        else // distanceR = 0
        {
            // left and right wheels stopped, no move
            alpha = 0.;
            distance = 0.;
        }
    }
    
    mov_h = alpha;
    heading += mov_h;

    mov_x = distance * sin(heading);
    mov_y = distance * cos(heading);
    
    pos_x += mov_x;
    pos_y += mov_y;
}

