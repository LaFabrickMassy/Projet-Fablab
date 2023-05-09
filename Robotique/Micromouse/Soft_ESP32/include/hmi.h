#ifndef _HMI_H_
#define _HMI_H_

#define HMI_MODE_INDEX 0
#define HMI_MODE_TD 1 // mode test drive
#define HMI_MODE_PC 2 // mode parameter calibration
#define HMI_MODE_EX 3 // mode exploration

extern int hmi_mode;

void hmiInit();
void hmiStep();
void hmiHandleWSMessage(String message);

#endif