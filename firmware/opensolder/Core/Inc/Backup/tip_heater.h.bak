/*
 * tip_heater.h
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */

#ifndef INC_TIP_HEATER_H_
#define INC_TIP_HEATER_H_

/******    Includes    ******/
#include "opensolder.h"
#include "timers.h"
#include "../../Drivers/tip_clamp/inc/tip_clamp.h"

// handlers
void heater_control(void);	// calculates how much energy must be put into the heater cartridge
void heater_control_timer_handler(void);
void error_handler(void);

// setters
void set_new_temp(uint16_t new_temp);
void set_tip_heater_off(void);
void set_tip_error_halt(void);

// getters
uint16_t get_set_temp(void);
uint8_t get_power_bar_value(void);

#endif /* INC_TIP_HEATER_H_ */
