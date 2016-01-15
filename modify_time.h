#ifndef MODIFY_TIME_H
#define MODIFY_TIME_H

RTC_TimeTypeDef tempAlarmStruct;

//Constants
volatile int hourToSet;
volatile int minToSet;
volatile int modifyCurrently;

typedef enum{
	nothing,
	Time,
	Alarm
} modify;

typedef enum{
	down = -1,
	up = 1
} direction;

//Function prototypes
void initializeModifyTime(void);

#endif
