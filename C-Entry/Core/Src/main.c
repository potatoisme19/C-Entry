/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "keypad.h"
#include "lcd.h"
#include "motor.h"
#include "pn532.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "keypadPassword.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LCD_CLEAR_DISPLAY 0b00000001
#define LCD_SHIFT_LEFT 0b00010000
#define MAX_CHAR_IN_LINE 16
#define LCD_FIRST_LINE 0x00
#define LCD_SECOND_LINE 0x40
#define SET_DDRAM_ADDRESS 0b10000000
#define RFID_UID 0x765F3207
#define NFC_UID 0x3E610B07
#define KEYPAD_TIMEOUT 600000 // 600 seconds, 10 minutes
// #define KEYPAD_TIMEOUT 6000 // 6 seconds for testing
#define MAX_DIGITS 16

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

enum STATES {
	IDLE, EMERGENCY, OPEN, ALARM, EVENT, KEYPAD
};
// Sets the initial state of the gate
int STATE = IDLE;
int previousState = IDLE;

//0 = gate closed, 1 = Gate open
int isGateOpen = 0;

// counts how many milliseconds since start of program
volatile int milliseconds = 0;

// tracks time until keypad timeout
int keypadStartTimeout = 0;
// time that event state should last for
int eventTime = 0;
int eventStartTime = 0;
// for keypad input tracking
char timeRecorded[MAX_DIGITS] = { '\0' };
int timeDigitNum = 0;
// for scheduling
int scheduledMinutes = 0;
int keypadSchedulePrint = 0;
int forward = 1;
int currentLCDBit = 0;

// for event
int eventFirstStart = 0;
int lastPrinted = 0;

// for alarm
int alarmFirstStart = 0;
int alarmLedState = 0;
int alarmCounter = 0;

//emergency
// to clear LCD when emergency is called
int emergencyStateChange = 0;
int emergencyBlinkCounter = 0;
int emergencyBlinkState = 0;

// for open state
int openStateChange = 0;
int openTimerStart = 0;
int leaving = 0;

// for idle
int idleTimerStart = 0;
int idleStart = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM6_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

int scheduleChange(int scheduledMinutes, int forward, int value);
bool checkPassword ();
void shift_register_write(uint16_t data);
void clearPeripherals();
void Buzzer_Tone(uint32_t frequency_hz);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	// test code to check NFC module
//  for (uint8_t addr = 1; addr < 128; addr++) {
//      if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK) {
//          char msg[32];
//          sprintf(msg, "Found device at 0x%02X\r\n", addr);
//          UART_Print(msg);
//      }
//  }
// start lcd
	LCD_init();
	// start timer 6
	HAL_TIM_Base_Start_IT(&htim6);

	// start NFC configuration and check if it works
	HAL_Delay(100);
	PN532_SamConfig();

	// test code
//  HAL_Delay(100);
//  if (PN532_SamConfig()) {
//      UART_Print("PN532 SAM Config OK\r\n");
//  } else {
//      UART_Print("PN532 SAM Config FAILED\r\n");
//  }
	// to check if NFC reader works
	uint8_t uid[7];
	uint8_t uidLen;

	// callibrate ADC
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		// commented code is just for testing, it can make keypad unreliable
		// for testing, check current state
		char stateMsg[32];
		sprintf(stateMsg, "STATE: %d\r\n", STATE);
		UART_Print(stateMsg);

		// check if NFC reader works
		//	  uidLen = PN532_ReadPassiveTarget(uid);
		//	  if (uidLen > 0) {
		//		  UART_PrintUID(uid, uidLen);
		//	  }
		//	  HAL_Delay(1);
		//	  char m[100];
		//	  snprintf(m, sizeof(m), "Milliseconds since program start: %d\r\n", milliseconds);
		//	  UART_Print(m);

//		if (STATE == IDLE) {
//			// clear all peripherals
//			clearPeripherals();
//
//			// get uid
//			uidLen = PN532_ReadPassiveTarget(uid);
//
//			// check if success
//			if (uidLen > 0) {
//			  uint32_t scannedUID = (uid[0] << 24) | (uid[1] << 16) | (uid[2] << 8) | uid[3];
//			  // from IDLE to KEYPAD_STATE
//			  if (RFID_UID == scannedUID && uidLen > 0 && STATE != EMERGENCY) {
//				  if (checkPassword()) {
//					  // reset eventStartTime
//					  eventStartTime = 0;
//					  STATE = KEYPAD;
//					  keypadSchedulePrint = 1;
//					  keypadStartTimeout = milliseconds;
//				  }
//			  }
//			}
//
//			// turns to event after scheduled time
//			// if eventStartTime == 0 then its not initialised
//			if (milliseconds >= eventStartTime && eventStartTime != 0 && STATE != EMERGENCY) {
//			  STATE = EVENT;
//			}
//
//		}

		if (STATE == IDLE) {
			if (idleStart == 1) {
				// clear all peripherals
				clearPeripherals();
				LCD_SendStr("IDLE");
				idleStart = 0;
				idleTimerStart = milliseconds;
				// if gate open, close it
				if (isGateOpen && leaving == 0) {
					for (int i = 0; i < 128; i++) {
						FullStep(5);
					}
					isGateOpen = 0;
				}

				if (isGateOpen && leaving == 1) {
					for(int i = 0; i < 128; i++) {
						FullStepReverse(5);
					}
					isGateOpen = 0;
				}
			}

			// get uid
			uidLen = PN532_ReadPassiveTarget(uid);

			// check if success
			if (uidLen > 0) {
				uint32_t scannedUID = (uid[0] << 24) | (uid[1] << 16)
						| (uid[2] << 8) | uid[3];

				// from IDLE to KEYPAD_STATE
				if (RFID_UID == scannedUID && uidLen > 0
						&& STATE != EMERGENCY) {
					// move to second line before checking password
					LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);
					if (checkPassword()) {
						// reset eventStartTime
						eventStartTime = 0;
						STATE = KEYPAD;
						keypadSchedulePrint = 1;
						keypadStartTimeout = milliseconds;
					}
				} else if (NFC_UID == scannedUID && uidLen > 0
							&& STATE != EMERGENCY) {

					UART_Print("HELLOWOLRD");
					HAL_Delay(1000);
						// play correct buzzer tone
						int rest = 0;
						int tune1[] = { 622, rest, 932, 784, 784, 698, 622, rest,
								932, 784, 784, 698, 740, 784, rest, 932, 784, 784,
								698, 622, 523, 622, 698, 932, 523, 622, 698, 622 };
						int i = 0;
						int lastNoteTime = 0;
						int numNotes = sizeof(tune1) / sizeof(tune1[0]);
						while (i < numNotes) {
						    if (STATE == EMERGENCY) {
						        break;
						    }
						    if (milliseconds - lastNoteTime >= 25) {
						        Buzzer_Tone(tune1[i]);
						        lastNoteTime = milliseconds;
						        i++;
						    }
						}
						// change state to OPEN
						STATE = OPEN;
						openStateChange = 1;
				} else {
					// play incorrect buzzer tone
					int tune1[] = {440, 550, 440, 550, 440, 550, 440, 550, 440,
								550, 400, 300, 255, 255, 300, 303};

					int i = 0;
					int lastNoteTime = 0;
					int numNotes = sizeof(tune1) / sizeof(tune1[0]);
					while (i < numNotes) {
						if (STATE == EMERGENCY) {
							break;
						}

						// only play and increment if time between is 25 milliseconds
						if (milliseconds - lastNoteTime >= 25) {
							Buzzer_Tone(tune1[i]);
							lastNoteTime = milliseconds;
							i++;
						}
					}
					Buzzer_Tone(0);
				}

			}

			// detect if someone is leaving the building
			HAL_ADC_Start(&hadc3);
			// wait until finished converting
			HAL_ADC_PollForConversion(&hadc3, 100);
			int val = HAL_ADC_GetValue(&hadc3);
			// debounce, 500 is the threshold
			if (val > 500) {
				idleTimerStart = milliseconds;
			} else {
				// check for 500 milliseconds
				// person needs to wait half a second for gate to start opening
				if (milliseconds - idleTimerStart > 50) {
					// exiting detected
					STATE = OPEN;
					openStateChange = 1;
					leaving = 1;
				}

			}

			// turns to event after scheduled time
			// if eventStartTime == 0 then its not initialised
			if (milliseconds >= eventStartTime && eventStartTime != 0
					&& STATE != EMERGENCY && STATE != OPEN) {
				STATE = EVENT;
			}

		}

		if (STATE == OPEN) {
			if (openStateChange == 1) {
				clearPeripherals();
				LCD_SendStr("DOOR OPENING!");
				shift_register_write(0xFFFF);
				// rotate 90 degrees
				stepper_rotate_steps(512, 5);
				isGateOpen = 1;
				openTimerStart = milliseconds;
				openStateChange = 0;
			}

			// detect LDR1, then after 10 seconds, close
			HAL_ADC_Start(&hadc2);
			// wait until finished converting
			HAL_ADC_PollForConversion(&hadc2, 100);
			int val1 = HAL_ADC_GetValue(&hadc2);
			if (val1 < 500) {
				// give 10 second timeout
				int timeout = 1000 * 60; // 1 minute timeout
				int timer = milliseconds;
				while (milliseconds - timer < timeout && STATE != EMERGENCY) {
					// if LDR2 is detected,
					HAL_ADC_Start(&hadc2);
					// wait until finished converting
					HAL_ADC_PollForConversion(&hadc2, 100);
					int val2 = 0;
					val2 = HAL_ADC_GetValue(&hadc3);
					if (val2 < 500) {
						// they left already
						STATE = IDLE;
					}
//					// check success
//					if (uidLen > 0) {
//						uint32_t scannedUID = (uid[0] << 24) | (uid[1] << 16) | (uid[2] << 8) | uid[3];
//						if (RFID_UID == scannedUID && uidLen > 0) {
//							if (checkPassword()) {
//								STATE = IDLE;
//								eventTime = 0;
//								eventStartTime = 0;
//								if (isGateOpen) {
//									// close gates
//									for (int i = 0; i < 128; i++) {
//										// goes anticlockwise
//										FullStep(5);
//									}
//									isGateOpen = 0;
//								}
//								// only break while loop if password correct
//								// otherwise keep looping until timeout
//								break;
//							}
//						}
//					}
				}

			}


			// detect tailgating
			// start ADC, LDR 2
			HAL_ADC_Start(&hadc3);
			// wait until finished converting
			HAL_ADC_PollForConversion(&hadc3, 100);
			int val = HAL_ADC_GetValue(&hadc3);
			// set threshold to be 500
			if (val > 500) {
				// still triggered, reset timer,
				openTimerStart = milliseconds;
			} else {
				// check for 5 seconds if its still greater than 500
				// i.e check that person isnt near the gate
				// check every half a second
				if (milliseconds - openTimerStart > 5000) {
					// assign alarmFirstStart = 1, when tailgate detected
					STATE = ALARM;
					alarmFirstStart = 1;
				}
			}

		}

		if (STATE == ALARM) {
			if (alarmFirstStart == 1) {
				// print ALARM on the LCD
				LCD_SendCmd(LCD_CLEAR_DISPLAY);
				char alarmStr[20];
				snprintf(alarmStr, sizeof(alarmStr), "ALARM");
				LCD_SendStr(alarmStr);

				int timerStart = milliseconds;
				// loops until it sees that no one has triggered LDR for over 2 seconds
				while (1) {
					// start ADC, LDR 2
					HAL_ADC_Start(&hadc3);
					// wait until finished converting
					HAL_ADC_PollForConversion(&hadc3, 100);
					int val = HAL_ADC_GetValue(&hadc3);
					// set threshold to be 500
					if (val < 500) {
						// still triggered, reset timer,
						timerStart = milliseconds;
					} else {
						// check for 2 seconds if its still greater than 500
						// i.e check that person isnt near the gate
						// check every half a second
						if (milliseconds - timerStart > 2000) {
							break;
						}
					}
				}
				// then starts to close gate
				// motor rotates 90 degrees
				for (int i = 0;i < 128; i++) {
					FullStepReverse(5);
				}
				isGateOpen = 0;
				alarmFirstStart = 0;
			}

			// to exit ALARM state
			if (HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin) == 1) {
				HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
				int timeout = 1000 * 60; // 1 minute timeout
				// scans for RFID
				int timer = milliseconds;
				while (milliseconds - timer < timeout && STATE != EMERGENCY) {
					 // get uid
					 uidLen = PN532_ReadPassiveTarget(uid);

					 // check if success
					if (uidLen > 0) {
						uint32_t scannedUID = (uid[0] << 24) | (uid[1] << 16) | (uid[2] << 8) | uid[3];
						// from ALARM to IDLE
						if (RFID_UID == scannedUID && uidLen > 0) {
							if (checkPassword()) {
								STATE = IDLE;
								// rotate anticlockwise
								for (int i = 0; i < 128; i++) {
									FullStep(5);
								}
								isGateOpen = 0;
								HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
								break;
							}
						}
					}
				}
			}

			// flashing lights and buzzer
			// flash shift registers
			// D1 - D4
			if (STATE == ALARM && milliseconds - alarmCounter > 500) {
			    alarmLedState = !alarmLedState;
			    if (alarmLedState) {
			        shift_register_write(0xFFFF);
		            Buzzer_Tone(650);
			    } else {
			        shift_register_write(0x0000);
		            Buzzer_Tone(1200);
			    }

				// D1 - D4
		    	HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, alarmLedState);
		    	HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin, alarmLedState);
		    	HAL_GPIO_WritePin(LED_D3_GPIO_Port, LED_D3_Pin, alarmLedState);
		    	HAL_GPIO_WritePin(LED_D4_GPIO_Port, LED_D4_Pin, alarmLedState);
			    alarmCounter = milliseconds;
			}

		}

		if (STATE == EMERGENCY) {
			if (emergencyStateChange) {
				clearPeripherals();
				LCD_SendCmd(LCD_CLEAR_DISPLAY);
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_FIRST_LINE);
				LCD_SendStr("!! EMERGENCY!!");
				emergencyStateChange = 0;
				// if it changed to emergency in the middle of keypad input
				// clear temporary keypad buffer
				for (int i = 0; i < timeDigitNum; i++) {
					timeRecorded[i] = '\0';
				}
				timeDigitNum = 0;
			}

			// open gate
			if (!isGateOpen) {
				stepper_rotate_steps(512, 3);
				isGateOpen = 1;
			}

			// beep and blinking shift register 0.25s
			if (milliseconds - emergencyBlinkCounter > 250) {
				emergencyBlinkState = !emergencyBlinkState;
				emergencyBlinkCounter = milliseconds;

				if (emergencyBlinkState) {
					shift_register_write(0xFFFF);
					Buzzer_Tone(1000); // Beep at 1000 Hz
				} else {
					shift_register_write(0x0000);
					Buzzer_Tone(0);      // Turn off PWM
				}
			}

			// get uid
			uidLen = PN532_ReadPassiveTarget(uid);

			// check if success
			if (uidLen > 0) {
				uint32_t scannedUID = (uid[0] << 24) | (uid[1] << 16)
						| (uid[2] << 8) | uid[3];
				// from EMERGENCY to IDLE
				if (RFID_UID == scannedUID && uidLen > 0) {
					// turn off beep and shift register
					Buzzer_Tone(0);
					shift_register_write(0x0000);

					if (checkPassword()) {
						// Password correct: Turn off alarm outputs
						HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

						// Case 1: Returning to IDLE or KEYPAD
						if (previousState == IDLE || previousState == KEYPAD) {
							if (isGateOpen) {
								for (int i = 0; i < 128; i++) {
									FullStep(5);
								}
								isGateOpen = 0;
							}
							STATE = IDLE;
						}
						// Case 2: Returning to EVENT
						else if (previousState == EVENT) {
							// Check if event has not expired yet
							if ((milliseconds - eventStartTime) < eventTime) {
								STATE = EVENT;
								eventFirstStart = 1; // Redraw LCD event screen
								// Gate stays open! (isGateOpen remains 1)
							} else {
								// Event expired while in emergency
								if (isGateOpen) {
									for (int i = 0; i < 128; i++) {
										FullStep(5);
									}
									isGateOpen = 0;
								}
								STATE = IDLE;
							}
						}
						// Fallback safety (e.g. if previous state was ALARM)
						else {
							STATE = IDLE;
						}
					} else {
						// Password failed/cancelled - re-flag emergency display update if needed
						emergencyStateChange = 1;
					}
				}
			}
		}

		if (STATE == EVENT) {
			// timer on how long the event is going to go on for in first line
			// timer when event is starting on second line (if its now set 'now')
			int elapsedEventTime = milliseconds - eventStartTime;
			int remainingEventSec = (eventTime - elapsedEventTime) / 1000;

			if (remainingEventSec < 0) {
				remainingEventSec = 0;
			}

			int eventMins = remainingEventSec / 60;
			int eventSecs = remainingEventSec % 60;

			// how long the event is going to go on
			char line2Str[17];
			snprintf(line2Str, sizeof(line2Str), "Dur: %02dm %02ds", eventMins,
					eventSecs);

			if (eventFirstStart == 1) {
				LCD_SendCmd(LCD_CLEAR_DISPLAY);
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_FIRST_LINE);
				char line1Str[17];
				snprintf(line1Str, sizeof(line1Str), "EVENT");
				LCD_SendStr(line1Str);
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);
				char rotatingStr[17];
				snprintf(rotatingStr, sizeof(rotatingStr), "ROTATING");
				LCD_SendStr(rotatingStr);

				if (!isGateOpen) {
					stepper_rotate_steps(512, 5);
					isGateOpen = 1;
				}
				eventFirstStart = 0;
			}

			if (milliseconds - lastPrinted >= 1000 && eventFirstStart == 0) {
				//update the LCD
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);
				LCD_SendStr("                ");
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);
				LCD_SendStr(line2Str);
				lastPrinted = milliseconds;
			}
				
			shift_register_write(0xAAAA);
			HAL_Delay(1);

//			if (eventFirstStart == 1) {
//				// open all doors
//				// rotate by 90 degrees (512 full steps)
//				// goes clockwise
//				if (!isGateOpen) {
//					stepper_rotate_steps(512, 5);
//					isGateOpen = 1;
//				}
//				eventFirstStart = 0;
//			}

			if (HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin) == 1) {
				LCD_SendCmd(LCD_CLEAR_DISPLAY);
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_FIRST_LINE);
				char exitStr[17];
				snprintf(exitStr, sizeof(exitStr), "EXITING");
				LCD_SendStr(exitStr);
				// if we want to end EVENT state early, press SW4 button
				// we do this since getting uid will make LCD print slower
				int timeout = 1000 * 60; // 1 minute timeout
				// scans for RFID
				int timer = milliseconds;
				while (milliseconds - timer < timeout && STATE != EMERGENCY) {
					// get uid
					uidLen = PN532_ReadPassiveTarget(uid);

					// check success
					if (uidLen > 0) {
						uint32_t scannedUID = (uid[0] << 24) | (uid[1] << 16) | (uid[2] << 8) | uid[3];
						if (RFID_UID == scannedUID && uidLen > 0) {
							if (checkPassword()) {
								STATE = IDLE;
								eventTime = 0;
								eventStartTime = 0;
								if (isGateOpen) {
									// close gates
									for (int i = 0; i < 128; i++) {
										// goes anticlockwise
										FullStep(5);
									}
									isGateOpen = 0;
								}
								// only break while loop if password correct
								// otherwise keep looping until timeout
								break;
							}
						}
					}
				}
			}
			// check event time, goes to IDLE state if event time is more than assigned time
			if ((milliseconds - eventStartTime) > eventTime && STATE != EMERGENCY) {
				STATE = IDLE;
				eventTime = 0;
				eventStartTime = 0;
			}
			
		}

		if (STATE == KEYPAD) {
			if (keypadSchedulePrint == 1) {
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);
				// clear only second line
				LCD_SendStr("                ");
				// time scheduled
				char scheduledTime[] = "S: ";
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);
				LCD_SendStr(scheduledTime);
				char hrs[20];
				snprintf(hrs, sizeof(hrs), "%d", scheduledMinutes / 60);
				LCD_SendStr(hrs);
				LCD_SendStr("hrs ");
				char mins[20];
				snprintf(mins, sizeof(mins), "%d", scheduledMinutes % 60);
				LCD_SendStr(mins);
				LCD_SendStr("min");

				keypadSchedulePrint = 0;
				// t for time period
				LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_FIRST_LINE);
				LCD_SendStr("T: ");
				// 3 since T:
				currentLCDBit = 3 + timeDigitNum;
				LCD_SendCmd(SET_DDRAM_ADDRESS | currentLCDBit);
			}

			int c = 0;
			c = scan_keypad();
			if (c != 0) {
				if (c == '*') {
					// backspace
					if (timeDigitNum > 0) {
						LCD_SendCmd(LCD_SHIFT_LEFT);
						LCD_SendData(' ');
						LCD_SendCmd(LCD_SHIFT_LEFT);

						// reset last digit in array
						timeDigitNum--;
						timeRecorded[timeDigitNum] = '\0';
					}

				} else if (c == '#') {
					// enter
					if (timeDigitNum == 0) {
						STATE = IDLE;
						idleStart = 1;
					} else {
						eventFirstStart = 1;
						// time recorded is in minutes
						// e.g inputted 1 is 1 minute
						// minutes
						if (atoi(timeRecorded) == 0) {
							STATE = IDLE;
							idleStart = 1;
							for (int i = 0; i < timeDigitNum; i++) {
								timeRecorded[i] = '\0';
							}
							timeDigitNum = 0;
						} else {
							eventTime  = atoi(timeRecorded) * 1000 * 60;
							// seconds for testing
							// eventTime = atoi(timeRecorded) * 1000;
							// reset time recorded
							for (int i = 0; i < timeDigitNum; i++) {
								timeRecorded[i] = '\0';
							}
							timeDigitNum = 0;
							// clear lcd after enter
							LCD_SendCmd(LCD_CLEAR_DISPLAY);
							if (scheduledMinutes == 0) {
								// if theres no scheduled time we start instantly
								STATE = EVENT;
								eventStartTime = milliseconds;
							} else {
								STATE = IDLE;
								eventStartTime = milliseconds + (scheduledMinutes * 1000 * 60);
							}
						}
					}
				} else if (c == 'A') {
					// move event start time forward/backward by one hour
					scheduledMinutes = scheduleChange(scheduledMinutes, forward, 60);
					keypadSchedulePrint = 1;
				} else if (c == 'B') {
					// move event start time forward/backward by 10 minutes
					scheduledMinutes = scheduleChange(scheduledMinutes, forward, 10);
					keypadSchedulePrint = 1;
				} else if (c == 'C') {
					// move event start time forward/backward by 1 minute
					scheduledMinutes = scheduleChange(scheduledMinutes, forward, 1);
					keypadSchedulePrint = 1;
				} else if (c == 'D') {
					// change to forward/backward
					forward = !forward;
				} else {
					// add number to end of array
					timeRecorded[timeDigitNum] = c;
					timeDigitNum++;

					// print the letter
					LCD_SendData(c);
				}
			}
			int isTimeout = milliseconds - keypadStartTimeout;
			// goes back to IDLE state if timeout
			if (isTimeout > KEYPAD_TIMEOUT) {
				STATE = IDLE;
			}

		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_ADC12
                              |RCC_PERIPHCLK_ADC34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.Adc34ClockSelection = RCC_ADC34PLLCLK_DIV1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc3, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 71;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SRCLK_Pin|COILA_Pin|RW_Pin|COILC_Pin
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|COL1_Pin|COL2_Pin|COL3_Pin
                          |COL4_Pin|COILB_Pin|RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, COILD_Pin|LED_D1_Pin|SER_Pin|LED_D4_Pin
                          |LED_D2_Pin|LED_D3_Pin|RCLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SRCLK_Pin COILA_Pin RW_Pin COILC_Pin
                           D4_Pin D5_Pin D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = SRCLK_Pin|COILA_Pin|RW_Pin|COILC_Pin
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SW4_Pin */
  GPIO_InitStruct.Pin = SW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin COL1_Pin COL2_Pin COL3_Pin
                           COL4_Pin COILB_Pin RS_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|COL1_Pin|COL2_Pin|COL3_Pin
                          |COL4_Pin|COILB_Pin|RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SW3_Pin */
  GPIO_InitStruct.Pin = SW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : COILD_Pin LED_D1_Pin SER_Pin LED_D4_Pin
                           LED_D2_Pin LED_D3_Pin RCLK_Pin */
  GPIO_InitStruct.Pin = COILD_Pin|LED_D1_Pin|SER_Pin|LED_D4_Pin
                          |LED_D2_Pin|LED_D3_Pin|RCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ROW1_Pin ROW2_Pin ROW3_Pin ROW4_Pin */
  GPIO_InitStruct.Pin = ROW1_Pin|ROW2_Pin|ROW3_Pin|ROW4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : E_Pin */
  GPIO_InitStruct.Pin = E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(E_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

	/*Configure GPIO pins : PB6 PB9 (I2C1_SCL, I2C1_SDA) */
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == SW1_Pin) {
		if (STATE != EMERGENCY) {
			previousState = STATE;
		}
		emergencyStateChange = 1;
		STATE = EMERGENCY;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim6) {
		milliseconds++;
	}
}

// for scheduling
int scheduleChange(int scheduledMinutes, int forward, int value) {
	if (forward == 1) {
		scheduledMinutes = scheduledMinutes + value;
	} else {
		if (scheduledMinutes >= 0) {
			int temp = scheduledMinutes - value;
			if (temp >= 0) {
				scheduledMinutes = scheduledMinutes - value;
			}
		}
	}
	return scheduledMinutes;
}


/**
 * @brief Sends a 16-bit pattern to the shifted LED registers.
 * @param data: 16-bit integer where each bit represents an LED (1 = ON, 0 = OFF)
 */
void shift_register_write(uint16_t data) {
	// Repeat for all 16 bits (MSB first)
	for (int i = 15; i >= 0; i--) {
		// Step 2: Read bit i and set SER pin
		if ((data >> i) & 1) {
			HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, 1);   // SER HIGH
		} else {
			HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, 0); // SER LOW
		}

		// Step 3: Pulse SRCLK (Serial Clock) HIGH then LOW
		HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, 1);
		HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, 0);
	}

	// Step 5: Pulse RCLK (Latch Clock) HIGH then LOW to output all bits
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, 1);
	HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, 0);
}

bool checkPassword() {
	// setting own password
	char password[] = "admin";
	char inputtedPassword[20];
	int inputtedPasswordCount = 0;
	int doneTyping = 0;

	emergencyStateChange = 0;
	LCD_SendCmd(SET_DDRAM_ADDRESS | LCD_SECOND_LINE);


	// timeout is 10 mins
	int timeout = 1000 * 60;
	int timerStart = milliseconds;
	while (((milliseconds - timerStart) < timeout) && (doneTyping == 0)) {
		if (emergencyStateChange == 1) {
			LCD_SendCmd(LCD_CLEAR_DISPLAY);
			return false;
		}
		int c = 0;
		c = scanKeypadPassword();
		if (c != 0) {
			// backspace
			if (c == '*') {
				//backspace on LCD
				LCD_SendCmd(LCD_SHIFT_LEFT);
				HAL_Delay(1);
				LCD_SendData(' ');
				HAL_Delay(1);
				LCD_SendCmd(LCD_SHIFT_LEFT);
				inputtedPassword[inputtedPasswordCount] = '\0';
				inputtedPasswordCount--;
			} else if (c == '#') {
				inputtedPassword[inputtedPasswordCount] = '\0';
				doneTyping = 1;
				LCD_SendCmd(LCD_CLEAR_DISPLAY);
			} else {
				LCD_SendData(c);
				inputtedPassword[inputtedPasswordCount++] = c;
			}
		}
	}

	// clear it if timer ends
	LCD_SendCmd(LCD_CLEAR_DISPLAY);
	if (strcmp(password, inputtedPassword) == 0) {
		return true;
	}
	return false;
}

void clearPeripherals() {
	alarmLedState = 0;
	LCD_SendCmd(LCD_CLEAR_DISPLAY);
	shift_register_write(0x0000);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
	HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, alarmLedState);
	HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin, alarmLedState);
	HAL_GPIO_WritePin(LED_D3_GPIO_Port, LED_D3_Pin, alarmLedState);
	HAL_GPIO_WritePin(LED_D4_GPIO_Port, LED_D4_Pin, alarmLedState);
}


void Buzzer_Tone(uint32_t frequency_hz) {
    if (frequency_hz == 0) {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
        return;
    }

    // With Prescaler = 71, Timer Clock = 1 MHz (1,000,000 Hz)
    uint32_t arr = (1000000 / frequency_hz) - 1;

    TIM1->ARR = arr;
    TIM1->CCR3 = (arr + 1) / 2; // 50% Duty Cycle

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
