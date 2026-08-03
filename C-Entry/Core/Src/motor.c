/*
 * motor.c
 *
 *  Created on: 20 July 2026
 *      Author: agathakomarudin
 */

#include "main.h"

void WaveStep(int speed) {
	HAL_Delay(speed);
	// step 1
	// set them all to zero excpet B
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 2
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 3
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);

	HAL_Delay(speed);

	// step 4
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);
}

void FullStep(int speed) {
	HAL_Delay(speed);
	// step 1
	// set them all to zero excpet B
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 2
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 3
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);

	HAL_Delay(speed);

	// step 4
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);
}

void HalfStep(int speed) {
	HAL_Delay(speed);
	// step 1
	// set them all to zero excpet B
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 2
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 3
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 4
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);

	HAL_Delay(speed);

	// step 5
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);

	HAL_Delay(speed);

	// step 6
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);

	HAL_Delay(speed);

	// step 7
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);

	HAL_Delay(speed);

	// step 8
	HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
	HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
	HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
	HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);
}

/**
 * @brief Rotates the stepper motor by a set number of full steps.
 * @param steps: Number of steps
 * @param delay_ms: Speed control delay between steps
 */
void stepper_rotate_steps(uint16_t steps, uint8_t delay_ms) {
	for (int i = 0; i < steps; i++) {

		int step = i % 4;

		if (step == 0) {
			//step 1
			HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
			HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);
			HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
			HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
			HAL_Delay(delay_ms);
		} else if (step == 1) {
			//step 2
			HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 1);
			HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);
			HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 0);
			HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
			HAL_Delay(delay_ms);
		} else if (step == 2) {
			//step 3
			HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
			HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 1);
			HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
			HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 0);
			HAL_Delay(delay_ms);
		} else if (step == 3) {
			//step 4
			HAL_GPIO_WritePin(COILB_GPIO_Port, COILB_Pin, 0);
			HAL_GPIO_WritePin(COILD_GPIO_Port, COILD_Pin, 0);
			HAL_GPIO_WritePin(COILA_GPIO_Port, COILA_Pin, 1);
			HAL_GPIO_WritePin(COILC_GPIO_Port, COILC_Pin, 1);
			HAL_Delay(delay_ms);
		}
	}
}
