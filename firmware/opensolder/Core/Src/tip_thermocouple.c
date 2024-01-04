/*
 * temperature.c
 *
 * License: GPL-3.0 or any later version
 * Copyright (c) 2022 Håvard Jakobsen
 *
 * modified to:
 * tip_thermocouple.c
 * 2023 Marc Fischer
 *
 * Primary purpose:
 * - This file provides a async state machine that handles the full ADC conversion including timings to read the thermocouple.
 * Main State Machine: thermocouple_read_timer_handler()
 */

/*
 * - NOTES -
 * temp	= temperature
 * tmp	= temporary
 * PA0	= THERMOCOUPLE_ADC
 * PA1	= TIP_CHECK
 * PA2	= TIP_CLAMP
 *
 * 4. HAL_ADC_ConvCpltCallback() calls adc_complete() when the ADC conversion is done
 *
 * The reason for these delays are to delay the ADC reading until the thermocouple amplifier
 * and low-pass filter have reached steady state.
 */

#include <tip_thermocouple.h>

/******    Local Function Declarations    ******/
static void start_adc(void);
static void adc_complete(void);
static void adc_calculate_buffer_average(void);
static void adc_to_temperature(void);
static void adc_deviation_check(void);

void thermocouple_error_handler(void);

/******    File Scope Variables    ******/
static uint16_t adc_buffer[ADC_BUFFER_LENGTH];
static uint32_t adc_buffer_average = 0;
static uint8_t adc_current_measurement = ADC_MEASURE_TEMPERATURE;

static uint16_t tip_temp = 0;

static volatile uint16_t tip_state = TIP_NOT_DETECTED;
static volatile uint16_t tip_check_counter = 0;
static volatile uint8_t tip_timer_delay_flag = SET;

static volatile uint8_t error_flag = RESET;



void tip_timer_handler(void) {
	/*
	 * Wait 2ms (one TIM7 period) after power is turned off for transients
	 * to settle, then remove thermo-couple clamp (pin PA2). Wait another 2ms
	 * for the RC pre-amp filter to settle before starting the ADC conversion
	 */

	// First period of TIM7, 2ms after true zero cross and disabling the power
	if (tip_timer_delay_flag == SET) {
		tip_timer_delay_flag = RESET;

		heater_driver__set_state(NOK);		// dis-allow the heater to be turned on (NOK = not ok to enable the heater)
		heater_driver__set_output_off();	// hard-disable the heater output to protect the op-amp
		tip_clamp_pin_disable();	// Set TIP_CLAMP pin to input state (high impedance)

		if (tip_check_counter > (TIP_CHECK_INTERVAL * 50)) {
			adc_current_measurement = ADC_CHECK_TIP_AVAILABLE;
			tip_check_pin_enable(); // Drive TIP_CHECK pin HIGH, if no tip is inserted the op-amp will saturate and ADC will read close to 4096.
			tip_check_counter = 0;
		}
	} else { 							// Second period of TIM7, 4ms after true zero cross and disabling the power
		tip_timer_delay_flag = SET;
		timer_stop_tip_measure_delay(); // Stop TIM7
		start_adc();				  	// Start ADC conversion in DMA mode
	}
}

void tip_check_counter_increase(void){
	tip_check_counter++;		// Increase counter every AC half cycle
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	tip_clamp_pin_enable();	// Drive TIP_CHECK pin LOW, this clamps thermo-couple signal to prevent transients and noise on the op-amp input
	heater_driver__set_state(OK);	// adc completed, allow the heater to be enabled again
	adc_complete();
}

static void start_adc(void) {
	HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc_buffer,
			(sizeof(adc_buffer) / sizeof(uint16_t)));
}

static void adc_complete(void) {
	adc_calculate_buffer_average();
	if (adc_current_measurement == ADC_CHECK_TIP_AVAILABLE) {
		adc_current_measurement = ADC_CHECK_TIP_AVAILABLE_WAIT;
		tip_check_pin_disable();
		tip_state = tip_check();
	} else if ((adc_current_measurement == ADC_MEASURE_TEMPERATURE) && (tip_state == TIP_DETECTED)) {
		adc_to_temperature();
		adc_deviation_check();
		if (error_flag == SET) {
			tip_temp = ADC_READING_ERROR;
			thermocouple_error_handler();
		} else if ((get_system_state() == ON_STATE)
				|| (get_system_state() == STANDBY_STATE)) {
			heater_control();
		}
	}
}

static void adc_to_temperature(void) {
	// Calculate tip temperature in Celsius
	tip_temp = (adc_buffer_average * 100 / 750) + 25;
}

static void adc_calculate_buffer_average(void) {
	// Calculate the average value of adc_buffer
	adc_buffer_average = 0;
	for (uint16_t i = 0; i < ADC_BUFFER_LENGTH; i++) {
		adc_buffer_average += adc_buffer[i];
	}
	adc_buffer_average /= ADC_BUFFER_LENGTH;
}

static void adc_deviation_check(void) {
	// Check if any values in adc_buffer deviates more than expected, if so set error_flag
	int16_t upper_limit = adc_buffer_average + ADC_MAX_DEVIATION;
	int16_t lower_limit = adc_buffer_average - ADC_MAX_DEVIATION;

	for (uint16_t i = 0; i < ADC_BUFFER_LENGTH; i++) {
		if ((adc_buffer[i] > upper_limit) || (adc_buffer[i] < lower_limit)) {
			error_flag = SET;
		}
	}
}

uint8_t tip_check(void) {
	/*
	 * - TIP CHECK -
	 * 1. tip_check_flag is set every TIP_CHECK_INTERVAL AC half cycles, and TIP_CHECK_PIN is pulled high
	 * 2. When ADC is finished, adc_complete() reset TIP_CHECK_PIN,
	 *    calculates the average value of the ADC buffer reading and calls tip_check() to update tip_state
	 */

	if (adc_buffer_average < ADC_TIP_MAX_VALUE){
		return TIP_DETECTED;
	} else {
		return TIP_NOT_DETECTED;
	}
	//TIP_CHECK_ERROR;	TODO: implement a actual handler for TIP_CHECK_ERROR
	}

/******    Other Functions   ******/
uint16_t get_tip_temp(void) {
	return tip_temp;
}

uint8_t get_tip_state(void) {
	return tip_state;
}

void thermocouple_error_handler(void){
	tip_state = TIP_CHECK_ERROR;
	error_flag = RESET;
}
