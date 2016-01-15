#ifndef ALARM_H
#define ALARM_H

//Funtion Protoypes
MyAlarmStructHours;
MyAlarmStructMinutes;
MyAlarmStructSeconds;

volatile int SnoozeMode;
volatile int CyclesLeftInSnoozeMode;
volatile int AlarmState;

void toggleMusic(void);

#endif
