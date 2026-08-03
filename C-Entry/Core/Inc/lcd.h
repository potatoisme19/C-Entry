/*
 * lcd.h
 *
 *  Created on: 15 June 2026
 *      Author: agathakomarudin
 */

#include <stdint.h>

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

// Function prototypes
void LCD_PutNibble(uint8_t nibble);
void LCD_SendData(uint8_t c);
void LCD_SendCmd(uint8_t c);
void LCD_Pulse();
void LCD_SendStr(char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_init();

#endif /* SRC_LCD_H_ */
