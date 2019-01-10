#include "include/switches.h"
#include "include/led-displays.h"
#include "include/jenkins.h"

int UNLOCK_VERIFIED = 0; // if someone left the key switch ON, this should never be 1
int UNLOCK_STATE = -1;
int SWITCH_1_STATE = -1;
int SWITCH_2_STATE = -1;
int SWITCH_3_STATE = -1;
int RED_BUTTON_STATE = -1;

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void read_switch_states() {
    UNLOCK_STATE = gpio_get_level(GPIO_INPUT_IO_0);
    SWITCH_3_STATE = gpio_get_level(GPIO_INPUT_IO_1);
    SWITCH_2_STATE = gpio_get_level(GPIO_INPUT_IO_2);
    SWITCH_1_STATE = gpio_get_level(GPIO_INPUT_IO_3);
    RED_BUTTON_STATE = gpio_get_level(GPIO_INPUT_IO_4);
}

void listen_switches(void* arg)
{
    uint32_t io_num;
    int current_state;

    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            current_state = gpio_get_level(io_num);
            printf("GPIO[%d] state: %d\n", io_num, current_state);

            if (io_num == 32 && UNLOCK_STATE != current_state) {
                // we accept OFF - ON transitions only when checking UNLOCK_STATE
                // eg. if someone left the key switch ON, then turned the board Off and On again, we should
                // not allow using the board
                if (UNLOCK_STATE == 0 && current_state == 1) {
                    UNLOCK_VERIFIED = 1;
                    IS_BUILD_IN_PROGRESS = 0;
                    IS_BUILD_FAILED = 0;
                    IS_BUILD_SUCCEEDED = 0;
                }

                if (UNLOCK_STATE == 1 && current_state == 0) {
                    // reset board
                    IS_BUILD_IN_PROGRESS = 0;
                    IS_BUILD_FAILED = 0;
                    IS_BUILD_SUCCEEDED = 0;
                }

                UNLOCK_STATE = current_state;
                refresh_led_screen();
            }

            if (io_num == 36 && SWITCH_3_STATE != current_state) {
                SWITCH_3_STATE = current_state;
                refresh_led_screen();
            }

            if (io_num == 34 && SWITCH_2_STATE != current_state) {
                SWITCH_2_STATE = current_state;
                refresh_led_screen();
            }

            if (io_num == 35 && SWITCH_1_STATE != current_state) {
                SWITCH_1_STATE = current_state;
                refresh_led_screen();
            }

            if (io_num == 39 && RED_BUTTON_STATE != current_state) {
                RED_BUTTON_STATE = current_state;
                if (current_state && SWITCH_1_STATE && CONTINUITY_CHECK) {
                    trigger_build(1);
                }
                if (current_state && SWITCH_2_STATE && CONTINUITY_CHECK) {
                    trigger_build(2);
                }
                if (current_state && SWITCH_3_STATE && CONTINUITY_CHECK) {
                    trigger_build(3);
                }

                refresh_led_screen();
            }
        }
    }
}

void setup_switches()
{
    gpio_config_t io_conf;

    // disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;

    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;

    // interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;

    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;

    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;

    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(GPIO_INPUT_IO_1, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(GPIO_INPUT_IO_2, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(GPIO_INPUT_IO_3, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(GPIO_INPUT_IO_4, GPIO_INTR_ANYEDGE);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // start gpio task
    xTaskCreate(listen_switches, "listen_switches", (1024 * 32), NULL, 5, NULL);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void*) GPIO_INPUT_IO_2);
    gpio_isr_handler_add(GPIO_INPUT_IO_3, gpio_isr_handler, (void*) GPIO_INPUT_IO_3);
    gpio_isr_handler_add(GPIO_INPUT_IO_4, gpio_isr_handler, (void*) GPIO_INPUT_IO_4);

    // remove isr handler for gpio number.
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    gpio_isr_handler_remove(GPIO_INPUT_IO_1);
    gpio_isr_handler_remove(GPIO_INPUT_IO_2);
    gpio_isr_handler_remove(GPIO_INPUT_IO_3);
    gpio_isr_handler_remove(GPIO_INPUT_IO_4);

    // hook isr handler for specific gpio pin again
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);
    gpio_isr_handler_add(GPIO_INPUT_IO_2, gpio_isr_handler, (void*) GPIO_INPUT_IO_2);
    gpio_isr_handler_add(GPIO_INPUT_IO_3, gpio_isr_handler, (void*) GPIO_INPUT_IO_3);
    gpio_isr_handler_add(GPIO_INPUT_IO_4, gpio_isr_handler, (void*) GPIO_INPUT_IO_4);
}

