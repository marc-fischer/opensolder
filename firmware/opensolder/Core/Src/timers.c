/*
 * timers.c
 *
 *	handling timers and interrupts
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 *
 * notes (originally from temperature.c:
 *  * - TIMELINE -
 * 1. Zero Cross Interrupt happens some µs before the "true" zero cross because
 *    of optocoupler hysteresis. ZC interrupt starts TIM6
 * 2. TIM6 interrupt happens at true ZC. Two options:
 * 		A - turn heater on:
 * 			- Set TIP_CLAMP as output, pull low (prevents noise on thermocouple amplifier input)
 * 			- Turn heater on
 * 		B - turn heater off:
 * 			- Turn heater off
 * 			- Start TIM7 (2ms timer)
 * 3. TIM7 interrupt
 * 		- Check if this is the first or second TIM7 interrupt. Two options:
 * 		A - First interrupt (2ms after ZC):
 * 			- Set TIP_CLAMP pin to input state (high impedance)
 * 			- Do a tip_state check if TIP_CHECK_INTERVAL has passed (checks if tip is inserted)
 * 		B - Second interrupt (4ms after ZC):
 * 			- Start the ADC reading
 */

#include "timers.h"

static volatile uint32_t ac_delay_tick_ms = 0;
static void zerocross_interrupt(uint16_t GPIO_Pin);

/******    Callback Functions    ******/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	zerocross_interrupt(GPIO_Pin);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {		// TODO: check race condition. What if both interrupts fire at the same time
	if (htim == &htim6) {	// TIM6 interrupt, indicating true AC zero cross. This is where to turn the heater on/off to avoid inductive spikes. Auto-Reload
		timer_stop_zerocross_timeout();
		heater_control_timer_handler();
	}
	if (htim == &htim7) {	// timer 7 handles a 2msec delay. Started after switching off the heater output. No Auto Reload
		tip_timer_handler();
	}
	if (htim == &htim14){	// timer14 handles a 2msec system tick to trigger
		hid__poll();
		tip_check_counter_increase();
	}
}

/******    ISR Functions    ******/
// ISR: Rising edge is detected on ZERO_CROSS pin. Start TIM6, which is a delay for when the true AC zero cross happens
static void zerocross_interrupt(uint16_t GPIO_Pin) {
	if (GPIO_Pin == ZERO_CROSS_Pin) {
		timer_start_zerocross_timeout();	// start the timeout until true zero cross
	}
}


// setters

void timer_stop_zerocross_timeout(void){
	// Timer6 = true zero cross (delay timer after zerocross interrupt)
	HAL_TIM_Base_Stop_IT(&htim6);	// stop the timer
}

// Timer6 = true zero cross (delay timer after zerocross interrupt)
void timer_start_zerocross_timeout(void){
	HAL_TIM_Base_Start_IT(&htim6);
	ac_delay_tick_ms = HAL_GetTick() + AC_DETECTION_INTERVAL_MS;
}

void timer_stop_tip_measure_delay(void){
	HAL_TIM_Base_Stop_IT(&htim7);
}
void timer_start_tip_measure_delay(void){
	HAL_TIM_Base_Stop_IT(&htim7);
}


// getters
uint32_t get_ac_delay_tick(void) {
	return ac_delay_tick_ms;
}


// handlers
void timer_init(void){
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim14);
}
