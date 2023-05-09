#ifndef _LOGUTILS_H
#define _LOGUTILS_H

#define LOGFNAME "/micromouse.log"

int logOpen();
void logWrite(String s);
void logWriteln(String s);
void dumpToSerial();

#endif
