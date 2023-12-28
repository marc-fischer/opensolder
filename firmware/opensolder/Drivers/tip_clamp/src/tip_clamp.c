/*
 * tip_clamp.c
 *
 *  Created on: Dec 28, 2023
 *      Author: marc
 */


#include "../inc/tip_clamp.h"

void tip_clamp_pin_enable(void){
	TIP_CLAMP_GPIO_Port->BRR |= GPIO_BRR_BR_1;		   // Set PA2 LOW
	TIP_CLAMP_GPIO_Port->MODER |= GPIO_MODER_MODER2_0; // Set PA2 to push pull output mode
}

void tip_clamp_pin_disable(void){
	TIP_CLAMP_GPIO_Port->MODER &= ~GPIO_MODER_MODER2_0; // Set PA2 to input mode
}
