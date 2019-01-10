#ifndef __led_displays_h_included__
#define __led_displays_h_included__

#define GPIO_DISPLAY_NETWORK		33
#define GPIO_DISPLAY_UNLOCKED		25
#define GPIO_DISPLAY_BUILD_GREEN	26
#define GPIO_DISPLAY_BUILD_RED		23
#define GPIO_DISPLAY_DEPLOY_GREEN	1
#define GPIO_DISPLAY_DEPLOY_RED		22
#define GPIO_DISPLAY_SWITCH_1		3
#define GPIO_DISPLAY_SWITCH_2		18
#define GPIO_DISPLAY_SWITCH_3		5
#define GPIO_DISPLAY_PUSH_BUTTON	17

/**
  Configure led display GPIOs
*/
extern void setup_led_displays();

/**
  Refresh feedback led GPIO states
*/
extern void refresh_led_screen();

/**
  LED test flashing when board is initializing
*/
extern void flash_led_test();

/**
  Reflects that all conditions are green for the red push button to operate
*/
extern int CONTINUITY_CHECK;

#endif