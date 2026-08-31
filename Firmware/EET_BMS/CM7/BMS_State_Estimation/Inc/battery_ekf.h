/*
 * battery_ekf.h
 *
 *  Created on: Oct 19, 2025
 *      Author: CQN
 */

#ifndef INC_BATTERY_EKF_H_
#define INC_BATTERY_EKF_H_
#include <stdint.h>

typedef struct
{
    // EKF state:
    // x[0] = SOC
    // x[1] = V1
    // x[2] = V2
    float x[3];

    // State covariance
    float P[9];

    // Prediction values
    float x_p[3];
    float Pp[9];

    // Kalman gain
    float K[3];

    // Temporary vector used during covariance update
    float temp_vec[3];

    // Previous current sample
    float I_prev;

    // Last calculated SOC
    float last_soc;

    // Last estimated terminal voltage
    float y_k1;

    // Rest Voltage
    float V_rest;
    uint8_t rest_voltage_valid;

    uint8_t soc_initialized;
    uint8_t first_sample;

} BatteryEKF_t;

extern float last_soh;

void EKF_Init(BatteryEKF_t *ekf);
/* Process one voltage/current sample for one cell */
void EKF_Process(BatteryEKF_t *ekf, float volt, float curr, float deltaT);

/* Read calculated SOC */
float EKF_GetSOC(const BatteryEKF_t *ekf);


#endif /* INC_BATTERY_EKF_H_ */
