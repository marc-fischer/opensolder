/*
 * tip_heater.c
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */


#include "tip_heater.h"
#include "../../Drivers/heater/inc/heater.h"	// HW driver layer to enable/disable the header.

static uint16_t set_temp = DEFAULT_TEMP;

static volatile uint8_t on_periods = 0;
static volatile uint8_t power_bar_value = 0;
static volatile uint32_t heater_power_history = 0; // DEBUG only - could be used for power histogram

// This function is called after the zerocross interrupt timeout happened
void heater_control_timer_handler(void) {
	heater_power_history <<= 1; // Records the tip power history of the past 32 AC half cycles. Power to tip = 1, no power = 0
	power_bar_value = on_periods;

	// Switch heater on or off
	if ((on_periods >= 1) && (get_tip_temp() < MAX_TEMP)) {
		set_heater_output_on();
		on_periods--;					// decrease the on-duration before next temp measurement happens
		heater_power_history++;
	} else {
		set_heater_output_off(); 							// Turn heater OFF
		timer_start_tip_measure_delay();					// Start TIM7 to read tip temperature
	}
}

void heater_control(void) {
	/*
	 * Decide if power should be delivered to tip next half period, and
	 * how many periods until next temp read (how much power to apply).
	 *
	 * Full power is one read period (OFF) per 4 power periods (ON).
	 * Number of power periods before next read period decreases as
	 * temperature closes in on set_temp to prevent too much overshoot.
	 */

	uint16_t tmp_set_temp = set_temp;
	if ((get_system_state() == STANDBY_STATE)
			&& (tmp_set_temp > STANDBY_TEMP)) {
		tmp_set_temp = STANDBY_TEMP;
	}

	// Turn heater ON/OFF
	if ((get_tip_temp() + 3) < tmp_set_temp) {		/// TODO: 3°C fixed hysteresis
		uint16_t temperature_error = tmp_set_temp - get_tip_temp();
		on_periods = temperature_error / 10;// regulator assumption: temperature_set - temperature_is / 10 = half waves to stay on
		if (on_periods > MAX_ON_PERIODS) {
			on_periods = MAX_ON_PERIODS;
		} else if (on_periods == 0) {
			on_periods = 1;
		}
	} else {
		on_periods = 0;	// disable on next interrupt
	}
}

void set_tip_heater_off(void) {
	on_periods = 0;
}

void set_new_temp(uint16_t new_temp) {
	set_temp = new_temp;
}

uint16_t get_set_temp(void) {
	return set_temp;
}


void set_tip_error_halt(void) {
	// Turn heater hard OFF
	set_tip_heater_off();		// disable the tip heater regulator
	set_heater_output_off();	// disable the heater output hardware pin
}

void error_handler(void) {
	set_tip_error_halt();
}

uint8_t get_power_bar_value(void) {
	return power_bar_value;
}
