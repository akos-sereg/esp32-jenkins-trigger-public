#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "include/led-displays.h"
#include "include/switches.h"
#include "include/networking.h"
#include "include/jenkins.h"

int CONTINUITY_CHECK = 0;

void refresh_led_screen() {
    int components = 0;

    // refresh red switch indicators
    gpio_set_level(GPIO_DISPLAY_SWITCH_1, SWITCH_1_STATE);
    gpio_set_level(GPIO_DISPLAY_SWITCH_2, SWITCH_2_STATE);
    gpio_set_level(GPIO_DISPLAY_SWITCH_3, SWITCH_3_STATE);

    // refresh authorization indicator (key switch)
    gpio_set_level(GPIO_DISPLAY_UNLOCKED, UNLOCK_STATE && UNLOCK_VERIFIED);

    // refresh continuity check
    if (SWITCH_1_STATE) components++;
    if (SWITCH_2_STATE) components++;
    if (SWITCH_3_STATE) components++;

    CONTINUITY_CHECK = UNLOCK_STATE
	    && UNLOCK_VERIFIED
	    && !IS_BUILD_IN_PROGRESS
	    && components == 1
	    && IS_WIFI_CONNECTED;

    gpio_set_level(GPIO_DISPLAY_PUSH_BUTTON, CONTINUITY_CHECK);

    // refresh build indicator
    gpio_set_level(GPIO_DISPLAY_BUILD_GREEN, IS_BUILD_SUCCEEDED);
    gpio_set_level(GPIO_DISPLAY_BUILD_RED, IS_BUILD_FAILED);
}

void flash_led_test() {

    int cnt = 0;

    for (cnt = 0; cnt != 7; cnt++) {
        gpio_set_level(GPIO_DISPLAY_NETWORK, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_UNLOCKED, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_BUILD_GREEN, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_BUILD_RED, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_DEPLOY_GREEN, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_DEPLOY_RED, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_SWITCH_1, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_SWITCH_2, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_SWITCH_3, cnt % 2);
        gpio_set_level(GPIO_DISPLAY_PUSH_BUTTON, cnt % 2);
        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

void setup_led_displays()
{
    gpio_set_direction(GPIO_DISPLAY_NETWORK, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_UNLOCKED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_BUILD_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_BUILD_RED, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(GPIO_DISPLAY_DEPLOY_GREEN);
    gpio_set_direction(GPIO_DISPLAY_DEPLOY_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_DEPLOY_RED, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(GPIO_DISPLAY_SWITCH_1);
    gpio_set_direction(GPIO_DISPLAY_SWITCH_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_SWITCH_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_SWITCH_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DISPLAY_PUSH_BUTTON, GPIO_MODE_OUTPUT);
}
