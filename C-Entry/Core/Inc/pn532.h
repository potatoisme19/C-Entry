/*
 * pn532.h
 *
 *  Created on: 13 July 2026
 *      Author: agathakomarudin
 */

#ifndef INC_PN532_H_
#define INC_PN532_H_

#include "main.h"  // gives access to hi2c1, HAL types, etc.

uint8_t PN532_SamConfig(void);
uint8_t PN532_ReadPassiveTarget(uint8_t *uid);
void UART_Print(char *msg);
void UART_PrintUID(uint8_t *uid, uint8_t len);

#endif /* INC_PN532_H_ */
