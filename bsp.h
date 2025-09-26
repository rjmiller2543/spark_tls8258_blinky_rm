/*
 * bsp.h
 *
 *  Created on: Sep 24, 2025
 *      Author: robert.miller
 */

#ifndef VENDOR_TEL_TEST_BSP_H_
#define VENDOR_TEL_TEST_BSP_H_

#pragma once
#include "driver.h"
#include "common.h"
#include "./lib/led_proc.h"

#define CLOCK_SYS_CLOCK_HERTZ 24000000

#define LED_RED 	GPIO_PD5
#define LED_WHITE	GPIO_PD4
#define LED_GREEN	GPIO_PD3
#define LED_BLUE	GPIO_PD2

typedef enum LED_NUMS {
	LED_RED_NUM,
	LED_WHITE_NUM,
	LED_GREEN_NUM,
	LED_BLUE_NUM
}led_nums;

led_t red_led = {
		.led_ptr = LED_RED,
		.led_type = LED_TYPE_OUTPUT
};

led_t white_led = {
		.led_ptr = LED_WHITE,
		.led_type = LED_TYPE_PWM
};

led_t green_led = {
		.led_ptr = LED_GREEN,
		.led_type = LED_TYPE_OUTPUT
};

led_t blue_led = {
		.led_ptr = LED_BLUE,
		.led_type = LED_TYPE_OUTPUT
};

#define NUM_LEDS 4


#endif /* VENDOR_TEL_TEST_BSP_H_ */
