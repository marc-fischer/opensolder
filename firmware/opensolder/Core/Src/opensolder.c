/*
 * opensolder.c
 *
 * This file contains the main functions and variables for the OpenSolder
 * firmware
 *
 * License: GPL-3.0 or any later version
 * Copyright (c) 2022 Håvard Jakobsen
 *
 * TODO:
 * - implement better power regulation: PID or other alternative
 * - Better error handling
 * - Regular pcb temp readings for overheating protection and cold junction compensation
 * - Settings menu	--- problem: there is no eeprom. a settings menu is useless
 * - "sleep-mode" to halt all heating and just show OFF after a elapsed amount of time after standby
 */

#include <tip_thermocouple.h>
#include "opensolder.h"
#include "gui.h"
#include "timers.h"
#include "hid.h"

/******    File Scope Variables    ******/
static uint8_t system_state;		// error, idle, standby, sleep, operational, initial (default)
static uint8_t tool_state;			// heating, cooling, idle 	//	TODO: Move to separate class
static uint8_t toolholder_state;	// error, present, absent	//	TODO: Move to separate class
static uint8_t toolchanger_state;	// error, present, absent	//	TODO: Move to separate class


/******    Init    ******/
void opensolder_init(void) {
	timer_init();
	HAL_I2C_Init(&hi2c1);
	HAL_ADCEx_Calibration_Start(&hadc);
	HAL_Delay(50); // Wait for calibration to finish
	hid__init();
	init_display(SPLASHSCREEN_TIMEOUT_MS);
	system_state = INIT_STATE;
}

// --- handlers ---
/** opensolder synchronous main system state machine
 * main state machine controls:
 * - Display
 * - Menu
 * - Error, Idle, Standby, Sleep, Operational State
 *
 *
 * opensolder_main implements:
 * - system state
 * opensoder_main extends:
 * - tool state
 * - toolholder state
 * - toolchanger state
 * - display state
 * - tip heater state
 * - tip thermocouple state
 * - hid input state
 */
void opensolder_main(void) {
	static uint32_t standby_timeout_tick_ms = 0;
		static uint32_t tip_insert_delay_tick_ms = 0;
		uint8_t tool_tip_state = get_tip_state();

		if (HAL_GetTick() > get_ac_delay_tick()) {
			error_handler();
			display_message(AC_NOT_DETECTED);
			system_state = ERROR_STATE;
			return;
		}

		switch (system_state) {
		case INIT_STATE:
			set_tip_heater_off();
			draw_default_display();
			system_state = TIP_CHANGE_STATE;
			break;

		case TIP_CHANGE_STATE:
			set_tip_heater_off();
			if (tool_tip_state != TIP_DETECTED) {
				tip_insert_delay_tick_ms = HAL_GetTick() + TIP_CHANGE_DELAY_MS;
				display_message(tool_tip_state);
			} else if ((tool_tip_state == TIP_DETECTED)
					&& (HAL_GetTick() > tip_insert_delay_tick_ms)
					&& !tip_change_state) {
				draw_default_display();
				system_state = OFF_STATE;
			}
			break;

		case OFF_STATE:
			set_tip_heater_off();
			if (tip_change_state || (tool_tip_state != TIP_DETECTED)) {
				system_state = TIP_CHANGE_STATE;
			} else if (!tool_holder_state) {
				system_state = ON_STATE;
			}
			update_display();
			break;

		case ON_STATE:
			if (tip_change_state || (tool_tip_state != TIP_DETECTED)) {
				system_state = TIP_CHANGE_STATE;
			} else if (tool_holder_state) {
				standby_timeout_tick_ms = HAL_GetTick() + (STANDBY_TIME_S * 1000);
				system_state = STANDBY_STATE;
			}
			update_display();
			break;

		case STANDBY_STATE:
			if (tip_change_state || (tool_tip_state != TIP_DETECTED)) {
				system_state = TIP_CHANGE_STATE;
			} else if (!tool_holder_state) {
				system_state = ON_STATE;
			} else if (HAL_GetTick() > standby_timeout_tick_ms) {
				system_state = OFF_STATE;
			}
			update_display();
			break;

		case ERROR_STATE:
			error_handler();
			system_state = INIT_STATE;
			break;

		default:
			system_state = ERROR_STATE;
			break;
		}
}

// --- getters ---
uint8_t get_system_state(void) {
	return system_state;
}
