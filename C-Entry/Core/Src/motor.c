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
