/*
 * BMS_State_Estimation.c
 *
 *  Created on: Jan 15, 2024
 *      Author: max
 */


#include "BMS_State_Estimation.h"
#include "Shared_DataTypes.h"
#include "battery_ekf.h"

float StateEstimationGetSoCbyCoulombCounting(Cell_Module_t* module);

uint32_t last_timestamp = 0;
float coulomb_counter_value = BATTERYCAPACITY;

#define EKF_CELL_COUNT 4U
static BatteryEKF_t cell_ekf[EKF_CELL_COUNT];
static uint8_t ekf_initialized = 0U;
static uint32_t last_ekf_timestamp = 0U;
float last_soh = 1.0f;

static float StateEstimationCellVoltageToVolt(uint16_t raw_voltage)
{
    return ((float)raw_voltage * 5.0f) / 65536.0f;
}



State_Estimation_t bmsStateEstimationCallback(Cell_Module_t* module)
{

	/*
     * Initialize all four EKF instances once
     */
    if (!ekf_initialized)
    {
        for (uint8_t i = 0U; i < EKF_CELL_COUNT; i++)
        {
            EKF_Init(&cell_ekf[i]);
        }

        ekf_initialized = 1U;
    }


    /*
     * Calculate elapsed time for EKF
     */
    float delta_t_s = 0.0f;

    if (last_ekf_timestamp == 0U)
    {
        /*
         * First state-estimation cycle after boot:
         * only store timestamp.
         */
        last_ekf_timestamp = module->timestamp;
    }
    else
    {
        uint32_t delta_t_ms =
            module->timestamp - last_ekf_timestamp;

        last_ekf_timestamp = module->timestamp;

        delta_t_s =
            (float)delta_t_ms / 1000.0f;
    }
    float ekf_current = -module->current;
    float cell_voltage[EKF_CELL_COUNT];

    for (uint8_t i = 0U; i < EKF_CELL_COUNT; i++)
    {
        cell_voltage[i] =
            StateEstimationCellVoltageToVolt(module->cellVoltages[i]);
    }
    /*
     * Run EKF for all four cells
     */
    if (delta_t_s > 0.0f)
    {
        for (uint8_t i = 0U; i < EKF_CELL_COUNT; i++)
        {
            EKF_Process(&cell_ekf[i],
                        cell_voltage[i],
                        ekf_current,
                        delta_t_s);
        }

    }
    /*
     * Store individual cell EKF SOC values
     */
    for (uint8_t i = 0U; i < EKF_CELL_COUNT; i++)
    {
        new_state_estimation.SoC[i] =
            EKF_GetSOC(&cell_ekf[i]);
    }
//    /*
//     * Calculate average SOC of all four cell EKFs
//     */
//    new_state_estimation.module_SoC_Avg =
//        (new_state_estimation.SoC[0] +
//         new_state_estimation.SoC[1] +
//         new_state_estimation.SoC[2] +
//         new_state_estimation.SoC[3]) / 4.0f;

	//old coulomb counting
	new_state_estimation.module_SoC = StateEstimationGetSoCbyCoulombCounting(module);


	return new_state_estimation;
}

float StateEstimationGetSoCbyCoulombCounting(Cell_Module_t* module)
{
	if(last_timestamp == 0)
	{
		//Only called in first iteration after bootup
		last_timestamp = module->timestamp;
	}
	else
	{

		uint32_t delta_t = module->timestamp - last_timestamp;
		last_timestamp = module->timestamp;

		coulomb_counter_value = coulomb_counter_value - (module->current*delta_t)/1000;
	}

	//Coulomb Counter Upper Limit
	if(coulomb_counter_value > BATTERYCAPACITY)
		coulomb_counter_value = BATTERYCAPACITY;

	//Coulomb Counter Lower Limit
	if(coulomb_counter_value < 0)
		coulomb_counter_value = 0;

	return coulomb_counter_value/BATTERYCAPACITY;
}
