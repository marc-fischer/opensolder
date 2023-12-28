/*
 * temperature.h
 *
 * License: GPL-3.0 or any later version
 * Copyright (c) 2022 Håvard Jakobsen
 */

#ifndef INC_TIP_TEMPERATURE_H
#define INC_TIP_TEMPERATURE_H

/******    Includes    ******/
#include "opensolder.h"

/******    Global Function Declarations    ******/
// getters
uint8_t tip_check(void);
uint16_t get_tip_temp(void);
uint16_t get_set_temp(void);
uint8_t get_tip_state(void);
uint32_t get_ac_delay_tick(void);
uint8_t get_power_bar_value(void);

// setters
void set_new_temp(uint16_t new_temp);
void heater_off(void);
void error_handler(void);

#endif
