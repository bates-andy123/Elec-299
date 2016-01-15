#ifndef CLOCK_CONTROL_H
#define CLOCK_CONTROL_H

//flags
volatile int TwelveHourMode;

//Function Prototypes
void InitialClock();


int GetRTCDigit(int digit);

#endif
