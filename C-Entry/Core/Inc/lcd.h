/*
 * lcd.h
 *
 *  Created on: 15 June 2026
 *      Author: agathakomarudin
 */

#include <stdint.h>
#include "main.h"

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

// Common HD44780 Commands
#define LCD_CLEAR_DISPLAY   0x01
#define LCD_RETURN_HOME     0x02
#define LCD_ENTRY_MODE_SET  0x06
#define LCD_DISPLAY_ON      0x0C
#define LCD_DISPLAY_CONTROL 0x0F
#define LCD_SHIFT_LEFT      0x10
#define LCD_SHIFT_RIGHT     0x14
#define SET_DDRAM_ADDRESS   0x80
#define LCD_FIRST_LINE      0x00
#define LCD_SECOND_LINE     0x40

// Function prototypes
void LCD_PutNibble(uint8_t nibble);
void LCD_SendData(uint8_t c);
void LCD_SendCmd(uint8_t c);
void LCD_Pulse(void);
void LCD_SendStr(char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_init(void);

#endif /* SRC_LCD_H_ */
