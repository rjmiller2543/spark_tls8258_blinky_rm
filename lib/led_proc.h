/*
 * led_proc.h
 *
 *  Created on: Sep 24, 2025
 *      Author: robert.miller
 */

#ifndef VENDOR_TEL_TEST_LIB_LED_PROC_H_
#define VENDOR_TEL_TEST_LIB_LED_PROC_H_

#include "common.h"

typedef enum LED_PROC_ERROR_TYPES {
	LED_PROC_ERROR_TYPE_NONE = 1,		// No errors
	LED_PROC_ERROR_TYPE_WRONG_TYPE,		// Passed LED is of wrong type
	LED_PROC_ERROR_TYPE_NULL,			// No LED was passed or LED passed is NULL
	LED_PROC_ERROR_TYPE_BAD_STATE,
	LED_PROC_ERROR_TYPE_UNKNOWN
}led_proc_error_type;

typedef enum LED_OUTPUT_STATE {
	LED_OFF,
	LED_ON
}led_output_state_t;

typedef enum LED_TYPE {
	LED_TYPE_OUTPUT,
	LED_TYPE_PWM
}led_type_t;

typedef struct led_pwm_state_t{
	int led_pwm_hertz;
	int led_duty_cycle;
	void * led_pwm_info;	// optional generic void to define and make a struct on application side to reference any other pwm specific APIs required
}led_pwm_state_t;

union led_state_t {
	led_output_state_t led_output_state;
	led_pwm_state_t led_pwm_state;
};

/******* NOTE! *******
 * User MUST change the led_ptr type to the GPIO TypeDef for the selected API SDK
 */
#warning "User Must Change GPIO_PinTypeDef to correct SDK GPIO Typedef"
typedef struct LED {
	GPIO_PinTypeDef led_ptr;
	led_type_t led_type;
	union led_state_t led_state;
	int led_pwm_hertz;
}led_t;



/**************************************************************/
/**\name	led_proc_t   			                          */
/**************************************************************/
/*!
 *	@brief This struct is used to keep the led processor library generic
 *	a separate file must be created, #include "led_proc.h" and each of the functions
 *	below *MUST* be created and passed as a reference to the struct.  The init will fail
 *	if any of the functions do not exist
 *
 *	 @param led_init
 *	 	a function for initializing a single LED, this needs to take the led_t struct above
 *	 	as a parameter.  Within the initialization, setup the LED as either an Output or a PWM
 *
 *	 @param led_set_polarity
 *	 	for setting the LED GPIOs output.  The parameters are for LED_ON or LED_OFF. It is up to the
 *	 	developer to know whether that means the GPIO is a sink and needs to be set to 0 to turn on the LED
 *	 	or if the GPIO is push / pull and needs to be set to 1 to turn on the LED
 *
 *	 @param led_set_duty_cycle
 *	 	for setting the duty cycle of a PWM LED
 *
 *	 @param led_get_state
 *	 	for getting the current state of an LED, a reference to an Integer is passed and is where the state should
 *	 	be set to and return an error state.  Developer should follow the led_output_state_t and return if LED is ON or OFF
 *	 	not if it's set to 1 or 0
 *
 *	 @param led_deinit
 *	 	for deinitializing an LED and cleanup, or if GPIO needs to be reused for another function
 *
 *	 @param led_array
 *	 	a reference to array of led_t types
 *
 *	 @param led_typedef
 *	 	the actual typedef of the GPIO, for instance GPIO_Typedef
 *
 *
 *
 *
 *	@return led_proc_error_type - results of LED initializations
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
// to keep the LED Typecast as generic as possible, using void
// keeping a reference to what the LED Type is, so we can make sure that what we're turning on and off is indeed an LED
typedef struct led_proc_t {
	led_proc_error_type (*led_init)(led_t*);
	led_proc_error_type (*led_set_polarity)(led_t*, led_output_state_t);
	led_proc_error_type (*led_set_duty_cycle)(led_t*, int);
	led_proc_error_type (*led_get_state)(led_t*, int*);
	led_proc_error_type (*led_deinit)(led_t*);
	led_t *led_array;
	void *led_typedef;
}led_proc_t;



/**************************************************************/
/**\name	init_led_proc			                          */
/**************************************************************/
/*!
 *	@brief This function is to initialize an array of LEDs
 *
 *	 @param led_proc_t structure pointer.
 *	 @param leds - an array of LEDs in the typedef for the MCU SDK
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *	@return led_proc_error_type - results of LED initializations
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type init_led_proc(struct led_proc_t * led_proc, led_t leds[], int num_leds);



/**************************************************************/
/**\name	turn_led_on			                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn on LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param led_t
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning on LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_led_on(struct led_proc_t * led_proc, led_t * led);



/**************************************************************/
/**\name	turn_leds_on			                          */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn on LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param array of led_t
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning on LEDs
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_leds_on(struct led_proc_t * led_proc, led_t * leds[], int num_leds);



/**************************************************************/
/**\name	turn_led_on 		                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn on LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - the place in the LED array that is to be turned on
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning on LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_led_num_on(struct led_proc_t * led_proc, int led_num_in_array);



/**************************************************************/
/**\name	turn_leds_on 		                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn on LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - the place in the LED array that is to be turned on
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning on LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_leds_nums_on(struct led_proc_t * led_proc, int led_nums_in_array[], int num_leds);



/**************************************************************/
/**\name	turn_led_off 		                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn off LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param led_t
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning off LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_led_off(struct led_proc_t * led_proc, led_t * led);



/**************************************************************/
/**\name	turn_leds_off 		                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn off LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param array of led_t
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning off LEDs
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_leds_off(struct led_proc_t * led_proc, led_t * leds[], int num_leds);



/**************************************************************/
/**\name	turn_led_num_off 		                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn off LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - the place in the LED array that is to be turned on
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning off LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_led_num_off(struct led_proc_t * led_proc, int led_num_in_array);



/**************************************************************/
/**\name	turn_leds_nums_off 		                              */
/**************************************************************/
/*!
 *	@brief This function is to explicitly turn off LED.  It is up to the user LED Lib code to ensure that an LED is set to
 *		high or low depending on if the GPIO is a push / pull or a sink
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int array - the place in the LED array that is to be turned on
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of turning off LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type turn_leds_nums_off(struct led_proc_t * led_proc, int led_num_in_array[], int num_leds);



/**************************************************************/
/**\name	toggle_led_ensure 		                          */
/**************************************************************/
/*!
 *	@brief This function is to toggle an LED to its opposite.  It is meant to check states before and after to ensure that
 *		it has indeed toggled
 *
 *	 @param led_proc_t structure pointer.
 *	 @param led_t
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of toggling LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type toggle_led_ensure(struct led_proc_t * led_proc, led_t * led);



/**************************************************************/
/**\name	toggle_leds_ensure 		                          */
/**************************************************************/
/*!
 *	@brief This function is to toggle an LED to its opposite.  It is meant to check states before and after to ensure that
 *		it has indeed toggled
 *
 *	 @param led_proc_t structure pointer.
 *	 @param array of led_t
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of toggling LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type toggle_leds_ensure(struct led_proc_t * led_proc, led_t * leds[], int num_leds);



/**************************************************************/
/**\name	toggle_led_num_ensure 		                          */
/**************************************************************/
/*!
 *	@brief This function is to toggle an LED to its opposite.  It is meant to check states before and after to ensure that
 *		it has indeed toggled
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - the place in the LED array that is to be toggled
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of toggling LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type toggle_led_num_ensure(struct led_proc_t * led_proc, int led_num_in_array);



/**************************************************************/
/**\name	toggle_leds_nums_ensure 		                          */
/**************************************************************/
/*!
 *	@brief This function is to toggle an LED to its opposite.  It is meant to check states before and after to ensure that
 *		it has indeed toggled
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int array - the places in the LED array that is to be toggled
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *
 *	@return led_proc_error_type - result of toggling LED
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type toggle_leds_nums_ensure(struct led_proc_t * led_proc, int led_nums_in_array[], int num_leds);



/**************************************************************/
/**\name	set_led_pwm_duty_cycle 		                              */
/**************************************************************/
/*!
 *	@brief This function is to set the PWM Duty Cycle of an LED
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - led number in array needed to get state
 *	 @param reference to uint8_t to pass the state of the LED
 *
 *
 *
 *
 *	@return led_proc_error_type - result of getting LED state
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type set_led_pwm_duty_cycle(struct led_proc_t * led_proc, led_t * led, int pwm_dc);



/**************************************************************/
/**\name	set_leds_pwm_duty_cycle 		                              */
/**************************************************************/
/*!
 *	@brief This function is to set the PWM Duty Cycle of multiple LEDs
 *
 *	 @param led_proc_t structure pointer.
 *	 @param array of led_t - led number in array needed to get state
 *	 @param reference to uint8_t to pass the state of the LED
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *	@return led_proc_error_type - result of getting LED state
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type set_leds_pwm_duty_cycle(struct led_proc_t * led_proc, led_t * leds[], int pwm_dc, int num_leds);



/**************************************************************/
/**\name	set_led_num_pwm_duty_cycle 		                              */
/**************************************************************/
/*!
 *	@brief This function is to set the PWM Duty Cycle of an LED by the number associated with LED in the array
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - led number in array needed to get state
 *	 @param reference to uint8_t to pass the state of the LED
 *
 *
 *
 *
 *	@return led_proc_error_type - result of getting LED state
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type set_led_num_pwm_duty_cycle(struct led_proc_t * led_proc, int led_num_in_array, int pwm_dc);



/**************************************************************/
/**\name	set_led_nums_pwm_duty_cycle 		                              */
/**************************************************************/
/*!
 *	@brief This function is to set the PWM Duty Cycle of multiple LEDs by the number associated with LED in the array
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - led number in array needed to get state
 *	 @param reference to uint8_t to pass the state of the LED
 *	 @param int - the number of LEDs in the array
 *
 *
 *
 *
 *	@return led_proc_error_type - result of getting LED state
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type set_led_nums_pwm_duty_cycle(struct led_proc_t * led_proc, int led_nums_in_array[], int pwm_dc, int num_leds);



/**************************************************************/
/**\name	get_led_state 		                              */
/**************************************************************/
/*!
 *	@brief This function is to get the state of a specific LED
 *
 *	 @param led_proc_t structure pointer.
 *	 @param led_t needed to get state
 *	 @param reference to uint8_t to pass the state of the LED
 *
 *
 *
 *
 *	@return led_proc_error_type - result of getting LED state
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type get_led_state(struct led_proc_t * led_proc, led_t * led, int * led_state);



/**************************************************************/
/**\name	get_led_state 		                              */
/**************************************************************/
/*!
 *	@brief This function is to get the state of a specific LED
 *
 *	 @param led_proc_t structure pointer.
 *	 @param int - led number in array needed to get state
 *	 @param reference to uint8_t to pass the state of the LED
 *
 *
 *
 *
 *	@return led_proc_error_type - result of getting LED state
 *	@retval 1 -> Success
 *	@retval all else -> Error (see descriptions above
 *
 *
*/
led_proc_error_type get_led_num_state(struct led_proc_t * led_proc, int led_num_in_array, int * led_state);

#endif /* VENDOR_TEL_TEST_LIB_LED_PROC_H_ */
