/*
 * motor.h
 *
 *  Created on: 20 July 2026
 *      Author: agathakomarudin
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

void WaveStep(int speed);
void FullStep(int speed);
void HalfStep(int speed);
void stepper_rotate_steps(uint16_t steps, uint8_t delay_ms);
void FullStepReverse(int speed);

#endif /* INC_MOTOR_H_ */
