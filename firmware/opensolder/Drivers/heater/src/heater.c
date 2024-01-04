/*
 * heater.c
 *
 *	This file keeps track of the current heater state and does not allow the heater to be turned on if any error state is set
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */

#include "../inc/heater.h"

uint8_t heater_driver__current_state = OK;	// state mutex, defining if the heater output is allowed to be switched on

void heater_driver__set_state(uint8_t state){
	heater_driver__current_state = state;
}
uint8_t heater_driver__get_state(void) {
	return heater_driver__current_state;
}

void heater_driver__set_output_on(void) {
	if(heater_driver__current_state == OK) {	// if an error is present, the output is not allowed to turn on
		HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, ON); // Turn heater on
	} else {
		heater_driver__set_output_off();	// Any other case turn the heater off
	}
}
void heater_driver__set_output_off(void) {
	HAL_GPIO_WritePin(HEATER_GPIO_Port, HEATER_Pin, OFF); // Turn heater off (ignores the forbidden flag, OFF can be a safety state)
}

