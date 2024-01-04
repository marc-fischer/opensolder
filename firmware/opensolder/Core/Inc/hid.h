/*
 * hid.h
 *
 *  Created on: Dec 29, 2023
 *      Author: marc
 */

#ifndef INC_HID_H_
#define INC_HID_H_

#include "button.h"
#include "encoder.h"
#include "opensolder.h"

enum hid_states {
	BUTTON_STATE_NOT_PRESSED = 0,
	BUTTON_STATE_PRESSED = 1,
	STAND_STATE_IRON_PRESENT = 2,
	STAND_STATE_IRON_NOT_PRESENT = 3,
	TIP_CHANGE_STATE_IRON_PRESENT = 4,
	TIP_CHANGE_STATE_IRON_NOT_PRESENT = 5
};


// --- handlers ---
void hid__init(void);
void hid__poll(void);

// --- getters ---
uint8_t hid__get_button_state(void);
uint8_t hid__get_stand_state(void);
uint8_t hid__get_tip_change_state(void);

#endif /* INC_HID_H_ */
