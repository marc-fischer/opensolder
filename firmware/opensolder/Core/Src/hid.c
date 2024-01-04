/*
 * hid.c
 *
 *  Created on: Dec 29, 2023
 *      Author: marc
 */

#include "hid.h"

#include "button.h"
#include "encoder.h"

static button tool_holder_sensor; 	// Detects when tool is placed in holder
static button tip_change_sensor; 	// Detects when tool touches the tip change bracket
static button mmi_button;		  	// Front rotary encoder button
static encoder mmi_encoder;		  	// Front rotary encoder
static uint8_t mmi_button_event;
static uint8_t mmi_encoder_event;
static uint8_t tool_holder_state;
static uint8_t tip_change_state;

// --- handlers ---
void hid__init(void){
	button_init(&tool_holder_sensor, STAND_GPIO_Port, STAND_Pin, INVERTED);
	button_init(&tip_change_sensor, TIP_REMOVER_GPIO_Port, TIP_REMOVER_Pin, INVERTED);
	button_init(&mmi_button, ENC_SW_GPIO_Port, ENC_SW_Pin, INVERTED);
	encoder_init(&mmi_encoder, TIM2);
}

void hid__poll(void){
	mmi_button_event = button_event(&mmi_button);
	mmi_encoder_event = encoder_event(&mmi_encoder);

	static uint32_t standby_delay_tick_ms = 0;
	static uint32_t tip_change_delay_tick_ms = 0;

	if (button_state(&tool_holder_sensor)) {
		tool_holder_state = SET;
		standby_delay_tick_ms = HAL_GetTick() + STANDBY_DELAY_MS;
	} else if (HAL_GetTick() > standby_delay_tick_ms) {
		tool_holder_state = RESET;
	}

	if (button_state(&tip_change_sensor)) {
		tip_change_state = SET;
		tip_change_delay_tick_ms = HAL_GetTick() + TIP_CHANGE_DELAY_MS;
	} else if (HAL_GetTick() > tip_change_delay_tick_ms) {
		tip_change_state = RESET;
	}

	if (mmi_encoder_event != NO_CHANGE) {
		int16_t new_temp = get_set_temp();
		new_temp += (TEMP_STEPS * get_encoder_delta(&mmi_encoder));

		if (new_temp > MAX_TEMP) {
			new_temp = MAX_TEMP;
		} else if (new_temp < MIN_TEMP) {
			new_temp = MIN_TEMP;
		}

		set_new_temp(new_temp);
	}
	button_scan(&tool_holder_sensor);
	button_scan(&tip_change_sensor);
	button_scan(&mmi_button);
}

// --- getters ---
uint8_t hid__get_button_state(void){
	return button_state(&mmi_button);
}

uint8_t hid__get_stand_state(void){
	return button_state(&tool_holder_sensor);
}

uint8_t hid__get_tip_change_state(void){
	return button_state(&tip_change_sensor);
}
