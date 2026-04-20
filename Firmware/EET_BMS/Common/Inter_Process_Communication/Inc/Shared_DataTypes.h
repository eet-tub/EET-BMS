
#ifndef _SHARED_DATA_TYPES
#define _SHARED_DATA_TYPES

//Chip dependent limits
#define MAXIMUM_CELL_VOLTAGES 12
#define MAXIMUM_AUX_VOLTAGES 5
#define MAXIMUM_GPIO 2

#include <stdint.h>

typedef enum
{
	CURRENT_SENSE_INACTIVE,
	CURRENT_SENSE_ACTIVE
} current_sense_state_t;

//A cell module struct describes all relevant battery values and internal BMS states
typedef struct
{
	uint16_t cellVoltages[MAXIMUM_CELL_VOLTAGES];	//cell voltage in ADC Quants
	uint16_t auxVoltages[MAXIMUM_AUX_VOLTAGES];	//aux voltages e.g. temp sensors

	uint16_t internalDieTemp;						//Internal DIE Temperature User Manual 4.35
	uint16_t sumOfCellMeasurements;				//Voltage of the whole battery measured by an redundant ADC (User Manual BVM 4.28)
	uint16_t refVoltage;

	uint16_t GPIO_Values[MAXIMUM_GPIO];			//Values of the GPIO Pins

	uint32_t overVoltage;							//Overvoltage flags see User Manual 4.13
	uint32_t underVoltage;						//Undervoltage flags see User Manual 4.12

	current_sense_state_t currentsense;			//State of the Current Sensor because sumOfCellMeasurements is disabled if currentsense is active
	int16_t currentsenseraw;					//Raw ADC Value of the current sensor
	float current;								//Current converted to ampere

	float Temperatures[MAXIMUM_AUX_VOLTAGES]; //Temperature Values
	uint32_t timestamp;

} Cell_Module_t;

typedef struct{
	float SoC[MAXIMUM_CELL_VOLTAGES];
	float SoH[MAXIMUM_CELL_VOLTAGES];

	float module_SoC;
	float module_SoH;

	float remaining_operating_time;
	float RuL;

} State_Estimation_t;

#endif
