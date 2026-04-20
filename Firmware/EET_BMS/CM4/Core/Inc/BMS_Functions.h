/*
 * BMS_Functions.h
 *
 *  Created on: Jan 19, 2024
 *      Author: max
 */

#ifndef INC_BMS_FUNCTIONS_H_
#define INC_BMS_FUNCTIONS_H_

#include <stdint.h>
#include "TLE_Abstraction.h"
#include "stm32h7xx_hal.h"

typedef enum{
	BMS_OK = 0,
	BMS_CHARGING,
	BMS_DISCHARGING,
	BMS_ERROR,
	BMS_SLEEP,
	BMS_NUMOFSTATES
} Bms_State_t;

typedef enum{
	BMS_NO_ERROR = 0,
	BMS_CELL_OVERVOLTAGE,
	BMS_CELL_UNDERVOLTAGE,
	BMS_COMMUNICATION_ERROR,
	BMS_OVERTEMP,
	BMS_UNDERTEMP,
	BMS_OVERCURRENT,
	BMS_TLE_ERROR
}Bms_Error_t;

void BMS_Init(UART_HandleTypeDef *huart, Cell_Module_t* module, uint8_t ncells, uint8_t ntemps);
void BMS_loop();

void BMS_Wakeup();
void BMS_Sleep();

void BMS_Measure();
Bms_Error_t BMS_CheckLimits();
void BMS_Balance();

void BMS_ErrorHandler(Bms_Error_t error);

__weak void BMS_Balancing_Algorithm(Cell_Module_t *module); //Default Voltage based. Override for individual Balancing Algorithm
void BMS_SetNewLimits(bms_config_limits_t limits);
bms_config_limits_t BMS_GetLimits();

#endif /* INC_BMS_FUNCTIONS_H_ */
