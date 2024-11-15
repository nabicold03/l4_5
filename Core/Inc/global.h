/*
 * global.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Admin
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

// param definition
// main fsm
#define 	INIT					1
#define		TIME_DISPLAY			2
#define		TIME_ADJUST				3
#define		ALARM					4
#define		UART					21

// time adjust fsm
#define		TA_INIT					5
#define		TA_DAY					6		// Mon, Tue, Wed, Thu, Fri, Sat, Sun
#define		TA_DATE					7
#define		TA_MONTH				8
#define		TA_YEAR					9
#define		TA_HOUR					10
#define		TA_MINUTE				11
#define		TA_SECOND				12

// alarm fsm
#define		AL_INIT					13
#define		AL_DAY					14		// Mon, Tue, Wed, Thu, Fri, Sat, Sun
#define		AL_DATE					15
#define		AL_MONTH				16
#define		AL_YEAR					17
#define		AL_HOUR					18
#define		AL_MINUTE				19
#define		AL_SECOND				20

#include <stdio.h>
#include <main.h>
#include "main.h"

//#include "software_timer.h"
//#include "button.h"
//#include "led_7seg.h"
//#include "spi.h"
//#include "i2c.h"
//#include "lcd.h"
//#include "lcdfont.h"
//#include "ds3231.h"

// status of fsm: idx 0 (main fsm), idx 1 (adjust time fsm), idx 2 (alarm fsm)
extern int status[3];	// idx 0 main fsm
						// idx 1 time adjust fsm
						// idx 2 alarm fsm



// time variables: idx 0 (time for display), idx 1 (time for alarm)
extern int day[2];
extern int date[2];
extern int month[2];
extern int year[2];
extern int hour[2];
extern int minute[2];
extern int second[2];
extern uint8_t error_uart;


// time variables for temp use when modify time or alarm
extern int day_temp;
extern int date_temp;
extern int month_temp;
extern int year_temp;
extern int hour_temp;
extern int minute_temp;
extern int second_temp;
extern int alarm_on;
extern int count_at;
extern int toggle_at;
extern int count_al;
extern int toggle_al;
extern int flag_alarm;

void system_init();
void displayTime();
void updateTime();
void main_fsm();
void al_fsm();
void ta_fsm();
#endif /* INC_GLOBAL_H_ */
