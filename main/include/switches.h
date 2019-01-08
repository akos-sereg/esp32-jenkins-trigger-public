#ifndef __switches_h_included__
#define __switches_h_included__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define GPIO_INPUT_IO_0     32 /* Key Switch */
#define GPIO_INPUT_IO_1	    36 /* Switch #3 */
#define GPIO_INPUT_IO_2	    34 /* Switch #2 */
#define GPIO_INPUT_IO_3	    35 /* Switch #1 */
#define GPIO_INPUT_IO_4     39 /* Red Button */

#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1) | (1ULL<<GPIO_INPUT_IO_2) | (1ULL<<GPIO_INPUT_IO_3) | (1ULL<<GPIO_INPUT_IO_4))
#define ESP_INTR_FLAG_DEFAULT 0

extern int UNLOCK_VERIFIED;
extern int UNLOCK_STATE;
extern int SWITCH_1_STATE;
extern int SWITCH_2_STATE;
extern int SWITCH_3_STATE;
extern int RED_BUTTON_STATE;

extern void listen_switches(void* arg);
extern void setup_switches();
extern void read_switch_states();

#endif
