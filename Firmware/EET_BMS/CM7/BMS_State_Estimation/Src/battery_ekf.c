/*
 * battery_ekf.c
 *
 *  Created on: Oct 19, 2025
 *      Author: CQN
 */



#include "ECM_parameter.h"
#include "interpolation.h"
#include "battery_ekf.h"
#include "matrix.h"
#include <math.h>

static float C0 = 1.8f * 3600.0f;;
static float R_base = 9e-5f;

static float Q[9] = {
    1e-8f, 0.0f, 0.0f,
    0.0f, 2e-3f, 0.0f,
    0.0f, 0.0f, 2e-3f
};



static void ekf_run(BatteryEKF_t *ekf, float volt, float curr, float deltaT)
{
	float Cb = C0 * last_soh;
	float SoC = ekf->x[0];

	float eff=1.0f;
	// Parameter of charge or discharge
	float R1,R2,tau1,tau2,Ri,OCV,dOCV;
	if (ekf->I_prev >= 0.0f){
		R1=lookup2D_nearestSOH(&charge_R1, SoC,last_soh);
		R2=lookup2D_nearestSOH(&charge_R2, SoC,last_soh);
		tau1=lookup2D_nearestSOH(&charge_tau1, SoC,last_soh);
		tau2=lookup2D_nearestSOH(&charge_tau2, SoC,last_soh);
		eff=0.999f;
	}
	else{
		R1=lookup2D_nearestSOH(&discharge_R1, SoC,last_soh);
		R2=lookup2D_nearestSOH(&discharge_R2, SoC,last_soh);
		tau1=lookup2D_nearestSOH(&discharge_tau1, SoC,last_soh);
		tau2=lookup2D_nearestSOH(&discharge_tau2, SoC,last_soh);
	}


	// prediction
    float Ad[9] = {
        1.0f, 0.0f,                    0.0f,
        0.0f, expf(-deltaT / tau1),    0.0f,
        0.0f, 0.0f,                    expf(-deltaT / tau2)
    };

    float Bd[3] = {
        eff * deltaT / Cb,
        R1 * (1.0f - expf(-deltaT / tau1)),
        R2 * (1.0f - expf(-deltaT / tau2))
    };

    for (int i = 0; i < 3; i++)
    {
        ekf->x_p[i] =
            Ad[i]     * ekf->x[0] +
            Ad[i + 3] * ekf->x[1] +
            Ad[i + 6] * ekf->x[2] +
            Bd[i]     * ekf->I_prev;
    }

	float AT[9];
	float temp1[9];
	float temp2[9];
    mat3_trans(Ad, AT);
    mat3_mul(Ad, ekf->P, temp1);
    mat3_mul(temp1, AT, temp2);
    mat3_add(temp2, Q, ekf->Pp);

    /* =========================================================
     * Voltage prediction
     * ========================================================= */


    SoC = ekf->x_p[0];
    if (SoC < 0.0f)
        SoC = 0.0f;

    if (SoC > 1.0f)
        SoC = 1.0f;
    if (curr >= 0.0f){
		Ri=lookup2D_nearestSOH(&charge_Ri, SoC,last_soh);
		OCV=lookup2D_nearestSOH(&charge_OCV, SoC,last_soh);
		dOCV=lookup2D_nearestSOH(&charge_dOCV, SoC,last_soh);
	}
	else{
		Ri=lookup2D_nearestSOH(&discharge_Ri, SoC,last_soh);
		OCV=lookup2D_nearestSOH(&discharge_OCV, SoC,last_soh);
		dOCV=lookup2D_nearestSOH(&discharge_dOCV, SoC,last_soh);
	}

	float Cd[3]={dOCV,1.0f,1.0f};
    float y_p =
        OCV +
        ekf->x_p[1] +
        ekf->x_p[2] +
        Ri * curr;
    /* =========================================================
     * EKF Update
     * ========================================================= */

    float delta_y = volt - y_p;

	//S=Cd*P_p*Cd'+R
    float S =
        Cd[0] * (ekf->Pp[0] * Cd[0] +
                 ekf->Pp[1] * Cd[1] +
                 ekf->Pp[2] * Cd[2]) +

        Cd[1] * (ekf->Pp[3] * Cd[0] +
                 ekf->Pp[4] * Cd[1] +
                 ekf->Pp[5] * Cd[2]) +

        Cd[2] * (ekf->Pp[6] * Cd[0] +
                 ekf->Pp[7] * Cd[1] +
                 ekf->Pp[8] * Cd[2]) +

        R_base;

	//K=P_p*Cd'/S;
    for (int i = 0; i < 3; i++)
    {
        float temp =
            ekf->Pp[i * 3 + 0] * Cd[0] +
            ekf->Pp[i * 3 + 1] * Cd[1] +
            ekf->Pp[i * 3 + 2] * Cd[2];

        ekf->K[i] = temp / S;
    }

	//limit dx=K*delta_y
	float dx[3];
    for (int i = 0; i < 3; i++)
    {
        dx[i] = ekf->K[i] * delta_y;
    }

	float max_step=fabsf(curr)*deltaT/Cb*5.0f;
	if(dx[0]>max_step) dx[0]=max_step;
	if(dx[0]<-max_step) dx[0]=-max_step;

	//x_k1=x_p+dx;
    for (int i = 0; i < 3; i++)
    {
        ekf->x[i] = ekf->x_p[i] + dx[i];
    }

    if (ekf->x[0] < 0.0f)
        ekf->x[0] = 0.0f;

    if (ekf->x[0] > 1.0f)
        ekf->x[0] = 1.0f;




	// temp = Cd * Pp
    for (int i = 0; i < 3; i++)
    {
        ekf->temp_vec[i] =
            Cd[0] * ekf->Pp[i] +
            Cd[1] * ekf->Pp[3 + i] +
            Cd[2] * ekf->Pp[6 + i];
    }

	// P = Pp - K * temp
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            ekf->P[i * 3 + j] =
                ekf->Pp[i * 3 + j] -
                ekf->K[i] * ekf->temp_vec[j];
        }
    }
    ekf->y_k1 =
        OCV +
        ekf->x[1] +
        ekf->x[2] +
        Ri * curr;


}

void EKF_Init(BatteryEKF_t *ekf)
{
    /*
     * Initial state
     */
    ekf->x[0] = 1.0f;
    ekf->x[1] = 0.0f;
    ekf->x[2] = 0.0f;


    /*
     * Initial covariance P
     */
    ekf->P[0] = 1e-4f;
    ekf->P[1] = 0.0f;
    ekf->P[2] = 0.0f;

    ekf->P[3] = 0.0f;
    ekf->P[4] = 1e-3f;
    ekf->P[5] = 0.0f;

    ekf->P[6] = 0.0f;
    ekf->P[7] = 0.0f;
    ekf->P[8] = 1e-3f;


    /*
     * Clear prediction arrays
     */
    for (int i = 0; i < 3; i++)
    {
        ekf->x_p[i] = 0.0f;
        ekf->K[i] = 0.0f;
        ekf->temp_vec[i] = 0.0f;
    }

    for (int i = 0; i < 9; i++)
    {
        ekf->Pp[i] = 0.0f;
    }

    ekf->V_rest = 0.0f;
    ekf->rest_voltage_valid= 0U;

    ekf->I_prev = 0.0f;

    ekf->last_soc = 1.0f;
    ekf->y_k1 = 0.0f;

    ekf->soc_initialized = 0;
    ekf->first_sample = 1;
}

static float EKF_EstimateInitialSOCFromStep(BatteryEKF_t *ekf, float volt, float curr, float soh){
	float best_soc = 0.0f;
	float best_error = 1000.0f;
	float measured_dv = volt - ekf->V_rest;

	for(uint16_t i=0U; i<=1000U; i++){
		float soc = (float)i/1000.0f;
		float ocv_candidate;
		float ri_candidate;

		if(curr >= 0.0f){
			ocv_candidate= lookup2D_nearestSOH(&charge_OCV, soc, soh);
			ri_candidate= lookup2D_nearestSOH(&charge_Ri, soc, soh);
		}
		else{
            ocv_candidate =
                lookup2D_nearestSOH(&discharge_OCV, soc, soh);

            ri_candidate =
                lookup2D_nearestSOH(&discharge_Ri, soc, soh);

		}
        /*
         * At the instant current is applied:
         *
         * V_load - V_rest ≈ Ri * I
         */
        float predicted_dv = ri_candidate * curr;
        /*
         * Error 1:
         * Does ECM OCV at this SOC match
         * the measured rest voltage?
         */
        float error_ocv =
            fabsf(ekf->V_rest - ocv_candidate);

        /*
         * Error 2:
         * Does ECM Ri at this SOC reproduce
         * the measured instantaneous voltage step?
         */
        float error_drop =
            fabsf(measured_dv - predicted_dv);

        float total_error =
            error_ocv + error_drop;

        if (total_error < best_error)
        {
            best_error = total_error;
            best_soc = soc;
        }
    }

    return best_soc;
}

void EKF_Process(BatteryEKF_t *ekf, float volt, float curr, float deltaT)
//    if (ekf->first_sample)
//    {
//        ekf->I_prev = curr;
//        if(fabsf(curr) < 0.01f){
//            		ekf->V_rest = volt;
//            		ekf->rest_voltage_valid =1U;
//            	}
//        ekf->first_sample = 0;
//        return;
//    }
//
//    if(!ekf->soc_initialized){
//    	if(fabsf(curr) < 0.01f){
//    		ekf->V_rest = volt;
//    		ekf->rest_voltage_valid =1U;
//    	}
//    }
//    /*
//     * Initial SOC estimation
//     */
//    if (!ekf->soc_initialized)
//    {
//        float soc_candidate =
//            init_soc(&charge_OCV,
//                     volt,
//                     last_soh);
//
//
//        /*
//         * Detect transition from approximately zero current
//         * to charge/discharge current.
//         */
//        if ((fabsf(ekf->I_prev) < 0.01f) &&
//            (fabsf(curr) >= 0.01f))
//        {
//            float Ri_candidate;
//
//
//            if (curr >= 0.0f)
//            {
//                Ri_candidate = lookup2D_nearestSOH(&charge_Ri,soc_candidate,last_soh);
//                float OCV_candidate = volt - Ri_candidate * curr;
//                soc_candidate = init_soc(&charge_OCV, OCV_candidate,last_soh);
//            }
//            else
//            {
//                Ri_candidate = lookup2D_nearestSOH( &discharge_Ri, soc_candidate, last_soh);
//                float OCV_candidate = volt - Ri_candidate * curr;
//                soc_candidate = init_soc(&charge_OCV, OCV_candidate,last_soh);
//            }
//            ekf->x[0] = soc_candidate;
//            ekf->soc_initialized = 1;
//        }
//
//
//        ekf->I_prev = curr;
//        ekf->last_soc = soc_candidate;
	{
	    if (ekf->first_sample)
	    {
	        ekf->I_prev = curr;

	        if (fabsf(curr) < 0.01f)
	        {
	            ekf->V_rest = volt;
	            ekf->rest_voltage_valid = 1U;
	        }

	        ekf->first_sample = 0U;
	        return;
	    }

	    if (!ekf->soc_initialized)
	    {
	        float soc_candidate =
	            init_soc(&charge_OCV, volt, last_soh);

	        if (fabsf(curr) < 0.01f)
	        {
	            ekf->V_rest = volt;
	            ekf->rest_voltage_valid = 1U;
	        }

	        if ((fabsf(ekf->I_prev) < 0.01f) &&
	            (fabsf(curr) >= 0.01f) &&
	            (ekf->rest_voltage_valid == 1U))
	        {
	            soc_candidate =
	                EKF_EstimateInitialSOCFromStep(
	                    ekf,
	                    volt,
	                    curr,
	                    last_soh);

	            ekf->x[0] = soc_candidate;
	            ekf->x[1] = 0.0f;
	            ekf->x[2] = 0.0f;

	            ekf->soc_initialized = 1U;
	        }

	        ekf->I_prev = curr;
	        ekf->last_soc = soc_candidate;

	        return;
	    }

	    /* Normal EKF operation */
	    ekf_run(ekf, volt, curr, deltaT);

	    ekf->last_soc = ekf->x[0];
	    ekf->I_prev = curr;
	}

float EKF_GetSOC(const BatteryEKF_t *ekf)
{
    return ekf->last_soc;
}
