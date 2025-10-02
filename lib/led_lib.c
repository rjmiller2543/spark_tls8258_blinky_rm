/*
 * led_lib.c
 *
 *  Created on: Sep 24, 2025
 *      Author: robert.miller
 */
#include "led_lib.h"
#include "led_proc.h"
#include "../bsp.h"
#include "common.h"
#include "../app_config.h"


#define FLASH_ALL_LEDS	1
#define CYCLE_LEDS		2

#define LED_BEHAVIOR	CYCLE_LEDS

#if (LED_BEHAVIOR==CYCLE_LEDS)
volatile int cntr = 0;
#endif

led_proc_error_type init_led(led_t * led);
led_proc_error_type set_led_polarity(led_t * led, led_output_state_t state);
led_proc_error_type set_led_duty_cycle(led_t * led, int pwm_dc);
led_proc_error_type get_state_of_led(led_t * led, int * state);
led_proc_error_type deinit_led(led_t * led);


struct led_proc_t led_proc;
led_t bsp_leds[NUM_LEDS];


// PWM seems to require the irq_handler going by the examples
_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	if(pwm_get_interrupt_status(PWM_IRQ_PWM2_FRAME)){
		pwm_clear_interrupt_status(PWM_IRQ_PWM2_FRAME);
	}

	if(timer_get_interrupt_status(TMR_STA_TMR0))
	{
		timer_clear_interrupt_status(TMR_STA_TMR0); //clear irq status
#if (LED_BEHAVIOR==FLASH_ALL_LEDS)

		int toggling_led_nums[3] = { LED_RED_NUM, LED_GREEN_NUM, LED_BLUE_NUM };	// preferred method since it syncs the state and error checks the LED type
		toggle_leds_nums_ensure(&led_proc, toggling_led_nums, 3);

#elif (LED_BEHAVIOR==CYCLE_LEDS)
		if (cntr == 0)
			toggle_led_num_ensure(&led_proc, LED_RED_NUM);
		else if (cntr == 1)
			toggle_led_num_ensure(&led_proc, LED_GREEN_NUM);
		else if (cntr == 2)
			toggle_led_num_ensure(&led_proc, LED_BLUE_NUM);

		cntr++;
		if (cntr > 2)
			cntr = 0;
#endif
	}
}

led_proc_error_type init_led(led_t * led)
{
	if (led->led_type == LED_TYPE_OUTPUT)
	{
		gpio_set_output_en(led->led_ptr, 1); 		//enable output
		gpio_set_input_en(led->led_ptr,0);			//disable input
		gpio_write(led->led_ptr, 0);              	//LED Off
		led->led_state.led_output_state = LED_OFF;
	}
	else if (led->led_type == LED_TYPE_PWM)
	{
		app_led_pwm_info_t * info = (app_led_pwm_info_t *)led->led_state.led_pwm_state.led_pwm_info;
		gpio_set_func(led->led_ptr, info->pwm_type);			// white LED GPIO is on PWM2, this is being hard coded here, but needs to be NOTED
		pwm_set_mode(info->id, info->mode);
		pwm_set_cycle_and_duty(info->id, led->led_state.led_pwm_state.led_pwm_hertz * CLOCK_SYS_CLOCK_1US, led->led_state.led_pwm_state.led_duty_cycle);			// initialize the Duty Cycle to 0 and let the processor set the DC
		pwm_set_interrupt_enable(info->irq);
		irq_enable();
		pwm_start(info->id);
		//led->led_state.led_pwm_state.led_duty_cycle = 0;
	}
	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type set_led_polarity(led_t * led, led_output_state_t state)
{
	gpio_write(led->led_ptr, (unsigned int)state);
	// work around to fix issue with gpio_read function
	led->led_state.led_output_state = state;
	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type set_led_duty_cycle(led_t * led, int pwm_dc)
{
	app_led_pwm_info_t * info = (app_led_pwm_info_t *)led->led_state.led_pwm_state.led_pwm_info;
	pwm_set_cmp(info->id, pwm_dc * 10 * CLOCK_SYS_CLOCK_1US);
	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type get_state_of_led(led_t * led, int * state)
{
	// the gpio_read function appears to be broken
	//*state = gpio_read(led);
	//work around for gpio_read function
	*state = led->led_state.led_output_state;

	return LED_PROC_ERROR_TYPE_NONE;
}

led_proc_error_type deinit_led(led_t * led)
{
	// not implemented
	return LED_PROC_ERROR_TYPE_NONE;
}

void init_led_lib()
{
	pwm_set_clk(CLOCK_SYS_CLOCK_HERTZ, CLOCK_SYS_CLOCK_HERTZ);

	led_proc.led_init = init_led;
	led_proc.led_set_polarity = set_led_polarity;
	led_proc.led_set_duty_cycle = set_led_duty_cycle;
	led_proc.led_get_state = get_state_of_led;
	led_proc.led_deinit = deinit_led;

	white_led_pwm_info.id = PWM2_ID;
	white_led_pwm_info.irq = PWM_IRQ_PWM2_FRAME;
	white_led_pwm_info.mode = PWM_NORMAL_MODE;
	white_led_pwm_info.pwm_type = AS_PWM2_N;
	white_led.led_state.led_pwm_state.led_pwm_info = &white_led_pwm_info;

	bsp_leds[LED_RED_NUM] = red_led;
	bsp_leds[LED_WHITE_NUM] = white_led;
	bsp_leds[LED_GREEN_NUM] = green_led;
	bsp_leds[LED_BLUE_NUM] = blue_led;

	led_proc.led_array = bsp_leds;

	init_led_proc(&led_proc, bsp_leds, NUM_LEDS);

	timer0_set_mode(TIMER_MODE_SYSCLK,0,LED_TIMER_MS * CLOCK_SYS_CLOCK_1MS);
	timer_start(TIMER0);
	irq_enable();
}

void run_led_loop()
{
	int pwm_dc = 0;
	int pwm_up = 1;
	timer1_set_mode(TIMER_MODE_TICK,0,0);
	timer_start(TIMER1);
	while(1)
	{
		if(reg_tmr1_tick > 50 * CLOCK_SYS_CLOCK_1US * 1000)
		{
			// PWM on this pin is Inverted, bigger number is dimmer LED
			if (pwm_up == 1)
			{
				set_led_num_pwm_duty_cycle(&led_proc, LED_WHITE_NUM, pwm_dc);
				pwm_dc += 1;
				if (pwm_dc > LED_PWM_DIMMEST)
					pwm_up = 0;
			}
			else
			{
				set_led_num_pwm_duty_cycle(&led_proc, LED_WHITE_NUM, pwm_dc);
				pwm_dc -= 1;
				if (pwm_dc < LED_PWM_BRIGHTEST)
					pwm_up = 1;
			}
			reg_tmr1_tick = 0;
		}
	}
}
