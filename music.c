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

void toggleMusic(void){
	if(pause_flag == 1){
		pause_flag = 0;
	}else{
		pause_flag = 1;
	}
}

void turnOnMusic(){
	pause_flag = 0;
}

void turnOffMusic(){
	pause_flag = 1;
}
