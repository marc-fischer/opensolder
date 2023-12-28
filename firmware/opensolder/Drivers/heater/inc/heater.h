/*
 * heater.h
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */

#ifndef HEATER_INC_HEATER_H_
#define HEATER_INC_HEATER_H_
#include "opensolder.h"

enum heater_driver_states {
	OK = 0,
	NOK = 1,
	FORBIDDEN = 2
};

void set_heater_state(uint8_t state);
uint8_t get_heater_state(void);

void set_heater_output_on(void);
void set_heater_output_off(void);

#endif /* HEATER_INC_HEATER_H_ */
