#ifndef UART_UTIL_H__
#define UART_UTIL_H__

#include <stdint.h>
#include "uart.h"
#include "lcd.h"
#include "software_timer.h"

#define MAX_BUFFER_SIZE 256

void request_time_uart(uint8_t *day, uint8_t *date, uint8_t *month, uint32_t *year, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *ret_error);

#endif
