#include "led_display.h"
#include "audioMP3.h"
#include "core_cm4.h"
#include "stm32f4xx_conf.h"
#include "mp3dec.h"
#include "Audio.h"
#include <string.h>
#include "main.h"

void initializeClock(void){
	//Initialise what segment of the clock we are on
	currentSegment = 0;


	//Turn off all the LED digits
    GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
    GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET);
}

int turnOnNextSegment(void){

	//Responsible for incrementing the display
	if(currentSegment > 4)
		currentSegment = 0;
	else {
		currentSegment++;
	}

	//Turn off all the connections
	int gndPins = GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_WriteBit(GPIOD, gndPins, Bit_RESET);

	//Connecting the cathode to the ground
	if(currentSegment == 0){
		GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET);
		return 0;
	}
	else if(currentSegment == 1){
		GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
		return 1;
	}
	//The Colon
	else if(currentSegment == 2){
		//GPIO_WriteBit(GPIOE, GPIO_Pin_12, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);
		return -1;
	}
	else if(currentSegment == 3){
		GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_SET);
		return 2;
	}
	else if(currentSegment == 4){
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
		return 3;
	}
}

void turnOnSelectedSegment(int segment){

	//Responsible for incrementing the display

	//Turn off all the connections
	int gndPins = GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_WriteBit(GPIOD, gndPins, Bit_RESET);

	//Connecting the cathode to the ground
	if(segment == 0){
		GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET);
	}
	else if(segment == 1){
		GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
	}
	//The Colon
	else if(segment == 2){
		//GPIO_WriteBit(GPIOE, GPIO_Pin_12, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);
	}
	else if(segment == 3){
		GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_SET);
	}
	else if(segment == 4){
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
	}
}

void turnOnAnodes(int num){
	unsigned int leds = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_WriteBit(GPIOE, leds, Bit_SET);

	if(num == 0){
		leds = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	}
	else if(num == 1){
		leds = GPIO_Pin_12 | GPIO_Pin_15;
	}
	else if(num == 2){
		leds = GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_10 | GPIO_Pin_9;
	}
	else if(num == 3){
		leds = GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_9;
	}
	else if(num == 4){
		leds =  GPIO_Pin_15 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_11;
	}
	else if(num == 5){
		leds = GPIO_Pin_13 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_9;
	}
	else if(num == 6){
		leds = GPIO_Pin_13 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_9 | GPIO_Pin_10;
	}
	else if(num == 7){
		leds = GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_15;
	}
	else if(num == 8){
		leds = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	}
	else if(num == 9){
		leds = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	}
	//Case for the colon in the middle
	else if(num == -1){
		leds = GPIO_Pin_13 | GPIO_Pin_15;
		if(displayPM()){
			leds = leds | GPIO_Pin_12;
		}
	}

	GPIO_WriteBit(GPIOE, leds, Bit_RESET);
}

void turnOffAnodes(){
	//Turn off all the connections
	int gndPins = GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_WriteBit(GPIOD, gndPins, Bit_RESET);
}
