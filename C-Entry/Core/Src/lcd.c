/*
 * lcd.c
 *
 *  Created on: 15 June 2026
 *      Author: agathakomarudin
 */

#include "lcd.h"

#define LCD_CLEAR_DISPLAY 0b00000001

void LCD_PutNibble(uint8_t nibble) {
	uint8_t D4 = nibble & 1;
	uint8_t D5 = (nibble >> 1) & 1;
	uint8_t D6 = (nibble >> 2) & 1;
	uint8_t D7 = (nibble >> 3) & 1;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, D4);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, D5);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, D6);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, D7);
}

void LCD_SendData(uint8_t c) {
	// RS = 1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	LCD_PutNibble(c >> 4);
	LCD_Pulse();
	LCD_PutNibble(c & 0x0F);
	LCD_Pulse();
	// RS = 0
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
}

void LCD_SendCmd(uint8_t c) {
	// RS = 0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	LCD_PutNibble(c >> 4);
	LCD_Pulse();
	LCD_PutNibble(c & 0x0F);
	LCD_Pulse();
}

void LCD_Pulse() {
	// E = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_Delay(1);
	// E = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void LCD_SendStr(char *str) {

	int count = 0;
	char c = str[count];
	while (c != '\0') {
	  LCD_SendData(c);
	  count++;
	  c = str[count];
	}

}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    // Calculate DDRAM address based on row and column
        if (row == 0) {
            address = SET_DDRAM_ADDRESS + LCD_FIRST_LINE + col; // Row 0 starts at DDRAM address 0x00 (0x80 command mask)
        } else {
            address = SET_DDRAM_ADDRESS + LCD_SECOND_LINE + col; // Row 1 starts at DDRAM address 0x40 (0x80 command mask + 0x40)
        }

        // Send the address command to the LCD
        LCD_SendCmd(address);
}

void LCD_init() {
	 // 1. wait for enough time to stabilise
	 HAL_Delay(50);
	 // 2. send command 0011 (function set) and wait for >=4.1 ms (enough wait inside the pulse)
	 LCD_PutNibble(0b0011);
	 LCD_Pulse();
	 HAL_Delay(5);

	 // 3. send command 0011 (function set) again and wait for >=100 us
	 LCD_PutNibble(0b0011);
	 LCD_Pulse();
	 HAL_Delay(1);

	 // 4. send command 0011 (function set) again
	 LCD_PutNibble(0b0011);
	 LCD_Pulse();

	 // 5. send command 0010 to set to 4-bit bus mode
	 LCD_PutNibble(0b0010);
	 LCD_Pulse();

	 // 6. send command 0010 1100 (function set: 4-bit mode, 2-lines, 5x8 font)
	 LCD_SendCmd(0b00101100);

	 // 7. Send command 0000 1000 to display ON/OFF
	 LCD_SendCmd(0b00001000);

	 // 8. Send command to clear the display
	 LCD_SendCmd(LCD_CLEAR_DISPLAY);

	 // 9. Send command set entry mode (increment cursor, no display shift)
	 LCD_SendCmd(0b00000110);

	 // 10. send command 0000 1111 to display on, cursor on, blink on
	 LCD_SendCmd(0b00001111);
}
