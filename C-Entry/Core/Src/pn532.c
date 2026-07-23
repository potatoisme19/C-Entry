/*
 * pn532.c
 *
 *  Created on: 13 July 2026
 *      Author: agathakomarudin
 */


#include "pn532.h"
#include <string.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

#define PN532_I2C_ADDR       (0x24 << 1)  // 8-bit address for HAL
#define PN532_PREAMBLE       0x00
#define PN532_STARTCODE1     0x00
#define PN532_STARTCODE2     0xFF
#define PN532_POSTAMBLE      0x00
#define PN532_HOSTTOPN532    0xD4
#define PN532_PN532TOHOST    0xD5

#define PN532_COMMAND_GETFIRMWAREVERSION  0x02
#define PN532_COMMAND_SAMCONFIGURATION    0x14
#define PN532_COMMAND_INLISTPASSIVETARGET 0x4A

static HAL_StatusTypeDef PN532_ReadAck(void);

// Send a command frame to PN532
static HAL_StatusTypeDef PN532_WriteCommand(uint8_t *cmd, uint8_t cmdLen)
{
    uint8_t buf[64];
    uint8_t checksum;
    uint8_t idx = 0;

    buf[idx++] = PN532_PREAMBLE;
    buf[idx++] = PN532_STARTCODE1;
    buf[idx++] = PN532_STARTCODE2;

    uint8_t length = cmdLen + 1; // +1 for TFI byte
    buf[idx++] = length;
    buf[idx++] = ~length + 1; // LCS (length checksum)

    buf[idx++] = PN532_HOSTTOPN532;
    checksum = PN532_HOSTTOPN532;

    for (uint8_t i = 0; i < cmdLen; i++) {
        buf[idx++] = cmd[i];
        checksum += cmd[i];
    }

    buf[idx++] = ~checksum + 1; // DCS (data checksum)
    buf[idx++] = PN532_POSTAMBLE;

    return HAL_I2C_Master_Transmit(&hi2c1, PN532_I2C_ADDR, buf, idx, 100);
}

// Check if PN532 is ready to respond
static uint8_t PN532_IsReady(void)
{
    uint8_t status;
    HAL_I2C_Master_Receive(&hi2c1, PN532_I2C_ADDR, &status, 1, 100);
    return (status == 0x01);
}

static HAL_StatusTypeDef PN532_WaitReady(uint32_t timeout)
{
    uint32_t start = HAL_GetTick();
    while (!PN532_IsReady()) {
        if ((HAL_GetTick() - start) > timeout) return HAL_TIMEOUT;
        HAL_Delay(2);
    }
    return HAL_OK;
}

// Read response frame into buf, returns number of data bytes (after TFI)
static int16_t PN532_ReadResponse(uint8_t *buf, uint8_t bufLen)
{
    uint8_t rxBuf[64];

    if (PN532_WaitReady(1000) != HAL_OK) return -1;

    HAL_I2C_Master_Receive(&hi2c1, PN532_I2C_ADDR, rxBuf, bufLen + 8, 100);

    if (rxBuf[1] != 0x00 || rxBuf[2] != 0x00 || rxBuf[3] != 0xFF) return -2;

    uint8_t length = rxBuf[4];

    if (rxBuf[6] != PN532_PN532TOHOST) return -3;

    uint8_t dataLen = length - 1; // minus TFI byte
    memcpy(buf, &rxBuf[7], dataLen); // FIXED: data starts at index 7, not 8

    return dataLen;
}

uint8_t PN532_SamConfig(void)
{
    uint8_t cmd[] = {PN532_COMMAND_SAMCONFIGURATION, 0x01, 0x14, 0x01};
    PN532_WriteCommand(cmd, sizeof(cmd));
    PN532_ReadAck();

    uint8_t response[16];
    int16_t result = PN532_ReadResponse(response, sizeof(response));
    return (result > 0);
}
// Returns UID length, fills uid[] buffer. Returns 0 if no card found.
uint8_t PN532_ReadPassiveTarget(uint8_t *uid)
{
    uint8_t cmd[] = {PN532_COMMAND_INLISTPASSIVETARGET, 0x01, 0x00};
    PN532_WriteCommand(cmd, sizeof(cmd));
    PN532_ReadAck();

    uint8_t response[32];
    int16_t len = PN532_ReadResponse(response, sizeof(response));

    if (len < 0) return 0;

    // response[0] = command response code (0x4B) - skip it
    if (response[1] != 1) return 0; // NbTg check (fixed offset)

    uint8_t uidLength = response[6];      // fixed offset
    memcpy(uid, &response[7], uidLength); // fixed offset

    return uidLength;
}

void UART_Print(char *msg)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

void UART_PrintUID(uint8_t *uid, uint8_t len)
{
    char buf[64];
    char temp[8];
    strcpy(buf, "UID: ");
    for (uint8_t i = 0; i < len; i++) {
        sprintf(temp, "%02X ", uid[i]);
        strcat(buf, temp);
    }
    strcat(buf, "\r\n");
    UART_Print(buf);
}

// Reads and discards the 6-byte ACK frame the PN532 sends after every command
static HAL_StatusTypeDef PN532_ReadAck(void)
{
    uint8_t ackBuf[7]; // 1 status byte + 6 ACK bytes

    if (PN532_WaitReady(1000) != HAL_OK) return HAL_TIMEOUT;

    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&hi2c1, PN532_I2C_ADDR, ackBuf, 7, 100);

    // Optional: verify it's actually 00 00 FF 00 FF 00 (skipping ackBuf[0], the status byte)
    // if (ackBuf[1]!=0x00 || ackBuf[2]!=0x00 || ackBuf[3]!=0xFF || ackBuf[4]!=0x00 || ackBuf[5]!=0xFF || ackBuf[6]!=0x00)
    //     return HAL_ERROR;

    return status;
}
