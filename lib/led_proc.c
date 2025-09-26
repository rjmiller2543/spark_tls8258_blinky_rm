/*
 * led_proc.c
 *
 *  Created on: Sep 24, 2025
 *      Author: robert.miller
 */
#include "led_proc.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

led_proc_error_type init_led_proc(struct led_proc_t * led_proc, led_t leds[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;
	// NULL checks
	if (led_proc == NULL)
		return LED_PROC_ERROR_TYPE_NULL;
	if (leds == NULL)
		return LED_PROC_ERROR_TYPE_NULL;
	else if (sizeof(leds) == 0)
		return LED_PROC_ERROR_TYPE_NULL;
	if (led_proc->led_init == NULL)
		return LED_PROC_ERROR_TYPE_NULL;
	if (led_proc->led_set_polarity == NULL)	//consider making this a warning and not hard returning an error
		return LED_PROC_ERROR_TYPE_NULL;
	if (led_proc->led_set_duty_cycle == NULL)	//consider making this a warning and not hard returning an error
		return LED_PROC_ERROR_TYPE_NULL;
	if (led_proc->led_get_state == NULL)
		return LED_PROC_ERROR_TYPE_NULL;

	for (int i = 0; i < num_leds; i++)
	{
		status = led_proc->led_init(&leds[i]);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type turn_led_on(struct led_proc_t * led_proc, led_t * led)
{
	return led_proc->led_set_polarity(led, LED_ON);
}

led_proc_error_type turn_leds_on(struct led_proc_t * led_proc, led_t * leds[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = turn_led_on(led_proc, leds[i]);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type turn_led_num_on(struct led_proc_t * led_proc, int led_num_in_array)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	if (led_proc->led_array[led_num_in_array].led_type == LED_TYPE_OUTPUT)
	{
		status = turn_led_on(led_proc, &led_proc->led_array[led_num_in_array]);
		led_proc->led_array[led_num_in_array].led_state.led_output_state = LED_ON;
	} else {
		return LED_PROC_ERROR_TYPE_WRONG_TYPE;
	}

	return status;
}

led_proc_error_type turn_leds_nums_on(struct led_proc_t * led_proc, int led_nums_in_array[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = turn_led_num_on(led_proc, led_nums_in_array[i]);
		led_proc->led_array[led_nums_in_array[i]].led_state.led_output_state = LED_ON;
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type turn_led_off(struct led_proc_t * led_proc, led_t * led)
{
	return led_proc->led_set_polarity(led, LED_OFF);
}

led_proc_error_type turn_leds_off(struct led_proc_t * led_proc, led_t * leds[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = turn_led_off(led_proc, leds[i]);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type turn_led_num_off(struct led_proc_t * led_proc, int led_num_in_array)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	status = turn_led_off(led_proc, &led_proc->led_array[led_num_in_array]);
	led_proc->led_array[led_num_in_array].led_state.led_output_state = LED_OFF;

	return status;
}

led_proc_error_type turn_leds_nums_off(struct led_proc_t * led_proc, int led_nums_in_array[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = turn_led_num_off(led_proc, led_nums_in_array[i]);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type toggle_led_ensure(struct led_proc_t * led_proc, led_t * led)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;
	int curr_led_state;
	status |= get_led_state(led_proc, led, &curr_led_state);
	if (status != LED_PROC_ERROR_TYPE_NONE)
		return status;

	if (curr_led_state == LED_ON)
	{
		status = turn_led_off(led_proc, led);
		status |= get_led_state(led_proc, led, &curr_led_state);
		if (status == LED_PROC_ERROR_TYPE_NONE && curr_led_state == LED_OFF)
			return LED_PROC_ERROR_TYPE_NONE;
		else if (curr_led_state != LED_OFF)
			return LED_PROC_ERROR_TYPE_BAD_STATE;
		else
			return status;
	}
	else
	{
		status = turn_led_on(led_proc, led);
		status |= get_led_state(led_proc, led, &curr_led_state);
		if (status == LED_PROC_ERROR_TYPE_NONE && curr_led_state == LED_ON)
			return LED_PROC_ERROR_TYPE_NONE;
		else if (curr_led_state != LED_ON)
			return LED_PROC_ERROR_TYPE_BAD_STATE;
		else
			return status;
	}
}

led_proc_error_type toggle_leds_ensure(struct led_proc_t * led_proc, led_t * leds[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = toggle_led_ensure(led_proc, leds[i]);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type toggle_led_num_ensure(struct led_proc_t * led_proc, int led_num_in_array)
{
	int curr_led_state;
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	status = toggle_led_ensure(led_proc, &led_proc->led_array[led_num_in_array]);
	status |= get_led_num_state(led_proc, led_num_in_array, &curr_led_state);

	// we need to make sure everything went well before we update the state of the led output
	if (status == LED_PROC_ERROR_TYPE_NONE)
		led_proc->led_array[led_num_in_array].led_state.led_output_state = (led_output_state_t)curr_led_state;

	return status;
}

led_proc_error_type toggle_leds_nums_ensure(struct led_proc_t * led_proc, int led_nums_in_array[], int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = toggle_led_num_ensure(led_proc, led_nums_in_array[i]);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type set_led_pwm_duty_cycle(struct led_proc_t * led_proc, led_t * led, int pwm_dc)
{
	return led_proc->led_set_duty_cycle(led, pwm_dc);
}

led_proc_error_type set_leds_pwm_duty_cycle(struct led_proc_t * led_proc, led_t * leds[], int pwm_dc, int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = set_led_pwm_duty_cycle(led_proc, leds[i], pwm_dc);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type set_led_num_pwm_duty_cycle(struct led_proc_t * led_proc, int led_num_in_array, int pwm_dc)
{
	return set_led_pwm_duty_cycle(led_proc, &led_proc->led_array[led_num_in_array], pwm_dc);
}

led_proc_error_type set_led_nums_pwm_duty_cycle(struct led_proc_t * led_proc, int led_nums_in_array[], int pwm_dc, int num_leds)
{
	led_proc_error_type status = LED_PROC_ERROR_TYPE_NONE;

	for (int i = 0; i < num_leds; i++)
	{
		status = set_led_pwm_duty_cycle(led_proc, &led_proc->led_array[led_nums_in_array[i]], pwm_dc);
		if (status != LED_PROC_ERROR_TYPE_NONE)
			return status;
	}

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type get_led_state(struct led_proc_t * led_proc, led_t * led, int * led_state)
{
	return led_proc->led_get_state(led, led_state);
}

led_proc_error_type get_led_num_state(struct led_proc_t * led_proc, int led_num_in_array, int * led_state)
{
	return get_led_state(led_proc, &led_proc->led_array[led_num_in_array], led_state);
}

