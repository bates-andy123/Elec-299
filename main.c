//This program uses the RTC to display time on a 4 digit 7 segment display
//When the alarm triggers, it plays mp3 files through a USB connected on the
//micro USB port

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
//structures
RCC_ClocksTypeDef RCC_Clocks;
GPIO_InitTypeDef	GPIOInitStruct;
TIM_TimeBaseInitTypeDef TIM_InitStruct;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;


//function prototypes
void configuration(void);
;


//global variables
int interruptOccurred = 0;


extern volatile int exitMp3 = 0;
extern volatile int mp3PlayingFlag = 0;
extern volatile int snoozeMemory = 0;

/*
void TIM3_IRQHandler()
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

    }
}

void EnableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 1;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}*/

void InitializeLEDs(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_StructInit( &GPIOInitStruct );
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStruct.GPIO_OType = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIOInitStruct);

	GPIO_WriteBit(GPIOE, GPIO_Pin_11, Bit_RESET);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_StructInit( &GPIOInitStruct );
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStruct.GPIO_OType = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIOInitStruct);

	GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
}

void InitializeTimer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 40000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 1;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &timerInitStructure);
    TIM_Cmd(TIM3, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}

void configureExternalInterrupts_4(void){
	/* Set variables used */
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Enable clock for GPIOB */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable clock for SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Set pin as input */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* Tell system that you will use PB12 for EXTI_Line12 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);

	/* PB12 is connected to EXTI_Line12 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x03;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

void configureExternalInterrupts_15_10(void){
	/* Set variables used */
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* Enable clock for GPIOB */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* Enable clock for SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Set pin as input */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* Tell system that you will use PB12 for EXTI_Line12 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);

	/* PB12 is connected to EXTI_Line12 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x03;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

void configureExternalInterrupts_9_5(void){
    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for GPIOB */
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Tell system that you will use PC8 for EXTI_Line8 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);

    /* PC8 is connected to EXTI_Line12 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line5 | EXTI_Line8 | EXTI_Line9;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);

}

/*for testing
uint32_t *ptr;
uint32_t *ptr2;*/

void main()
{

  configuration();
  //InitializeLEDs();
  //InitializeTimer();
  //EnableTimerInterrupt();

  initializeClock();
  initializeAlarm();
  initializeModifyTime();

  configureExternalInterrupts_4();
  configureExternalInterrupts_9_5();
  configureExternalInterrupts_15_10();

  ticks = 1;

  pause_flag = 1;

  doNotTriggerButton = 0;

  //testPtr = 0;
  state = RunNormal;
  testNumber = 0;
  mp3PlayingFlag = 0;
  while(1)
  {

	 //if(mp3PlayingFlag == 1)
		 audioToMp3();

  }

}

void changeState(States newState){
	state = newState;
}

//The function responible for handling my finite state machine
void finiteStateMachine(int button){
	//May be a button debounce so exit the function
	if(doNotTriggerButton > 0)
		return;

	//The next bit contains the code monitoring the buttons
	//The buttons change their function depending on what mode the clock is
	//If the clock is running in normal mode
	if(state == RunNormal)
		switch(button){
			case 4  :
				switchTwelveHourMode();
			   break;
			case 5  :
				startModifyingTime(Alarm);
			   break;
			case 8  :
				//Stop the clock from operating
				startModifyingTime(Time);
				break;
			case 9  :
				toggleAlarm();
				break;
			case 11  :
				toggleMusic();
				break;
		}
	else if(state == SetupClocks)
		switch(button){
			case 4  :
				incrementModifyMinute(down);
				break;
			case 5  :
				incrementModifyMinute(up);
			   break;
			case 8  :
				incrementModifyHour(down);
				break;
			case 9  :
				incrementModifyHour(up);
				break;
			case 11  :
				finishModifyingTime();
				break;
		}
	else if(state == AlarmOn)
			switch(button){
				case 4  :
					switchTwelveHourMode();
					break;
				case 5  :
					enterSnoozeMode(10);
				   break;
				case 8  :
					enterSnoozeMode(15);
					break;
				case 9  :
					changeState(RunNormal);
					turnOffAlarm();
					break;
				case 11  :
					changeState(RunNormal);
					turnOffAlarm();
					break;
			}

	doNotTriggerButton = 50; //Set the debounce time
}

void EXTI4_IRQHandler(void){
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		/* Do your stuff when PB12 is changed */

		finiteStateMachine(4);
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

void EXTI15_10_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		/* Do your stuff when PB12 is changed */

		finiteStateMachine(11);
		/* Clear interrupt flag */
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}
/* Handle PB12 interrupt */
void EXTI9_5_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        /* Do your stuff when PC9 is changed */
    	finiteStateMachine(8);
        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
	   /* Do your stuff when PC9 is changed */

    	finiteStateMachine(9);
	   /* Clear interrupt flag */
	   EXTI_ClearITPendingBit(EXTI_Line9);
   }
   if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
	   /* Do your stuff when PC9 is changed */

      finiteStateMachine(5);
   	  /* Clear interrupt flag */
   	  EXTI_ClearITPendingBit(EXTI_Line5);
   }
}

//timer interrupt handler that is called at a rate of 500Hz
//this function gets the time and displays it on the 7 segment display
//it also checks for button presses, debounces, and handles each case
void TIM5_IRQHandler(void)
{
	//double checks that interrupt has occurred
	if( TIM_GetITStatus( TIM5, TIM_IT_Update ) != RESET )
	{
		//Lets track a second to slow stuff down
		ticks++;
		if(ticks > 800)
			ticks = 1;

		//Decrement the debounce count
		if(doNotTriggerButton > 0){
			doNotTriggerButton--;
		}

		if(checkAlarmTrigger() == true){
			turnOnAlarm();
			changeState(AlarmOn);
		}

		alarmLight();

		int digit = 0;

		//The current digit
		if(state != SetupClocks)
			digit = turnOnNextSegment();

		if(digit == -1){
			turnOnAnodes(-1);
		}
		//If it is the colon just turn that on
		else if(state == RunNormal){
			//Check to see if its the colon
			turnOnAnodes(GetRTCDigit(digit));
		}
		else if(state == SetupClocks){
			int Rem = ticks % 400;
			if(Rem > 200){
				Rem = Rem % 10;
				if(Rem < 2){
					digit = turnOnSelectedSegment(0);
					turnOnAnodes(updateModifyTime(0));
				}
				else if(Rem < 4){
					digit = turnOnSelectedSegment(1);
					turnOnAnodes(updateModifyTime(1));
				}
				else if(Rem < 6){
					digit = turnOnSelectedSegment(2);
					turnOnAnodes(updateModifyTime(-1));
				}
				else if(Rem < 8){
					digit = turnOnSelectedSegment(3);
					turnOnAnodes(updateModifyTime(2));
				}
				else if(Rem < 10){
					digit = turnOnSelectedSegment(4);
					turnOnAnodes(updateModifyTime(3));
				}
			}else{
				turnOffAnodes();
			}
		}
		else if(state == AlarmOn){
			//digit = turnOnNextSegment();
			turnOnAnodes(GetRTCDigit(digit));
		}
	}
	TIM5->SR = (uint16_t)~TIM_IT_Update;
}

//alarm A interrupt handler
//when alarm occurs, clear all the interrupt bits and flags
//then set the flag to play mp3
void RTC_Alarm_IRQHandler(void)
{

	//resets alarm flags and sets flag to play mp3
	  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	  {
    	RTC_ClearFlag(RTC_FLAG_ALRAF);
	    RTC_ClearITPendingBit(RTC_IT_ALRA);
	    EXTI_ClearITPendingBit(EXTI_Line17);
		interruptOccurred = 1;

		mp3PlayingFlag = 0;
		GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_RESET);

	  }


}

//configures the clocks, gpio, alarm, interrupts etc.
void configuration(void)
{
	  //lets the system clocks be viewed
	  RCC_GetClocksFreq(&RCC_Clocks);

	  //enable peripheral clocks
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	  //enable the RTC
	  PWR_BackupAccessCmd(DISABLE);
	  PWR_BackupAccessCmd(ENABLE);
	  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	  RCC_RTCCLKCmd(ENABLE);
	  RTC_AlarmCmd(RTC_Alarm_A,ENABLE);

	  //Enable the LSI OSC
	  RCC_LSICmd(ENABLE);

	  //Wait till LSI is ready
	  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	  //enable the external interrupt for the RTC to use the Alarm
	  /* EXTI configuration */
	  EXTI_ClearITPendingBit(EXTI_Line17);
	  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  //set timer 5 to interrupt at a rate of 500Hz
	  TIM_TimeBaseStructInit(&TIM_InitStruct);
	  TIM_InitStruct.TIM_Period	=  8000;	// 500Hz
	  TIM_InitStruct.TIM_Prescaler = 20;
	  TIM_TimeBaseInit(TIM5, &TIM_InitStruct);

	  // Enable the TIM5 global Interrupt
	  NVIC_Init( &NVIC_InitStructure );
	  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init( &NVIC_InitStructure );

      //setup the RTC for 12 hour format
	  myclockInitTypeStruct.RTC_HourFormat = RTC_HourFormat_24;
	  myclockInitTypeStruct.RTC_AsynchPrediv = 127;
	  myclockInitTypeStruct.RTC_SynchPrediv = 0x00FF;
	  RTC_Init(&myclockInitTypeStruct);

	  //set the time displayed on power up to 12PM
	  myclockTimeStruct.RTC_H12 = RTC_H12_PM;
	  myclockTimeStruct.RTC_Hours = 0x08;
	  myclockTimeStruct.RTC_Minutes = 0x00;
	  myclockTimeStruct.RTC_Seconds = 0x05;
	  RTC_SetTime(RTC_Format_BCD, &myclockTimeStruct);


	  //sets alarmA for 12:00AM, date doesn't matter
	  AlarmStruct.RTC_AlarmTime.RTC_H12 = RTC_H12_PM;
	  AlarmStruct.RTC_AlarmTime.RTC_Hours = 0x08;
	  AlarmStruct.RTC_AlarmTime.RTC_Minutes = 0x00;
	  AlarmStruct.RTC_AlarmTime.RTC_Seconds = 0x15;
	  AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	  RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&AlarmStruct);

	  // Enable the Alarm global Interrupt
	  NVIC_Init( &NVIC_InitStructure );
	  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init( &NVIC_InitStructure );

	  //IO for push buttons using internal pull-up resistors
	  GPIO_StructInit( &GPIOInitStruct );
	  GPIOInitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	  GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIOInitStruct.GPIO_Mode = GPIO_Mode_IN;
	  GPIOInitStruct.GPIO_OType = GPIO_OType_PP;
	  GPIOInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_Init(GPIOC, &GPIOInitStruct);

	  //configure GPIO for digits
	  GPIO_StructInit( &GPIOInitStruct );
	  GPIOInitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	  GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIOInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	  GPIOInitStruct.GPIO_OType = GPIO_OType_PP;
	  GPIOInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOE, &GPIOInitStruct);

	  //configure GPIO for multiplexing
	  GPIO_StructInit( &GPIOInitStruct );
	  GPIOInitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	  GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIOInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	  GPIOInitStruct.GPIO_OType = GPIO_OType_PP;
	  GPIOInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOD, &GPIOInitStruct);

	  //enables RTC alarm A interrupt
	  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	  //enables timer interrupt
	  TIM5->DIER |= TIM_IT_Update;

	  //enables timer
	  TIM5->CR1 |= TIM_CR1_CEN;
}
