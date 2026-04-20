/*
 * BMS_State_Estimation.h
 *
 *  Created on: Jan 15, 2024
 *      Author: max
 */

#ifndef BMS_STATE_ESTIMATION_H_
#define BMS_STATE_ESTIMATION_H_

#include "Shared_DataTypes.h"

#define BATTERYCAPACITY 6480 //Cell Capacity in Ampere seconds (1.8*3600)

State_Estimation_t bmsStateEstimationCallback(Cell_Module_t* module);



#endif /* BMS_STATE_ESTIMATION_H_ */
