/*
 * global.c
 *
 *  Created on: Nov 12, 2024
 *      Author: Admin
 */
#include "global.h"

int status[3];	// idx 0 main fsm
						// idx 1 time adjust fsm
						// idx 2 alarm fsm



// time variables: idx 0 (time for display), idx 1 (time for alarm)
int day[2];
int date[2];
int month[2];
int year[2];
int hour[2];
int minute[2];
int second[2];


// time variables for temp use when modify time or alarm: idx 0 (time for adjust), idx 1 (time for alarm)
int day_temp;
int date_temp;
int month_temp;
int year_temp;
int hour_temp;
int minute_temp;
int second_temp;
int alarm_on;
int count_at;
int toggle_at;
int count_al;
int toggle_al;
int flag_alarm;
uint8_t error_uart;
