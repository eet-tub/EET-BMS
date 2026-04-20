/*
 * TLE_Abstraction.h
 *
 *  Created on: Sep 8, 2022
 *      Author: max
 */
#include "TLE_Abstraction.h"
#include "math.h"

uint16_t balancingmask = 0;
uint8_t tle_deviceadr = 0;

uint8 numberofcells;
uint8 numberofauxchannels;

uint16_t general_Diagnostics = 0;

uint16_t scvm = 0;
uint16_t pcvm_stress_correction = 0;

int16_t adc_zeroval;

const NTC_Resistor_t ntc3950 = {
		.ntc_base_value = 100000,
		.ntc_base_temperature = CONVERT_TO_KELVIN(25),
		.ntc_b_value = 3950, //Maybe time 1000?
		.ntc_b_temperature = CONVERT_TO_KELVIN(50)
};

TempCurrentSource_t tempCurrentSource = SOURCE_5UA;


static float fsrtimespow4table[4] = {2.0,8.0,32.0,128.0};

/** @brief initilization function for the BMS
 *  @note This function has to be called before any other BMS function is used
 *  @param *huart Pointer to a uart peripheral handler that is used to communicate with the TLE
 *	@param numofcells Number of cells monitored by the BMS
 *	@param numofaux Number of additional channels monitored by the BMS
 *	@retval This function should always return 0
 */

uint8 initAMS(UART_HandleTypeDef* huart, uint8 numofcells, uint8 numofaux)
{
	numberofcells = numofcells;
	numberofauxchannels = numofaux;

	TLE_LL_Init(huart);
	wakeUpSignal();
	HAL_Delay(10);
	writeReg(0x00, OP_MODE, (0x0002)); //Set Watchdog in Extended Mode Mode
	amsConfigCellMeasurement(numofcells);	//Set up Cell Channels
	configMultiread(0x00, 12, 1, 3, 0, 1, 0, 0); //Measure Cell Voltages, Block Voltage, 3 temp sensors
	amsConfigTempSense(3, tempCurrentSource);
	amsConfigOverVoltage(DEFAULT_OV);
	amsConfigUnderVoltage(DEFAULT_UV);
	serveWatchdog(tle_deviceadr);
	amsClearCells();

	writeReg(tle_deviceadr, INT_OT_WARN_CONF, INTERNAL_OVERTEMP); //Set Internal Temperature Overtemp Fault Temp
	writeReg(tle_deviceadr, FAULT_MASK, 0xFFE0); //Activates Error Indication Pin and Enables all Faults to Trigger a Error

	return 0;
}

/**
 * @brief Send a wakeup signal to the TLE9012 BMS IC
 * @retval This function should always return 0
 */
uint8 amsWakeUp()
{
	wakeUpSignal();
	return 0;
}

/**
 * @brief Measure all cell voltages of monitored cells
 * @param *module Pointer to the Cell_Module struct holding measurement data
 * @retval This function should always return 0
 */

uint8 amsCellMeasurement(Cell_Module_t *module)
{
	uint16_t cellVoltages[MAXIMUM_CELL_VOLTAGES];
	uint32_t timestamp = HAL_GetTick(); //Get Timestamp right before measurement starts
	writeReg(0x00, MEAS_CTRL, 0xEE23); //Measure Block and Cell Voltages with 16bit Balancing off 1tvm_del_lsb setteling time
	module->timestamp = timestamp; //Write after Measurement @TODO check if the write was successfull
	HAL_Delay(5);
	if(module->currentsense == CURRENT_SENSE_INACTIVE)
	{
		multiRead(tle_deviceadr, cellVoltages, &module->sumOfCellMeasurements, module->auxVoltages, &module->internalDieTemp, &scvm, &pcvm_stress_correction);
	}
	else if(module->currentsense == CURRENT_SENSE_ACTIVE)
	{
		multiRead(tle_deviceadr, cellVoltages, &module->currentsenseraw, module->auxVoltages, &module->internalDieTemp, &scvm, &pcvm_stress_correction);
		module->current = (((float) module->currentsenseraw-adc_zeroval) * BAVM_RANGE/32768)/(CURRENT_SHUNT*SENSE_AMP_GAIN);
	}
	for(uint8_t n = 0; n < numberofcells; n++)
	{
		module->cellVoltages[n] = cellVoltages[MAXIMUM_CELL_VOLTAGES-numberofcells+n];
	}

	for(uint8_t n = 0; n < numberofauxchannels; n++)
	{
		module->Temperatures[n] = amsConvertAuxToTemp(module->auxVoltages[n], tempCurrentSource, ntc3950);
	}



	return 0;
}

/**
 * @brief Configure the number of cells that are monitored
 * @param number of cells that shall be monitored
 * @note See User Manual 4.3 for the register description
 * @retval CRC3 value of the response Frame? @TODO Check if this is the case
 */

uint8 amsConfigCellMeasurement(uint8 numberofChannels)
{
	uint8_t deviceadr = 0;
	uint16_t bitmask = 0x00;
	for(uint8_t n = 0; n < numberofChannels;n++)
	{
		bitmask |= (1<<(11-n));
	}
	return writeReg(deviceadr, PART_CONFIG, bitmask);
}

/**
 * @brief Measure aux channels e.g temperatures
 * @param *module Pointer to the Cell_Module struct holding measurement data
 * @retval This function should always return 0
 */

uint8 amsAuxMeasurement(Cell_Module_t *module)
{
	uint16_t cellVoltages[MAXIMUM_CELL_VOLTAGES];
	writeReg(tle_deviceadr, MEAS_CTRL, 0x66A1);
	HAL_Delay(5);
	if(module->currentsense == CURRENT_SENSE_INACTIVE)
	{
		multiRead(tle_deviceadr, cellVoltages, &module->sumOfCellMeasurements, module->auxVoltages, &module->internalDieTemp, &scvm, &pcvm_stress_correction);
	}
	else if(module->currentsense == CURRENT_SENSE_ACTIVE)
	{
		multiRead(tle_deviceadr, cellVoltages, &module->currentsenseraw, module->auxVoltages, &module->internalDieTemp, &scvm, &pcvm_stress_correction);
		module->current = (((float) module->currentsenseraw-adc_zeroval) * BAVM_RANGE/32768)/(CURRENT_SHUNT*SENSE_AMP_GAIN);
	}
	for(uint8_t n = 0; n < numberofcells; n++)
	{
		module->cellVoltages[n] = cellVoltages[MAXIMUM_CELL_VOLTAGES-numberofcells+n];
	}

	return 0;
}

/**
 * @brief configure the aux channels on the TLE9012
 * @param Channels Bitmask describing which aux channels shall be monitored
 * @retval This function should always return 0
 *
 */
uint8 amsConfigAuxMeasurement(uint16 Channels)
{
	uint16_t avm_config = 0;
	readReg(tle_deviceadr, AVM_CONFIG, &avm_config);
	avm_config |= (Channels & 0x1F) << 3;
	return 0;
}

/**
 * @brief Read the general Diagnose registers
 * @note currently only for debug use
 * @retval This function always returns 0
 */
uint8 amsInternalStatusMeasurement(Cell_Module_t *module)
{
	readReg(tle_deviceadr, GEN_DIAG, &general_Diagnostics);

	return 0;
}

/**
 * @brief not implemented
 */

uint8 amsConfigGPIO(uint16 gpios)
{
	return 1;
}

/**
 * @brief not implemented
 */
uint8 amsSetGPIO(uint16 gpios)
{
	return 1;
}

/**
 * @brief not implemented
 */
uint8 readGPIO(Cell_Module_t* module)
{
	return 1;
}

/**
 * @brief Configure which cells shall be balanced after the next amsStartBalancing function call
 * @param Channels Bitmask describing which cells should be balanced
 * @retval This function always returns 0
 */
uint8 amsConfigBalancing(uint32 Channels)
{
	balancingmask = (uint16_t) Channels << (MAXIMUM_CELL_VOLTAGES - numberofcells);
	return 0;
}

/**
 * @brief Enable balancing for configured cells
 * @param dutyCycle see User Manual 4.51
 */
uint8 amsStartBalancing(uint8 dutyCycle)
{
	uint8_t deviceAdr = 0x00;

	writeReg(deviceAdr, BAL_PWM, (dutyCycle & 0x07));
	return writeReg(deviceAdr, BAL_SETTINGS, balancingmask);
}

/**
 * @brief Disable Balancing for all cells
 */
uint8 amsStopBalancing()
{
	uint8_t deviceAdr = 0x00;
	return writeReg(deviceAdr, BAL_SETTINGS, 0x0000);
}

/**
 * @note Not implemented yet
 *
 * Hard to decide what this actually does considering the round robin that is integrated into the TLE
 */
uint8 amsSelfTest()
{
	return 0;
}

/**
 * @brief Configure undervoltage limit for all cells
 * @param Voltage value in voltage/5 * 1024
 */
uint8 amsConfigUnderVoltage(uint16 underVoltage)
{
	//uint16_t uV = underVoltage/3; //convert ADBMS per 16*100uV to TLEs 5V/1023 scaling
	//uV |= 0x00<<10; // TODO Calculate correct Values
	return writeReg(tle_deviceadr, OL_UV_THR, underVoltage);
}

/**
 * @brief check voltage limits and write under/overvoltage flags
 * @note voltage values are checked against hard coded limits and the over/undervoltage registers inside the TLE
 * @note currently only the internal registers are relevant for the return value
 * @param *module Cell_Module Pointer to the struct holding measurement data
 * @retval 1 if over/undervoltage was detected
 * 		   0 if no failure is detected
 */
uint8 amsCheckUnderOverVoltage(Cell_Module_t *module)
{
	uint16_t undervoltageflags = 0;
	uint16_t overvoltageflags = 0;
	readReg(tle_deviceadr, CELL_UV, &undervoltageflags);
	readReg(tle_deviceadr, CELL_OV, &overvoltageflags);
	module->overVoltage = overvoltageflags;
	module->underVoltage = undervoltageflags;

	uint8_t secondlevel_errorflag = 0;

	for(uint8_t n = 0; n < numberofcells; n++)
	{
		uint16_t cellVoltage = module->cellVoltages[n];
		if((cellVoltage > (758<<6)) || (cellVoltage < (389<<6)))
		{
			secondlevel_errorflag++;
		}
	}
	if(secondlevel_errorflag == 0)
		writeReg(tle_deviceadr, GEN_DIAG, 0xCFFF);

	if(undervoltageflags || overvoltageflags)
		return 1;
	else
		return 0;
}

/**
 * @brief Configure overvoltage limit for all cells
 * @param Voltage value in voltage/5 * 1024
 */
uint8 amsConfigOverVoltage(uint16 overVoltage)
{
	//uint16_t oV = overVoltage/3; //convert ADBMS per 16*100uV to TLEs 5V/1023 scaling
	//oV |= 0x3F<<10; // TODO Calculate correct Values
	return writeReg(tle_deviceadr, OL_OV_THR, overVoltage);
}

/**
 * @brief Check if open wire faults are detected
 * @param *module Cell_Module Pointer to the struct holding measurement data
 * @retval 1 if an open load error was detected
 * 		   0 if no fault was detected
 */
uint8 amscheckOpenCellWire(Cell_Module_t *module)
{
	uint16_t open_load = 0;
	readReg(tle_deviceadr, DIAG_OL, &open_load);

	if(open_load)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief Reset the general fault and diagnostic register
 */
uint8 amsClearStatus()
{
	writeReg(tle_deviceadr, GEN_DIAG, 0x0000); //Reset Fault Diagnostic Register
	return 0;
}

/**
 * Not implemented
 */
uint8 amsClearAux()
{
	return 1;
}

/**
 * Clear cell over and undervoltage limits
 */
uint8 amsClearCells()
{
	writeReg(tle_deviceadr, CELL_UV, 0x0000);
	writeReg(tle_deviceadr, CELL_OV, 0x0000);
	return 0;
}

/**
 * @brief Set a warning flag via the connected optocoupler
 */

uint8 amsSendWarning()//Switched Error and Warning for BMS Tests...
{

	return 0;
}

/**
 * @brief Set an error flag via the connected optocoupler
 */
uint8 amsSendError()
{

	return 0;
}

/**
 * @brief Clear the warning flag via the connected optocoupler
 */

uint8 amsClearWarning()
{

	return 0;
}

/**
 * @brief Clear the error flag via the connected optocoupler
 */

uint8 amsClearError()
{
	//HAL_GPIO_WritePin(AMS_Warning_GPIO_Port, AMS_Warning_Pin, GPIO_PIN_RESET);
	return 0;
}

void amsConfigCurrentSense(Cell_Module_t *module,current_sense_state_t sensestate)
{

	uint16_t regval = 0;
	readReg(tle_deviceadr, MEAS_CTRL, &regval);
	regval &= ~(0x0070);
	regval |= 0x0060;
	writeReg(tle_deviceadr, MEAS_CTRL, regval);

	readReg(tle_deviceadr, AVM_CONFIG, &regval);
	switch(sensestate)
	{
	case CURRENT_SENSE_INACTIVE:
		regval &= !(0x0100);
		module->currentsense = CURRENT_SENSE_INACTIVE;
		break;
	case CURRENT_SENSE_ACTIVE:
		regval |= (0x7F07);
		module->currentsense = CURRENT_SENSE_ACTIVE;
		break;
	}

	writeReg(tle_deviceadr, AVM_CONFIG, regval);

}


void amsCalibrateCurrentSense(int16_t zeroval)
{
	adc_zeroval = (int) zeroval;
}

/**
 * Dupliate of amsClearStatus()
 */
void resetGenDiag()
{
	writeReg(tle_deviceadr, GEN_DIAG, 0x0000);
}

void resetWatchdog()
{
	serveWatchdog(tle_deviceadr);
}

void amsConfigTempSense(uint8_t nroftempsense,TempCurrentSource_t source)
{
	//Return if number of sensors is invalid
	if(nroftempsense > 5) return;

	//Set Number of temperature Sensors
	writeReg(tle_deviceadr, TEMP_CONF, (nroftempsense<<12) | (source<<10));

	//Configure AVMConf Temp sensor settings
	uint16_t avm_conf;
	readReg(tle_deviceadr, AVM_CONFIG, &avm_conf);

	//Configure Sources in
	avm_conf &= ~(0x78F8);
	avm_conf |= source<<11; //Config Source 0
	avm_conf |= source<<13; //Config Source 1

	uint16_t temp_sense_manual_mask = 0x0000;
	for(uint8_t n = 0; n<nroftempsense; n++)
	{
		temp_sense_manual_mask |= 1<<(n+3);
	}
	avm_conf |= temp_sense_manual_mask;

	writeReg(tle_deviceadr, AVM_CONFIG, avm_conf);
}

float amsConvertAuxToTemp(uint16_t adcval, TempCurrentSource_t source, NTC_Resistor_t ntc)
{
	uint8_t sourcenr = (adcval>>10)&0x03;
	adcval = adcval & 0x3FF;
	float resistance = (((float)(adcval))*fsrtimespow4table[sourcenr])/(1024*0.000320) - 100; //Equation 3.4.2 from User Manual
	float temp = 1/(1/(ntc.ntc_base_temperature) + 1/(ntc.ntc_b_value)*logf(resistance/ntc.ntc_base_value)); //Shortend Steinhart-Hart
	return CONVERT_TO_CELSIUS(temp);
}

void amsEnterSleepMode()
{
	uint16_t data;
	readReg(tle_deviceadr, OP_MODE, &data);
	data |= 0x01;
	writeReg(tle_deviceadr, OP_MODE, data);
}
