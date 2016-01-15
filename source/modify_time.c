#include "led_display.h"
#include "audioMP3.h"
#include "core_cm4.h"
#include "stm32f4xx_conf.h"
#include "mp3dec.h"
#include "Audio.h"
#include <string.h>
#include "main.h"
#include "clock_control.h"
#include "modify_time.h"

void initializeModifyTime(){
	modifyCurrently = 0;
	hourToSet = 8;
	minToSet = 0;
}

void startModifyingTime(modify type){
	modifyCurrently = type;
	changeState(SetupClocks);

	if(type == Time){
		RTC_GetTime(RTC_Format_BIN, &myclockTimeStruct);
		hourToSet = myclockTimeStruct.RTC_Hours;
		minToSet = myclockTimeStruct.RTC_Minutes;
	}
	if(type == Alarm){
		hourToSet = getAlarmHours();
		minToSet = getAlarmMinutes();
	}
}

void finishModifyingTime(void){
	if(modifyCurrently == Alarm){
		writeNewAlarm();
	}
	else if(modifyCurrently == Time){
		writeNewTime();
	}
	modifyCurrently = nothing;
	changeState(RunNormal);
}

void writeNewTime(void){
	//set the time displayed on power up to 12PM
	myclockTimeStruct.RTC_H12 = RTC_H12_PM;
	myclockTimeStruct.RTC_Hours = hourToSet;
	myclockTimeStruct.RTC_Minutes = minToSet;
	myclockTimeStruct.RTC_Seconds = 0x05;
	RTC_SetTime(RTC_Format_BIN, &myclockTimeStruct);
}

void writeNewAlarm(void){
	//sets alarmA for 12:00AM, date doesn't matter
	setAlarmHours(hourToSet);
	setAlarmMinutes(minToSet);
}

int updateModifyTime(int digit){
	if(digit == 0){
		return minToSet % 10;
	}
	else if(digit == 1){
		return minToSet / 10;
	}
	else if(digit == 2){
		return hourToSet % 10;
	}
	else if(digit == 3){
		return hourToSet / 10;
	}else if(digit == -1){
		return -1;
	}
}

void incrementModifyMinute(int direction){
	minToSet = minToSet + direction;
	checkModifyTimeBounds();
}

void incrementModifyHour(int direction){
	hourToSet = hourToSet + direction;
	checkModifyTimeBounds();
}

void checkModifyTimeBounds(void){
	if(minToSet > 59){
		minToSet = 0;
		hourToSet++;
	}
	if(minToSet < 0){
		minToSet = 59;
		hourToSet--;
	}
	if(hourToSet > 23){
		hourToSet = 0;
	}
	if(hourToSet < 0){
		hourToSet = 23;
	}
}
