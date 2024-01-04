/*
 * timers.h
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */

#ifndef INC_TIMERS_H_
#define INC_TIMERS_H_

/******    Includes    ******/
#include "opensolder.h"
#include "tip_heater.h"
#include "tip_thermocouple.h"
#include "hid.h"
// API
// --- getters ---
uint32_t get_ac_delay_tick(void);

// --- setters ---
void timer_stop_zerocross_timeout(void);
void timer_start_zerocross_timeout(void);
void timer_stop_tip_measure_delay(void);
void timer_start_tip_measure_delay(void);

// --- handlers ---
void timer_init(void);

#endif /* INC_TIMERS_H_ */
