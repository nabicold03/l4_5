/*
 * main_fsm.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Admin
 */

#ifndef INC_MAIN_FSM_H_
#define INC_MAIN_FSM_H_

#include "global.h"
#include <stdbool.h>
#include "ta_fsm.h"
#include "al_fsm.h"
#include "ds3231.h"
#include "uart_util.h"
#include "software_timer.h"

void main_fsm();
bool is_alarm();

#endif /* INC_MAIN_FSM_H_ */
