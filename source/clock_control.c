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

void InitializeClock(){
	TwelveHourMode = 0;
	//TwelveHourMode = false;
}

/**
  * @brief  Return the current digit number
  * @param  digit can be any number between 0-3
  * 		If bigger function returns 0
  * @retval Returns the selected digit
  */
int GetRTCDigit(int digit){
	RTC_GetTime(RTC_Format_BCD, &myclockTimeStruct);
	if(digit == 0){
		return 0x0F & myclockTimeStruct.RTC_Minutes;
	}
	if(digit == 1){
		return (0xF0 & myclockTimeStruct.RTC_Minutes)>>4;
	}
	if(TwelveHourMode == false){
		if(digit == 2){
			return 0x0F & myclockTimeStruct.RTC_Hours;
		}
		if(digit == 3){
			return (0xF0 & myclockTimeStruct.RTC_Hours)>>4;
		}
	}
	if(TwelveHourMode == true){
		return convertToTwelveHourMode(digit);
	}
	return 0;
}

int convertToTwelveHourMode(digit){
	RTC_GetTime(RTC_Format_BIN, &myclockTimeStruct);
	int hour = myclockTimeStruct.RTC_Hours % 12;
	if(hour == 0){
		if(digit == 2){
			return 2;
		}
		if(digit == 3){
			return 1;
		}
	}
	if(digit == 2){
		return hour % 10;
	}
	if(digit == 3){
		return hour / 10;
	}
}

int displayPM(void){
	RTC_GetTime(RTC_Format_BIN, &myclockTimeStruct);
	if (TwelveHourMode == 0){
		return 0;
	}
	return myclockTimeStruct.RTC_Hours / 12;
}

void switchTwelveHourMode(void){
	if(TwelveHourMode == false)
		TwelveHourMode = true;
	else {
		TwelveHourMode = false;
	}
}
