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
#include "../../Drivers/heater/inc/heater.h"
#include "../../Drivers/tip_check/inc/tip_check.h"
#include "../../Drivers/tip_clamp/inc/tip_clamp.h"
#include "tip_heater.h"

/******    Global Function Declarations    ******/


enum adc_states {
	ADC_CHECK_TIP_AVAILABLE = 0,
	ADC_CHECK_TIP_AVAILABLE_WAIT = 1,
	ADC_MEASURE_TEMPERATURE = 2,
};

// handlers
void tip_check_counter_increase(void);	// called every zero cross
void tip_timer_handler(void);

// getters
uint8_t tip_check(void);
uint16_t get_tip_temp(void);
uint8_t get_tip_state(void);

#endif
