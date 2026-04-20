/*
 * TLE_LL_Driver.h
 *
 *  Created on: Sep 5, 2022
 *      Author: max
 */

#ifndef INC_TLE_LL_DRIVER_H_
#define INC_TLE_LL_DRIVER_H_

#include "TLE_Makros.h"
#include "main.h"

#define INITAL_CRC8 0xFF
#define UART_TIMEOUT 1000U

#define MAX_CRC_ERRORS 3

#define TARGET_STM32
#ifdef TARGET_STM32
	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
#endif

uint8 calculateCRC8(uint8* data, uint8 datalen);
uint8 updateCRC8(uint8 currentCRC, uint8 din);
uint8_t calculateCRC3(uint8_t data);
uint8_t writeReg(uint8 deviceadr, uint8 regadr, uint16 buffer);
uint8_t readReg(uint8_t deviceadr, uint8_t regadr, uint16_t *buffer);
uint8_t wakeUpSignal();
uint8_t serveWatchdog(uint8_t deviceadr);
uint8_t multiRead(uint8_t deviceadr, uint16_t* cellVoltages, uint16_t* blockVoltage, uint16_t* externalTemp, uint16_t* internalTemp, uint16_t* scvm, uint16_t* pcvm_stress);
uint8_t configMultiread(uint8_t deviceadr, uint8_t numofcells, uint8_t blockvoltage, uint8_t exttemp, uint8_t temp_r, uint8_t int_temp, uint8_t scvm, uint8_t pcvm_stress);

uint8_t uartWrite(uint8* buffer, uint8 datalen);
uint8_t uartRead(uint8* buffer, uint8 datalen);
HAL_StatusTypeDef readWrite(uint8_t * txbuffer, uint8_t txlen,uint8_t* rxbuffer, uint8_t rxlen);
uint8_t awaitRxCplt();
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);


uint8_t TLE_LL_Init(UART_HandleTypeDef* huart);


#endif /* INC_TLE_LL_DRIVER_H_ */
