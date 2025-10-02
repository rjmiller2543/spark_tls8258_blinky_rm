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
#define LED_PWM_HERTZ		1000
#define LED_TIMER_MS		500
#define LED_PWM_BRIGHTEST	0
#define LED_PWM_DIMMEST		100


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


typedef struct app_led_pwm_info_t {
	PWM_IRQ irq;
	pwm_id id;
	pwm_mode mode;
	GPIO_FuncTypeDef pwm_type;
}app_led_pwm_info_t;

app_led_pwm_info_t white_led_pwm_info;

led_t red_led = {
		.led_ptr = LED_RED,
		.led_type = LED_TYPE_OUTPUT
};

led_t white_led = {
		.led_ptr = LED_WHITE,
		.led_type = LED_TYPE_PWM,
		.led_state.led_pwm_state.led_duty_cycle = 95,
		.led_state.led_pwm_state.led_pwm_hertz = LED_PWM_HERTZ
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
