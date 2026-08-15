/*
 * interpolation.c
 *
 *  Created on: Oct 17, 2025
 *      Author: CQN
 */

#include "interpolation.h"
#include <math.h>

LookupTable2D charge_Ri={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.Ri,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D charge_R1={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.R1,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D charge_R2={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.R2,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D charge_tau1={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.tau1,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D charge_tau2={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.tau2,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D charge_OCV={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.ocv,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D charge_dOCV={
		.soc=ECM.soc,
		.soh=ECM.soh,
		.table=ECM.para_charge.dOCV,
		.Ns=sizeof(ECM.soc)/sizeof(float),
		.Nsoh=sizeof(ECM.soh)/sizeof(float)};

LookupTable2D discharge_Ri={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.Ri,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};
LookupTable2D discharge_R1={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.R1,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};
LookupTable2D discharge_R2={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.R2,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};
LookupTable2D discharge_tau1={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.tau1,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};
LookupTable2D discharge_tau2={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.tau2,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};
LookupTable2D discharge_OCV={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.ocv,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};
LookupTable2D discharge_dOCV={
		.soc = ECM.soc,
	    .soh = ECM.soh,
	    .table = ECM.para_discharge.dOCV,
	    .Ns = sizeof(ECM.soc)/sizeof(float),
	    .Nsoh = sizeof(ECM.soh)/sizeof(float)};

//x is soh
static uint16_t findNearestIndex(const float *arr, uint16_t N, float x)
{
    if (x <= arr[0]) return 0;
    if (x >= arr[N-1]) return N-1;

    uint16_t idx = 0;
    float minDiff = fabsf(x - arr[0]);

    for (uint16_t i = 1; i < N; i++) {
        float diff = fabsf(x - arr[i]);
        if (diff < minDiff) {
            minDiff = diff;
            idx = i;
        }
    }
    return idx;
}


static float interp1D(const float *arr, const float *y,
                           uint16_t Ns, float x)
{
    if (x <= arr[0])  return y[0];
    if (x >= arr[Ns-1]) return y[Ns-1];

    for (uint16_t i = 0; i < Ns-1; i++) {
        if (x >= arr[i] && x <= arr[i+1]) {
            float x0 = arr[i];
            float x1 = arr[i+1];
            float y0 = y[i];
            float y1 = y[i+1];
            float t = (x - x0) / (x1 - x0);
            return y0 + t * (y1 - y0);
        }
    }

    return y[Ns-1];
}

float init_soc(const LookupTable2D *tab,float u,float soh)
{
	const float *ocv_row=tab->table[findNearestIndex(tab->soh,tab->Nsoh,soh)];
	float soc_est;
	soc_est=interp1D(ocv_row,tab->soc,tab->Ns,u);
	if(soc_est<0.0f) soc_est=0.0f;
	if(soc_est>1.0f) soc_est=1.0f;
	return soc_est;
}
float lookup2D_nearestSOH(const LookupTable2D *tab, float soc, float soh)
{
	uint16_t k = findNearestIndex(tab->soh, tab->Nsoh, soh);
	const float *row = tab->table[k];
	return interp1D(tab->soc, row, tab->Ns, soc);

}

