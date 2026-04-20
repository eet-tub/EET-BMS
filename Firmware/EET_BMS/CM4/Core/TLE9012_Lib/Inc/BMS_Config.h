/*
 * BMS_Config.h
 *
 *  Created on: Jan 19, 2024
 *      Author: max
 */

#ifndef TLE9012_LIB_INC_BMS_CONFIG_H_
#define TLE9012_LIB_INC_BMS_CONFIG_H_

#define CONVERT_VOLTAGE_TO_10BIT_VALUE(x) ((uint16_t) (x*1024/5))
#define CONVERT_16_BIT_VALUE_TO_VOLTAGE(x) (((float) x) * 5/65536)

#define SET_UNDERVOLTAGE_LIMIT(x) CONVERT_VOLTAGE_TO_10BIT_VALUE(x)
#define SET_OVERVOLTAGE_LIMIT(x) (CONVERT_VOLTAGE_TO_10BIT_VALUE(x) | 0x3F<<10)

//Default values if no other value is provided
//See User Manual 4.4 and 4.5 for the corresponding registers
#define DEFAULT_UV SET_UNDERVOLTAGE_LIMIT(MINIMUM_VOLTAGE)
#define DEFAULT_OV SET_OVERVOLTAGE_LIMIT(MAXIMUM_VOLTAGE)


#define MAXIMUM_CHARGE_CURRENT 4.0
#define MAXIMUM_DISCHARGE_CURRENT 4.0

#define MAXIMUM_VOLTAGE 4.0
#define MINIMUM_VOLTAGE 2.5
#define MAXIMUM_TEMPERATURE 60.0
#define MINIMUM_TEMPERATURE -10.0


#define NUMBEROFCELLS 4
#define NUMBEROFTEMPS 3

#define CURRENT_SHUNT 0.00582 //in Ohm
#define SENSE_AMP_GAIN 100 //According to Datasheet

//This struct holds all informations about the battery and limits and can be used to overwrite the programmed default values
typedef struct
{
	float undervoltage;
	float overvoltage;
	float undertemp;
	float overtemp;
	float max_current_charge;
	float max_current_discharge;
	float shuntvalue;
	float senseampgain;
} bms_config_limits_t;



#define BALANCING_THRESHOLD 3.4
#define BALANCING_VOLTAGE_DELTA 0.01


#endif /* TLE9012_LIB_INC_BMS_CONFIG_H_ */
