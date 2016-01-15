#include "stm32f4xx_rtc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "audioMP3.h"
#include "main.h"
#include "led_display.h"
#include "clock_control.h"
#include "modify_time.h"
#include "alarm.h"
#include "music.h"

void initializeAlarm(){
	SnoozeMode = false;
	CyclesLeftInSnoozeMode = 0;
	AlarmState = DISABLE;

	MyAlarmStructHours = 8;
	MyAlarmStructMinutes = 20;
	MyAlarmStructSeconds = 1;
}


void changeAlarmState(FunctionalState newState){
	AlarmState = newState;
}

void toggleAlarm(){
	if(AlarmState == ENABLE){
		AlarmState = DISABLE;
	}else{
		AlarmState = ENABLE;
	}
}

void turnOnAlarm(){
	turnOnMusic();
}

void turnOffAlarm(){
	turnOffMusic();
}

int checkAlarmTrigger(){
	//If it is in snooze let it deal with turning the alarm on and off
	if(SnoozeMode == true){
		dealWithSnoozeMode();
	}
	//Else we check to see if the alarm has gone off
	else{
		RTC_GetTime(RTC_Format_BIN, &myclockTimeStruct);
		if(AlarmState == ENABLE)
			if(myclockTimeStruct.RTC_Hours == MyAlarmStructHours)
				if (myclockTimeStruct.RTC_Minutes == MyAlarmStructMinutes)
					if(myclockTimeStruct.RTC_Seconds == MyAlarmStructSeconds)
						return true;
		return false;
	}
}

void enterSnoozeMode(int seconds){
	turnOffMusic();
	CyclesLeftInSnoozeMode = 500 * seconds;
	SnoozeMode = true;
}

void dealWithSnoozeMode(){
	if(CyclesLeftInSnoozeMode > 0){
		CyclesLeftInSnoozeMode--;
	}else{
		turnOnMusic();
		SnoozeMode = false;
	}
}

void alarmLight(){
	if(AlarmState == ENABLE){
		 GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_SET);

	 }else{
		 GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_RESET);
	 }
}

int getAlarmMinutes(){
	return MyAlarmStructMinutes;
}
int getAlarmHours(){
	return MyAlarmStructHours;
}
void setAlarmMinutes(int mins){
	MyAlarmStructMinutes = mins;
}
void setAlarmHours(int hours){
	MyAlarmStructHours = hours;
}
