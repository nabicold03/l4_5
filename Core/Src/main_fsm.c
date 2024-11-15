/*
 * main_fsm.c
 *
 *  Created on: Nov 12, 2024
 *      Author: Admin
 */


#include "main_fsm.h"
//#include "ta_fsm.h"
//#include "al_fsm.h"
#include "global.h"
#include <stdbool.h>

// check if the real time reaches the setup alarm
bool is_alarm(){
	ds3231_ReadTime();
	if(ds3231_sec==second[1] && ds3231_min==minute[1] && ds3231_hours==hour[1] && ds3231_day==day[1] && ds3231_date==date[1] && ds3231_month==month[1] && ds3231_year==year[1]) return true;
	return false;
}

// display time (the displayTime function declared in main.c file is used to display ds3231 variables updated continuously
void display_ta_al(int hour, int min, int sec, int day, int date, int month, int year){
	lcd_ShowIntNum(70, 100, hour, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, year, 2, YELLOW, BLACK, 24);
}

// clear the string showing the current status of the system
void clear_header(){
	lcd_ShowStr(20, 50, "ABCDEFGHIJKLMNO1ADSD2", BLACK, BLACK, 24, 0);
}

// update time to ds3231 variables, the updateTime used in the main.c file is used only 1 time at the beginning, used to set initial value for ds3231 variables
void updateTime_ta_al(int hour, int min, int sec, int day, int date, int month, int year){
	ds3231_Write(ADDRESS_YEAR, year);
	ds3231_Write(ADDRESS_MONTH, month);
	ds3231_Write(ADDRESS_DATE, date);
	ds3231_Write(ADDRESS_DAY, day);
	ds3231_Write(ADDRESS_HOUR, hour);
	ds3231_Write(ADDRESS_MIN, min);
	ds3231_Write(ADDRESS_SEC, sec);
}

bool leap_year(int year){
	if(year%100 == 0){
		if(year%400 == 0) return true;
	} else {
		if(year%4 == 0) return true;
	}
	return false;
}

//int toggle_at;
void main_fsm(){
	if(is_alarm()) flag_alarm = 1;

	switch (status[0]){
	case INIT:
		status[0] = TIME_DISPLAY;
		alarm_on = 0;
		toggle_at = 0;
		break;

	case TIME_DISPLAY:
		// display mode TIME DISPLAY to lcd screen
		lcd_ShowStr(20, 50, "TIME DISPLAY", RED, BLUE, 24, 1);

		if(button_count[11] == 1){			// if change to next state (button ">")
			toggle_at = 0;

			// change state
			status[0] = TIME_ADJUST;

			// before changing state, store valur of the current time to temp variables used for 2 next states
			hour_temp = ds3231_hours;
			minute_temp = ds3231_min;
			second_temp = ds3231_sec;
			day_temp = ds3231_day;
			date_temp = ds3231_date;
			month_temp = ds3231_month;
			year_temp = ds3231_year;
			display_ta_al(hour_temp, minute_temp, second_temp, day_temp, date_temp, month_temp, year_temp);

			// clear string status TIME DISPLAY
			clear_header();
		} else {							// if stay -> display time
			status[0] = TIME_DISPLAY;
			displayTime();
		}
		break;

	case TIME_ADJUST:
		if(button_count[11] == 1){				// if change to alarm state -> not update time, reset state of adjust time to TA_INIT
			status[0] = ALARM;
			status[1] = TA_INIT;
			display_ta_al(hour_temp, minute_temp, second_temp, day_temp, date_temp, month_temp, year_temp);
			clear_header();
		} else {
			switch(status[1]){
				case TA_INIT:
					if(status[0] == TIME_ADJUST){	// if status of main fsm change to TIME_ADJUST -> change to TA_DAY state
						status[1] = TA_DAY;
					} else {
						status[1] = TA_INIT;
					}
					break;

				case TA_DAY:
					// Show current state "TIME ADJUST - DAY"
					lcd_ShowStr(20, 50, "TIME ADJUST - DAY", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						// change to date adjust
						status[1] = TA_DATE;

						// show the adjusted day to lcd screen - added after running and bug log
						lcd_ShowIntNum(20, 130, day_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_DAY;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							day_temp = day_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								day_temp = day_temp + 1;
							}
						}

						// day ranges from 1 to 7 (monday to sunday) -> if exceed 7, go back to 1
						if(day_temp >= 8) day_temp = 1;
						else if(day_temp <= 0) day_temp = 1;

						// toggle day value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(20, 130, day_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(20, 130, day_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case TA_DATE:
					// Show current state "TIME ADJUST - DATE"
					lcd_ShowStr(20, 50, "TIME ADJUST - DATE", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						// change to month adjust
						status[1] = TA_MONTH;

						// show the adjusted date to lcd screen - added after running and bug log
						lcd_ShowIntNum(70, 130, date_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_DATE;
						if(button_count[3] == 1){			// if press ^ -> increase value of date_temp
							date_temp = date_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								date_temp = date_temp + 1;
							}
						}

						// go back to 1st date if the date exceeds corresponding value of each month
						// if months have 31 days
						if(month_temp==1 || month_temp==3 || month_temp==5 || month_temp==7 || month_temp==8 || month_temp==10 || month_temp==12){
							if(date_temp>=32){
								date_temp = 1;
							}
						} else if(month_temp == 2){		// if month 2
							if(leap_year(year_temp)){	// check leap year
								if(date_temp >= 30) date_temp = 1;	// if leap year -> 29 days maximum
							} else {								// else 28 days maximum
								if(date_temp >= 29) date_temp = 1;
							}
						} else {						// if months have 30 days
							if(date_temp >= 31) date_temp = 1;
						}

						// toggle date value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(70, 130, date_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(70, 130, date_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case TA_MONTH:
					// Show current state "TIME ADJUST - MONTH"
					lcd_ShowStr(20, 50, "TIME ADJUST - MON", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						// change to year adjust
						status[1] = TA_YEAR;

						// show the adjusted month to lcd screen - added after running and bug log
						lcd_ShowIntNum(110, 130, month_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_MONTH;
						if(button_count[3] == 1){			// if press ^ -> increase value of month_temp
							month_temp = month_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								month_temp = month_temp + 1;
							}
						}

						// if month exceeds 12 -> reset to 1
						if(month_temp >= 13) month_temp = 1;

						// toggle month value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(110, 130, month_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(110, 130, month_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case TA_YEAR:
					// Show current state "TIME ADJUST - YEAR"
					lcd_ShowStr(20, 50, "TIME ADJUST - YEAR", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						status[1] = TA_HOUR;

						// show the adjusted year to lcd screen - added after running and bug log
						lcd_ShowIntNum(150, 130, year_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_YEAR;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							year_temp = year_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								year_temp = year_temp + 1;
							}
						}

						// we do not have upper limit for year

						// toggle year value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(150, 130, year_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(150, 130, year_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case TA_HOUR:
					// Show current state "TIME ADJUST - HOUR"
					lcd_ShowStr(20, 50, "TIME ADJUST - HOUR", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						status[1] = TA_MINUTE;

						// show the adjusted hour to lcd screen - added after running and bug log
						lcd_ShowIntNum(70, 100, hour_temp, 2, GREEN, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_HOUR;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							hour_temp = hour_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								hour_temp = hour_temp + 1;
							}
						}

						// if hour exceeds 23 -> reset to 0
						if(hour_temp >= 24) hour_temp = 0;

						// toggle hour value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(70, 100, hour_temp, 2, GREEN, BLACK, 24);
						} else {
							lcd_ShowIntNum(70, 100, hour_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case TA_MINUTE:
					// Show current state "TIME ADJUST - MINUTE"
					lcd_ShowStr(20, 50, "TIME ADJUST - MIN", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[1] = TA_SECOND;

						// show the adjusted minute to lcd screen - added after running and bug log
						lcd_ShowIntNum(110, 100, minute_temp, 2, GREEN, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_MINUTE;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							minute_temp = minute_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								minute_temp = minute_temp + 1;
							}
						}

						// if minute exceeds 59 -> reset to 0
						if(minute_temp >= 60) minute_temp = 0;

						// toggle minute value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(110, 100, minute_temp, 2, GREEN, BLACK, 24);
						} else {
							lcd_ShowIntNum(110, 100, minute_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case TA_SECOND:
					// Show current state "TIME ADJUST - SECOND"
					lcd_ShowStr(20, 50, "TIME ADJUST - SEC", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						// After adjust second -> go back to TA_INIT (ta_fsm) and TIME_DISPLAY (main_fsm)
						status[1] = TA_INIT;
						status[0] = TIME_DISPLAY;

						// update adjusted time components above to ds3231 variables
						updateTime_ta_al(hour_temp, minute_temp, second_temp, day_temp, date_temp, month_temp, year_temp);

						// show the adjusted second to lcd screen - added after running and bug log
						lcd_ShowIntNum(150, 100, second_temp, 2, GREEN, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[1] = TA_SECOND;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							second_temp = second_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								second_temp = second_temp + 1;
							}
						}

						// if second exceeds 59 -> reset to 1
						if(second_temp >= 60) second_temp = 1;

						// toggle second value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(150, 100, second_temp, 2, GREEN, BLACK, 24);
						} else {
							lcd_ShowIntNum(150, 100, second_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				default:
					break;
			}
		}
		break;

	case ALARM:
		if(button_count[11] == 1){				// if change to time display state -> not update time, reset state of alarm to AL_INIT
			status[0] = UART;
			status[2] = AL_INIT;
			display_ta_al(hour_temp, minute_temp, second_temp, day_temp, date_temp, month_temp, year_temp);
			clear_header();
		} else {
			switch(status[2]){
				case AL_INIT:
					if(status[0] == ALARM){		// if status of main fsm change to ALARM -> change to AL_DAY state
						status[2] = AL_DAY;
					} else {
						status[2] = AL_INIT;
					}
					break;

				case AL_DAY:
					// Show current state "ALARM - DAY"
					lcd_ShowStr(20, 50, "ALARM - DAY", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component: button "7"
						status[2] = AL_DATE;

						// update value of day
						day[1] = day_temp;

						// show the chosen alarm day to lcd screen - added after running and bug log
						lcd_ShowIntNum(20, 130, day_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_DAY;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							day_temp = day_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								day_temp = day_temp + 1;
							}
						}

						// if day exceeds 7, reset to 1
						if(day_temp >= 8) day_temp = 1;
						else if(day_temp <= 0) day_temp = 1;

						// toggle day value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(20, 130, day_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(20, 130, day_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case AL_DATE:
					// Show current state "ALARM - DATE"
					lcd_ShowStr(20, 50, "ALARM - DATE", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[2] = AL_MONTH;
						// update value of day
						date[1] = date_temp;

						// show the chosen alarm date to lcd screen - added after running and bug log
						lcd_ShowIntNum(70, 130, date_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_DATE;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							date_temp = date_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								date_temp = date_temp + 1;
							}
						}

						// go back to 1st date if the date exceeds corresponding value of each month
						// if months have 31 days
						if(month_temp==1 || month_temp==3 || month_temp==5 || month_temp==7 || month_temp==8 || month_temp==10 || month_temp==12){
							if(date_temp>=32){
								date_temp = 1;
							}
						} else if(month_temp == 2){		// if month 2
							if(leap_year(year_temp)){	// check leap year
								if(date_temp >= 30) date_temp = 1;	// if leap year -> 29 days maximum
							} else {								// else 28 days maximum
								if(date_temp >= 29) date_temp = 1;
							}
						} else {						// if months have 30 days
							if(date_temp >= 31) date_temp = 1;
						}

						// toggle date value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(70, 130, date_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(70, 130, date_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case AL_MONTH:
					// Show current state "ALARM - MONTH"
					lcd_ShowStr(20, 50, "ALARM - MONTH", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[2] = AL_YEAR;
						// update value of day
						month[1] = month_temp;

						// show the chosen alarm month to lcd screen - added after running and bug log
						lcd_ShowIntNum(110, 130, month_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_MONTH;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							month_temp = month_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								month_temp = month_temp + 1;
							}
						}

						// if month exceeds 12 -> reset to 1
						if(month_temp >= 13) month_temp = 1;

						// toggle month value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(110, 130, month_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(110, 130, month_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case AL_YEAR:
					// Show current state "ALARM - YEAR"
					lcd_ShowStr(20, 50, "ALARM - YEAR", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[2] = AL_HOUR;
						// update value of day
						year[1] = year_temp;

						// show the chosen alarm year to lcd screen - added after running and bug log
						lcd_ShowIntNum(150, 130, year_temp, 2, YELLOW, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_YEAR;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							year_temp = year_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								year_temp = year_temp + 1;
							}
						}

						// toggle year value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(150, 130, year_temp, 2, YELLOW, BLACK, 24);
						} else {
							lcd_ShowIntNum(150, 130, year_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case AL_HOUR:
					// Show current state "ALARM - HOUR"
					lcd_ShowStr(20, 50, "ALARM - HOUR", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[2] = AL_MINUTE;
						// update value of day
						hour[1] = hour_temp;

						// show the chosen alarm hour to lcd screen - added after running and bug log
						lcd_ShowIntNum(70, 100, hour_temp, 2, GREEN, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_HOUR;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							hour_temp = hour_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								hour_temp = hour_temp + 1;
							}
						}

						// if hour exceeds 59 -> reset to 1
						if(hour_temp >= 60) hour_temp = 1;

						// toggle hour value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(70, 100, hour_temp, 2, GREEN, BLACK, 24);
						} else {
							lcd_ShowIntNum(70, 100, hour_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case AL_MINUTE:
					// Show current state "ALARM - MINUTE"
					lcd_ShowStr(20, 50, "ALARM - MINUTE", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[2] = AL_SECOND;
						// update value of day
						minute[1] = minute_temp;

						// show the chosen alarm minute to lcd screen - added after running and bug log
						lcd_ShowIntNum(110, 100, minute_temp, 2, GREEN, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_MINUTE;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							minute_temp = minute_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								minute_temp = minute_temp + 1;
							}
						}

						// if minute exceeds 59 -> reset to 1
						if(minute_temp >= 60) minute_temp = 1;

						// toggle minute value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(110, 100, minute_temp, 2, GREEN, BLACK, 24);
						} else {
							lcd_ShowIntNum(110, 100, minute_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;

				case AL_SECOND:
					// Show current state "ALARM - SECOND"
					lcd_ShowStr(20, 50, "ALARM - SECOND", RED, BLUE, 24, 1);

					if(button_count[8] == 1){	// if confirm value and change to next time component
						status[2] = AL_INIT;
						status[0] = TIME_DISPLAY;
						// update value of day
						second[1] = second_temp;

						// turn on the alarm
						alarm_on = 1;

						// show the chosen alarm second to lcd screen - added after running and bug log
						lcd_ShowIntNum(150, 100, second_temp, 2, GREEN, BLACK, 24);

						// clear header
						clear_header();
					} else {
						status[2] = AL_SECOND;
						if(button_count[3] == 1){			// if press ^ -> increase value of day_temp
							second_temp = second_temp + 1;
						} else if(button_count[3] == 40){	// if press and hold
							flag_timer[4] = 0;
						} else if(button_count[3] > 40){
							if(flag_timer[4] == 1){
								flag_timer[4] = 0;
								second_temp = second_temp + 1;
							}
						}

						// if second exceeds 59 -> reset to 1
						if(second_temp >= 60) second_temp = 1;

						// toggle second value shown on screen to create animation
						if(toggle_at == 1){
							lcd_ShowIntNum(150, 100, second_temp, 2, GREEN, BLACK, 24);
						} else {
							lcd_ShowIntNum(150, 100, second_temp, 2, BLACK, BLACK, 24);
						}
					}
					break;
				default:
					break;
			}
		}
		break;

	case UART:
		// display UART mode
		lcd_ShowStr(20, 50, "UART CONFIG", RED, BLUE, 24, 1);

		// call API
		request_time_uart(&day_temp, &date_temp, &month_temp, &year_temp, &hour_temp, &minute_temp, &second_temp, &error_uart);

		// if there is not any error -> update time
		if(error_uart == 0){
			updateTime_ta_al(hour_temp, minute_temp, second_temp, day_temp, date_temp, month_temp, year_temp);
		}

		// clear header mode
		clear_header();

		// go back to display mode
		status[0] = TIME_DISPLAY;
		break;

	default:
		break;
	}
}
