/*
 * BMS_Functions.c
 *
 *  Created on: Jan 19, 2024
 *      Author: max
 */

#include "BMS_Functions.h"
#include "BMS_Config.h"
#include "TLE_Abstraction.h"
#include "Shared_DataTypes.h"
#include "IPPC_Functions.h"

static Cell_Module_t* tle_module;
static bms_config_limits_t bms_limits =
		{
		.max_current_charge = MAXIMUM_CHARGE_CURRENT,
		.max_current_discharge = MAXIMUM_DISCHARGE_CURRENT,
		.overvoltage = MAXIMUM_VOLTAGE,
		.undervoltage = MINIMUM_VOLTAGE,
		.overtemp = MAXIMUM_TEMPERATURE,
		.undertemp = MINIMUM_TEMPERATURE,
		.shuntvalue = CURRENT_SHUNT,
		.senseampgain = SENSE_AMP_GAIN
		};
static Bms_Error_t bms_error_state = BMS_NO_ERROR;
static Bms_State_t bms_state;
static State_Estimation_t bms_state_estimations;
static uint16_t tle_gendiag_reg;

void BMS_Init(UART_HandleTypeDef *huart,Cell_Module_t* module, uint8_t ncells, uint8_t ntemps)
{

	HAL_Delay(10000);
	initAMS(huart, ncells, ntemps);
	tle_module = module;

	amsConfigCurrentSense(module, CURRENT_SENSE_ACTIVE);
	amsCellMeasurement(module);

	HAL_GPIO_WritePin(BAT_Charge_GPIO_Port, BAT_Charge_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BAT_Discharge_GPIO_Port, BAT_Discharge_Pin, GPIO_PIN_RESET);
	serveWatchdog(0);
	HAL_Delay(1000);
	serveWatchdog(0);
	amsCellMeasurement(module);
	serveWatchdog(0);
	HAL_Delay(100);
	serveWatchdog(0);
	amsCellMeasurement(module);
	amsCalibrateCurrentSense(module->currentsenseraw);

	HAL_GPIO_WritePin(BAT_Charge_GPIO_Port, BAT_Charge_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BAT_Discharge_GPIO_Port, BAT_Discharge_Pin, GPIO_PIN_SET);
	resetGenDiag();
	serveWatchdog(0);
}

void BMS_loop()
{
	serveWatchdog(0);
	BMS_Measure();
	bms_error_state = BMS_CheckLimits();
	BMS_Balance();
	BMS_ErrorHandler(bms_error_state);
}

void BMS_Wakeup()
{
	wakeUpSignal();
	bms_state = BMS_OK;
}

void BMS_Sleep()
{
	bms_state = BMS_SLEEP;
	amsEnterSleepMode();
}

void BMS_Measure()
{
	amsCellMeasurement(tle_module);
}

Bms_Error_t BMS_CheckLimits()
{
	//Check Under and Overvoltage conditions
	if(tle_module->overVoltage != 0)
	{
		return BMS_CELL_OVERVOLTAGE;
	}

	if(tle_module->underVoltage != 0)
	{
		return BMS_CELL_UNDERVOLTAGE;
	}

	if(tle_module->current > bms_limits.max_current_charge)
	{
		return BMS_OVERCURRENT;
	}

	if(tle_module->current < (-bms_limits.max_current_discharge))
	{
		return BMS_OVERCURRENT;
	}

	//Check Cell Voltages
	for(uint8_t n = 0; n < NUMBEROFCELLS; n++)
	{
		float cellvoltage = CONVERT_16_BIT_VALUE_TO_VOLTAGE(tle_module->cellVoltages[n]);

		if(cellvoltage > bms_limits.overvoltage)
		{
			return BMS_CELL_OVERVOLTAGE;
		}
		if(cellvoltage < bms_limits.undervoltage)
		{
			return BMS_CELL_UNDERVOLTAGE;
		}
	}

	//Check Temp Sensors
	for(uint8_t n = 0; n < NUMBEROFTEMPS; n++)
	{
		if(tle_module->Temperatures[n] > bms_limits.overtemp)
		{
			return BMS_OVERTEMP;
		}
		if(tle_module->Temperatures[n] < bms_limits.undertemp)
		{
			return BMS_UNDERTEMP;
		}
	}

	//Handle General Diagnostic Register Faults
	GPIO_PinState bms_error_pin = HAL_GPIO_ReadPin(TLE_Error_GPIO_Port,TLE_Error_Pin);
	if(bms_error_pin == GPIO_PIN_SET)
	{
		readReg(0, GEN_DIAG, &tle_gendiag_reg); //Read general diagnostic register
		//TODO Handle General Diagnostic Faults properly
		resetGenDiag(); //If all Checks pass, reset Gen Diagnostic interface
	}

	return BMS_NO_ERROR;
}

void BMS_Balance()
{

	if(bms_error_state != BMS_NO_ERROR) //Early Return if BMS is in an error state
	{
		amsStopBalancing();
		return;
	}

	if(tle_module->current < -0.1)	//Early Return if Discharge is in Progress
	{
		amsStopBalancing();
		return;
	}

	BMS_Balancing_Algorithm(tle_module); //Enter Balancing Algorithm if all checks pass

}

__weak void BMS_Balancing_Algorithm(Cell_Module_t *module)
{

	float maximum_voltage = CONVERT_16_BIT_VALUE_TO_VOLTAGE(module->cellVoltages[0]);
	float minimum_voltage = CONVERT_16_BIT_VALUE_TO_VOLTAGE(module->cellVoltages[0]);

	float cell_voltages_f[NUMBEROFCELLS];

	for(uint8_t n = 0; n < NUMBEROFCELLS; n++)
		cell_voltages_f[n] = CONVERT_16_BIT_VALUE_TO_VOLTAGE(module->cellVoltages[n]);
	for(uint8_t n = 0; n < NUMBEROFCELLS; n++)
	{
		if(cell_voltages_f[n] > maximum_voltage)
			maximum_voltage = cell_voltages_f[n];

		if(cell_voltages_f[n] < minimum_voltage)
			minimum_voltage = cell_voltages_f[n];
	}

	if(maximum_voltage < BALANCING_THRESHOLD)	//Return if no cell is above balancing threshold
	{
		return;
	}

	uint16_t channelstobalance = 0; //Balancing Bitmask

	for(uint8_t n = 0; n < NUMBEROFCELLS; n++)	//Check for cells above the Balancing Voltage Delta
	{
		if((cell_voltages_f[n]-minimum_voltage)>BALANCING_VOLTAGE_DELTA)
		//if((cell_voltages_f[n]-BALANCING_THRESHOLD )>BALANCING_VOLTAGE_DELTA )
		{
			channelstobalance |= 1<<n;
		}
	}

	amsConfigBalancing(channelstobalance);
	amsStartBalancing(BAL_PWM_1000);
}

void BMS_ErrorHandler(Bms_Error_t error)
{
	switch(error)
	{
	case BMS_NO_ERROR:
		//HAL_GPIO_WritePin(BAT_Relay_GPIO_Port, BAT_Relay_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BAT_Charge_GPIO_Port, BAT_Charge_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BAT_Discharge_GPIO_Port, BAT_Discharge_Pin, GPIO_PIN_SET);
		break;
	case BMS_CELL_UNDERVOLTAGE:
		HAL_GPIO_WritePin(BAT_Charge_GPIO_Port, BAT_Charge_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BAT_Discharge_GPIO_Port, BAT_Discharge_Pin, GPIO_PIN_RESET);
		break;
	case BMS_CELL_OVERVOLTAGE:
		HAL_GPIO_WritePin(BAT_Discharge_GPIO_Port, BAT_Discharge_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(BAT_Charge_GPIO_Port, BAT_Charge_Pin, GPIO_PIN_RESET);
		break;
	case BMS_COMMUNICATION_ERROR:
		break;
	case BMS_OVERTEMP:
		HAL_GPIO_WritePin(BAT_Discharge_GPIO_Port, BAT_Discharge_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(BAT_Charge_GPIO_Port, BAT_Charge_Pin, GPIO_PIN_RESET);
		break;
	case BMS_UNDERTEMP:
		break;
	case BMS_OVERCURRENT:
		break;

	default:
		break;

	}
}

void BMS_SetNewLimits(bms_config_limits_t limits)
{
	bms_limits = limits;
	amsConfigOverVoltage(SET_OVERVOLTAGE_LIMIT(bms_limits.overvoltage));
	amsConfigUnderVoltage(SET_UNDERVOLTAGE_LIMIT(bms_limits.undervoltage));
}

bms_config_limits_t BMS_GetLimits()
{
	return bms_limits;
}
