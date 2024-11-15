/*
 * software_timer.c
 *
 *  Created on: Sep 24, 2023
 *      Author: HaHuyen
 */

#include "software_timer.h"

uint16_t flag_timer[5] = {0, 0, 0, 0, 0};
uint16_t timer_counter[5] = {0, 0, 0, 0, 0};
uint16_t timer_MUL[5] = {0, 0, 0, 0, 0};


#define TIMER_CYCLE_2 1
void timer_init(){
	HAL_TIM_Base_Start_IT(&htim2);
}

void setTimer(uint16_t duration, int idx){
	timer_MUL[idx] = duration/TIMER_CYCLE_2;
	timer_counter[idx] = timer_MUL[idx];
	flag_timer[idx] = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		for (int i = 0; i< 5; i++){
			if(timer_counter[i] > 0){
				timer_counter[i]--;
				if(timer_counter[i] == 0) {
					flag_timer[i] = 1;
					timer_counter[i] = timer_MUL[i];
				}
			}
		}
//		led7_Scan();
	}
}
