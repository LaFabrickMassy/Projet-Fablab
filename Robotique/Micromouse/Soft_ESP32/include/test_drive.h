#ifndef _TEST_DRIVE_H
#define _TEST_DRIVE_H


void testDriveInit();
void testDriveStop();
void testDriveStep();
void testDriveRunInit();
void testDriveRunStep();
void testDriveRotateInit();
void testDriveRotateInitCW();
void testDriveRotateInitCCW();
void testDriveRotateStep();
void testDriveHandleMessage(String message);

// robot state
extern double x,y,h,speed;

#endif