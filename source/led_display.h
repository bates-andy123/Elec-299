#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H


//data
volatile int currentSegment;

void initializeClock(void);
int turnOnNextSegment(void);
void UpdateTimeByOneMin(void);
void turnOnAnodes(volatile int num);

#endif
