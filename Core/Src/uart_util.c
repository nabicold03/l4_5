#include "uart_util.h"

uint8_t ring_buffer[MAX_BUFFER_SIZE];
uint32_t buffer_head = 0;
uint32_t buffer_tail = 0;
volatile uint8_t uart_data_ready = 0;

static const uint8_t max_retries = 3;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == USART1) {
        uart_data_ready = 1;
        ring_buffer[buffer_head] = receive_buffer1;
        buffer_head = (buffer_head + 1) % MAX_BUFFER_SIZE;
        HAL_UART_Transmit(&huart1, &receive_buffer1, 1, 10);
        HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
    }
}

static uint8_t read_buffer(void) {
    if(buffer_head != buffer_tail) {    //  check buffer empty
        uint8_t data = ring_buffer[buffer_tail];
        buffer_tail = (buffer_tail + 1) % MAX_BUFFER_SIZE;
        return data;
    }
    return 0;
}

static void flush_buffer(void) {
    buffer_tail = buffer_head;
}

static void request_time_uart_helper(const char *request, const uint8_t min_value, const uint8_t max_value, uint32_t *ret_value, uint8_t *ret_error) {
    uint8_t retry_counter = 0;
    uint8_t digit;
    uint8_t has_digit = 0;
    uint8_t has_buffer_value = 0;
    uint32_t value = 0;
    while(retry_counter < max_retries) {
        uart_Rs232SendString((uint8_t *)request);
        setTimer(10000, 2);
        while(!flag_timer[2]) {
            digit = read_buffer();
            while(digit) {    //  valid value read from buffer
            	if(digit >= '0' && digit <= '9') {
            		value = value * 10 + (digit - '0');
                    has_digit = 1;
                    has_buffer_value = 1;
//                    break;
            	}
            	else {
            		uart_Rs232SendString((uint8_t *)"Invalid input detected!\r\n");
            		has_digit = 0;      //  invalid input like 21A will be discarded
            		flush_buffer();     //  flush buffer of any leftover invalid values
            		has_buffer_value = 1;
            		break;
            	}
            	digit = read_buffer();
            }
            if(has_buffer_value) {
            	break;
            }
        }

        //  10s timeout reached

        if(has_digit && (value >= min_value || value <= max_value)) {
//        	uart_Rs232SendString("OK\r\n");
        	*ret_value = value;
            *ret_error = 0;
            return;
        }
        retry_counter += 1;     //  timeout or no valid value
        has_buffer_value = 0;
    }
    *ret_error = 1;     //  error after 3 retries without success
}

/*
    error code:
    0: no error
    1: day request error
    2: date request error
    3: month request error
    4: year request error
    5: hour request error
    6: minute request error
    7: second request error
*/

void request_time_uart(uint8_t *day, uint8_t *date, uint8_t *month, uint32_t *year, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *ret_error) {
    uint8_t temp_error = 0;

    //######################### DAY ADJUST ###########################
    // Show message updating day
    lcd_ShowStr(20, 200, "UPDATING DAY...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Day\r\n", 1, 7, day, &temp_error);

    // if there is an error
    if(temp_error) {
    	// off message "updating day", change to "set day err"
    	lcd_ShowStr(20, 200, "UPDATING DAY...", BLACK, BLACK, 24, 1);
    	lcd_ShowStr(20, 200, "SET DAY ERR", RED, BLACK, 24, 1);

        *ret_error = 1;

        // return null
        return;
    }
    // off message "updating day"
    lcd_ShowStr(20, 200, "UPDATING DAY...", BLACK, BLACK, 24, 1);
    //###############################################################



    //######################### DATE ADJUST ###########################
    // Show message updating date
    lcd_ShowStr(20, 200, "UPDATING DATE...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Date\r\n", 1, 31, date, &temp_error);

    // if there is an error
    if(temp_error) {
    	// off message "updating date", change to "set date err"
    	lcd_ShowStr(20, 200, "UPDATING DATE...", BLACK, BLACK, 24, 1);
    	lcd_ShowStr(20, 200, "SET DATE ERR", RED, BLACK, 24, 1);

    	*ret_error = 2;
        return;
    }
    // off message "updating date"
    lcd_ShowStr(20, 200, "UPDATING DATE...", BLACK, BLACK, 24, 1);
    //###############################################################



    //######################### MONTH ADJUST ###########################
    // Show message updating month
    lcd_ShowStr(20, 200, "UPDATING MONTH...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Month\r\n", 1, 12, month, &temp_error);

    // if there is an error
    if(temp_error) {
    	// off message "updating month", change to "set month err"
    	lcd_ShowStr(20, 200, "UPDATING MONTH...", BLACK, BLACK, 24, 1);
    	lcd_ShowStr(20, 200, "SET MONTH ERR", BLACK, BLACK, 24, 1);

        *ret_error = 3;
        return;
    }
    // off message "updating month"
    lcd_ShowStr(20, 200, "UPDATING MONTH...", BLACK, BLACK, 24, 1);
    //###############################################################


    //######################### YEAR ADJUST ###########################
    lcd_ShowStr(20, 200, "UPDATING YEAR...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Year\r\n", 0, 0xFFFFFFFF, year, &temp_error);

    // if there is an error
    if(temp_error) {
    	// off message "updating year", change to "set year err"
    	lcd_ShowStr(20, 200, "UPDATING YEAR...", BLACK, BLACK, 24, 1);
    	lcd_ShowStr(20, 200, "SET MONTH ERR", BLACK, BLACK, 24, 1);

    	*ret_error = 4;
        return;
    }
    // off message "updating year"
    lcd_ShowStr(20, 200, "UPDATING YEAR...", BLACK, BLACK, 24, 1);
    //#################################################################



    //######################### HOUR ADJUST ###########################
    lcd_ShowStr(20, 200, "UPDATING HOUR...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Hour\r\n", 0, 23, hour, &temp_error);

    // if there is an error
    if(temp_error) {
    	// off message "updating hour", change to "set hour err"
    	lcd_ShowStr(20, 200, "UPDATING HOUR...", BLACK, BLACK, 24, 1);
    	lcd_ShowStr(20, 200, "SET HOUR ERR", BLACK, BLACK, 24, 1);
        *ret_error = 5;
        return;
    }
    // off message "updating hour"
    lcd_ShowStr(20, 200, "UPDATING HOUR...", BLACK, BLACK, 24, 1);
    //#################################################################



    //######################### MIN ADJUST ###########################
    lcd_ShowStr(20, 200, "UPDATING MIN...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Minute\r\n", 0, 59, minute, &temp_error);

    // if there is an error
    if(temp_error) {
    	// off message "updating min", change to "set min err"
    	lcd_ShowStr(20, 200, "UPDATING MIN...", BLACK, BLACK, 24, 1);
    	lcd_ShowStr(20, 200, "SET MIN ERR", BLACK, BLACK, 24, 1);

        *ret_error = 6;
        return;
    }
    // off message "updating hour"
    lcd_ShowStr(20, 200, "UPDATING MIN...", BLACK, BLACK, 24, 1);
    //###############################################################



    //######################### SEC ADJUST ###########################
    lcd_ShowStr(20, 200, "UPDATING SEC...", GREEN, BLACK, 24, 1);

    request_time_uart_helper("Second\r\n", 0, 59, second, &temp_error);

    // if there is an error
    if(temp_error) {
    	lcd_ShowStr(20, 200, "SET SEC ERR", BLACK, BLACK, 24, 1);
        *ret_error = 7;
        return;
    }
    lcd_ShowStr(20, 200, "UPDATING SEC...", BLACK, BLACK, 24, 1);
    *ret_error = 0;
//    uart_Rs232SendString(()sprintf("Send %d, %d / %d / %d, %d : %d : %d\r\n", *day, *date, *month, *year, *hour, *minute, *second));
    //###############################################################
}
