#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/led-displays.h"
#include "include/switches.h"
#include "include/networking.h"
#include "include/jenkins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

void app_main()
{
    int cnt = 0;
    int isWifiConnectedRecently = 0;
    int jobCheckTimer = 0;

    setup_led_displays();
    setup_switches();

    initialize_wifi();
    read_switch_states();
    flash_led_test();
    refresh_led_screen();

    while(1) {
        vTaskDelay(500 / portTICK_RATE_MS);

        // jenkins indicator
        if (IS_BUILD_IN_PROGRESS) {
            gpio_set_level(GPIO_DISPLAY_BUILD_GREEN, cnt % 2);
            jobCheckTimer++;
            if (jobCheckTimer > 20) {
                jenkins_check_status();
                jobCheckTimer = 0;
            }
        }

        // network indicator
        if (IS_WIFI_CONNECTED) {
            if (!isWifiConnectedRecently) {
                gpio_set_level(GPIO_DISPLAY_NETWORK, 1);
                refresh_led_screen();
            }

            isWifiConnectedRecently = 1;
        } else {
            gpio_set_level(GPIO_DISPLAY_NETWORK, cnt % 2);
        }

	    cnt++;
	    if (cnt == 60000) {
	        cnt = 0;
	    }
    }
}

