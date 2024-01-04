/*
 * heater.h
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */

#ifndef HEATER_INC_HEATER_H_
#define HEATER_INC_HEATER_H_
#include "opensolder.h"

enum heater_driver__states {
	OK = 0,		// OUTPUT is operational
	NOK = 1,	// error detected, output not operational
	FORBIDDEN = 2	// forbidden to enable the output (safety disable)
};

void heater_driver__set_state(uint8_t state);
uint8_t heater_driver__get_state(void);

void heater_driver__set_output_on(void);
void heater_driver__set_output_off(void);

#endif /* HEATER_INC_HEATER_H_ */
