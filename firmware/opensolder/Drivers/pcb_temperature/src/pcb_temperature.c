/**
 * pcb_temperature.c
 *
 * This lib implements pcb temperature related functionalities
 */

#include "../inc/pcb_temperature.h"

int16_t read_pcb_temperature(void) {
	int16_t temp_register = 0;
	uint8_t tmp_buffer[2];

	if (HAL_I2C_Master_Receive(&hi2c1, PCT2075_I2C_ADDR, tmp_buffer, sizeof(tmp_buffer), 100) != HAL_ERROR) {
		temp_register = (tmp_buffer[0] << 8);
		temp_register |= tmp_buffer[1];
		return ((temp_register >> 5) * 0.125); // Returns 11 bit signed (2s compl.) temperature in Celsius.
	} else {
		return ADC_READING_ERROR;
	}
}
