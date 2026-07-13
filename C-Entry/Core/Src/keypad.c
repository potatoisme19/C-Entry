/*
 * keypad.c
 *
 *  Created on: 19 June 2026
 *      Author: agathakomarudin
 */

#include "main.h"

// initalise arrays
uint8_t key_map[4][4] = {{'1', '2', '3', 'A'},
					   {'4', '5', '6', 'B'},
					   {'7', '8', '9', 'C'},
					   {'*', '0', '#', 'D'}};
uint16_t colpin[4] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11};
uint16_t rowpin[4] = {GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14};


uint8_t scan_keypad() {
	uint8_t c = 0;

	for (int col = 0; col < 4; col++) {
		HAL_GPIO_WritePin(GPIOA, colpin[col], 1);
		for (int row = 0; row < 4; row++) {
			if (HAL_GPIO_ReadPin(GPIOB, rowpin[row]) == 1) {
				// debounce
				HAL_Delay(20);
				if (HAL_GPIO_ReadPin(GPIOB, rowpin[row]) == 1) {
					c = key_map[row][col];
				}
				// stops until button released
				while (HAL_GPIO_ReadPin(GPIOB, rowpin[row]) == 1) {}
			}
		}
		HAL_GPIO_WritePin(GPIOA, colpin[col], 0);
	}
	return c;
}
