/*
 * keypad.c
 *
 *  Created on: 19 June 2026
 *      Author: agathakomarudin
 */

#include "main.h"

int key_pressed_again(int row, int col);

// initalise arrays
uint8_t key_map1[4][4] = {
	{'a', 'd', 'g', '.'},
	{'j', 'm', 'p', ','},
	{'s', 'v', 'y', '!'},
	{'*', ' ', '#', '?'}};
uint8_t key_map2[4][4] = {
	{'b', 'e', 'h', '.'},
	{'k', 'n', 'q', ','},
	{'t', 'w', 'z', '!'},
	{'*', ' ', '#', '?'}};
uint8_t key_map3[4][4] = {
	{'c', 'f', 'i', '.'},
	{'l', 'o', 'r', ','},
	{'u', 'x', 'z', '!'},
	{'*', ' ', '#', '?'}};

static const uint16_t colpinP[4] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11};
static const uint16_t rowpinP[4] = {GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14};


uint8_t scanKeypadPassword() {
	int counter = 1;
	uint8_t c = 0;

	for (int col = 0; col < 4; col++) {
		HAL_GPIO_WritePin(GPIOA, colpinP[col], GPIO_PIN_SET);
		for (int row = 0; row < 4; row++) {
			if (HAL_GPIO_ReadPin(GPIOB, rowpinP[row]) == GPIO_PIN_SET) {
				// debounce
				HAL_Delay(20);
				if (HAL_GPIO_ReadPin(GPIOB, rowpinP[row]) == GPIO_PIN_SET) {
					// check to up to 3 presses
					for (int i = 0; i < 2; i++) {
						if (key_pressed_again(col, row)) {
						  // add to counter
						  counter++;
						} else {
							break;
						}

					}
					if (counter == 1) {
						c = key_map1[row][col];
					}
					if (counter == 2) {
						c = key_map2[row][col];
					}
					if (counter == 3) {
						c = key_map3[row][col];
					}
					counter = 1;
				}
				// Wait until button released with safety timeout
				uint32_t releaseTimeout = HAL_GetTick();
				while ((HAL_GPIO_ReadPin(GPIOB, rowpinP[row]) == GPIO_PIN_SET) &&
					(HAL_GetTick() - releaseTimeout < 500)) {
				}
			}
		}
		HAL_GPIO_WritePin(GPIOA, colpinP[col], GPIO_PIN_RESET);
	}
	return c;
}

int key_pressed_again(int col, int row) {
    // Wait for current press to release with safety timeout
    uint32_t releaseTimeout = HAL_GetTick();
    while ((HAL_GPIO_ReadPin(GPIOB, rowpinP[row]) == GPIO_PIN_SET) &&
           (HAL_GetTick() - releaseTimeout < 500)) {
    }

    // window of time to check for presses
    for (int i = 0; i < 10; i++) {
        HAL_Delay(20);
        // if we detect another press
        if (HAL_GPIO_ReadPin(GPIOB, rowpinP[row]) == GPIO_PIN_SET) {
            HAL_Delay(20); // debounce
            if (HAL_GPIO_ReadPin(GPIOB, rowpinP[row]) == GPIO_PIN_SET) {
                return 1; // add 1
            }
        }
    }
    return 0;
}
