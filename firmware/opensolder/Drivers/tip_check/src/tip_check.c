/*
 * tip_check.c
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */

#include "../inc/tip_check.h"

void tip_check_pin_enable(void){
	TIP_CHECK_GPIO_Port->BSRR |= GPIO_BSRR_BS_1;	   // Set PA1 HIGH
	TIP_CHECK_GPIO_Port->MODER |= GPIO_MODER_MODER1_0; // Set PA1 to push pull output mode
}

void tip_check_pin_disable(void){
	TIP_CHECK_GPIO_Port->MODER &= ~GPIO_MODER_MODER1_0; // Set TIP_CHECK pin PA1 to input mode
}
