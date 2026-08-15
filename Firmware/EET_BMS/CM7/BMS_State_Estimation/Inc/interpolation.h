/*
 * interpolation.h
 *
 *  Created on: Oct 17, 2025
 *      Author: CQN
 */

#ifndef INC_INTERPOLATION_H_
#define INC_INTERPOLATION_H_

#include <stdint.h>
#include "ECM_parameter.h"

typedef struct {
    const float *soc;
    const float *soh;
    const float (*table)[100];
    uint16_t Ns;
    uint16_t Nsoh;
} LookupTable2D;

float lookup2D_nearestSOH(const LookupTable2D *table, float soc, float soh);

float init_soc(const LookupTable2D *table,float u,float soh);

extern LookupTable2D charge_Ri;
extern LookupTable2D charge_R1;
extern LookupTable2D charge_R2;
extern LookupTable2D charge_tau1;
extern LookupTable2D charge_tau2;
extern LookupTable2D charge_OCV;
extern LookupTable2D charge_dOCV;

extern LookupTable2D discharge_Ri;
extern LookupTable2D discharge_R1;
extern LookupTable2D discharge_R2;
extern LookupTable2D discharge_tau1;
extern LookupTable2D discharge_tau2;
extern LookupTable2D discharge_OCV;
extern LookupTable2D discharge_dOCV;

#endif /* INC_INTERPOLATION_H_ */
