/*
 * TLE_Abstraction.h
 *
 *  Created on: Sep 5, 2022
 *      Author: max
 */

#ifndef INC_TLE_ABSTRACTION_H_
#define INC_TLE_ABSTRACTION_H_

#include "main.h"
#include "TLE_Makros.h"
#include "TLE_LL_Driver.h"
#include "Shared_DataTypes.h"
#include "BMS_Config.h"





#define INTERNAL_OVERTEMP 700 //83.62°C => -0.6624°K * 700 + 547.3°K
#define BAVM_RANGE 2 //ADC Range in Volt (+-2V)


extern volatile uint8_t tleuart_rxcplt;


#define CONVERT_TO_KELVIN(x) x+273.15
#define CONVERT_TO_CELSIUS(x) x-273.15

typedef struct
{
	float ntc_base_value;
	float ntc_base_temperature;
	float ntc_b_value;
	float ntc_b_temperature;
} NTC_Resistor_t;





uint8 initAMS(UART_HandleTypeDef* huart, uint8 numofcells, uint8 numofaux);
uint8 amsWakeUp();

uint8 amsCellMeasurement(Cell_Module_t *module);
uint8 amsConfigCellMeasurement(uint8 numberofChannels);

uint8 amsAuxMeasurement(Cell_Module_t *module);
uint8 amsConfigAuxMeasurement(uint16 Channels);

uint8 amsInternalStatusMeasurement(Cell_Module_t *module);

uint8 amsConfigGPIO(uint16 gpios);
uint8 amsSetGPIO(uint16 gpios);
uint8 readGPIO(Cell_Module_t* module);

uint8 amsConfigBalancing(uint32 Channels);
uint8 amsStartBalancing(uint8 dutyCycle);
uint8 amsStopBalancing();

uint8 amsSelfTest();

uint8 amsConfigUnderVoltage(uint16 underVoltage);

uint8 amsCheckUnderOverVoltage(Cell_Module_t *module);
uint8 amsConfigOverVoltage(uint16 overVoltage);

uint8 amscheckOpenCellWire(Cell_Module_t *module);

uint8 amsClearStatus();
uint8 amsClearAux();
uint8 amsClearCells();

uint8 amsSendWarning();
uint8 amsSendError();

uint8 amsClearWarning();
uint8 amsClearError();

void amsConfigCurrentSense(Cell_Module_t *module,current_sense_state_t sensestate);
void amsCalibrateCurrentSense(int16_t zeroval);

void resetGenDiag();
void resetWatchdog();

float amsConvertAuxToTemp(uint16_t adcval, TempCurrentSource_t source, NTC_Resistor_t ntc);
void amsConfigTempSense(uint8_t nroftempsense,TempCurrentSource_t source);

void amsEnterSleepMode();

#endif /* INC_TLE_ABSTRACTION_H_ */
