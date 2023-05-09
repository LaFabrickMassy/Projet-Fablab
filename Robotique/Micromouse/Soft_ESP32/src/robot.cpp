//*****************************************************************************
//*****************************************************************************
//
// robot state
//
//*****************************************************************************
//*****************************************************************************

#include <Arduino.h>
#include "errors.h"
#include "motor.h"
#include "distance_sensor.h"
#include "wheel_encoder.h"
#include "robot_controller.h"
#include "robot.h"

//*****************************************************************************
//
// robot state initialisation
//
//*****************************************************************************
Robot::Robot() {
    ;
}

//*****************************************************************************
//
//
//
//*****************************************************************************
int Robot::Init() {
    int err;

    // Initialise controller and state
    err = controller.Init();
    if (err)
        return err;
    
    // Reset internal state and variables
    Reset();

    return ERROR_NONE;
}

//********************************************************************
// 
//********************************************************************
void Robot::Reset() {
    controller.Stop();
    controller.Reset();

    // dynamic state
    dynamic_state = ROBOT_DSTATE_STOP;
}

//********************************************************************
// 
//********************************************************************
void Robot::Stop() {
    controller.Stop();
    dynamic_state = ROBOT_DSTATE_STOP;
}

//********************************************************************
// 
//********************************************************************
void Robot::Crash() {
    controller.Stop();
    dynamic_state = ROBOT_DSTATE_CRASH;
}

//********************************************************************
// 
//********************************************************************
String Robot::JSON_State() {
    String jsonString;
    
    jsonString = "{";
    /*
    jsonString += "\"dmode\":\""+String(dynamic_state)+"\"";
    jsonString += ",";
    jsonString += "\"speed\":\""+String(state.speed*100)+"\"";
    jsonString += ",";
    jsonString += "\"turn\":\""+String(state.turn*100)+"\"";
    jsonString += ",";
    jsonString += "\"x\":\""+String(state.x)+"\"";
    jsonString += ",";
    jsonString += "\"y\":\""+String(state.y)+"\"";
    jsonString += ",";
    jsonString += "\"h\":\""+String(180*state.h/PI)+"\"";
    */

    jsonString += "}";
    return jsonString;
}

//********************************************************************
// 
//********************************************************************
String Robot::String_State() {
    String s;
    
    s = "dstate="+String(dynamic_state);
    /*
    s+= " speed="+String(state.speed);
    s+= " x="+String(state.x);
    s+= " y="+String(state.y);
    s+= " h="+String(state.h*180./PI);
    */
    return s;
}


















//********************************************************************
// ComputeMove
//********************************************************************
void Robot::ComputeMove() {
    double dL, dR;

    double rL, rR, r; // curvature radius for left wheel, right wheel and center of robot
    double alpha;      // angle along circle
    double distance;   // distance run
    double q;          // quotient of left and right radius
    double temp;
    double ca, sa;     // cos(alpha), sin(alpha)
    double ch, sh;     // cos(h), sin(h)
    
    double mov_x, mov_y;
    #define W ROBOT_WHEEL_DISTANCE
    

    if (encoderL.flag_reverse)
        dL = (double)(encoderL.count - encoderL_lastcount) * ENCL_RESOL;
    else
        dL = -(double)(encoderL.count - encoderL_lastcount) * ENCL_RESOL;
    encoderL_lastcount = encoderL.count;
    if (encoderR.flag_reverse)
        dR = (double)(encoderR.count - encoderR_lastcount) * ENCR_RESOL;
    else
        dR = -(double)(encoderR.count - encoderR_lastcount) * ENCR_RESOL;
    encoderR_lastcount = encoderR.count;
    
    
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
                ch = cos(h);
                sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);

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
                ch = cos(h);
                sh = sin(h);
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
            ch = cos(h);
            sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);
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
            ch = cos(h);
            sh = sin(h);
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
            
            ch = cos(h);
            sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);
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
            ch = cos(h);
            sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);
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
                ch = cos(h);
                sh = sin(h);
                
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
                ch = cos(h);
                sh = sin(h);
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
    
    h += alpha;

    x += mov_x;
    y += mov_y;

	#ifdef DEBUG_MOVE
    Serial.println("########## Compute move");
    Serial.print(" h=");
    Serial.print(heading*180/PI);
    Serial.print(" Pos : x=");
    Serial.print(x);
    Serial.print(" y=");
    Serial.print(y);
    Serial.println("");
	#endif
}

#undef W
