#ifndef ECM_PARAMETER_H
#define ECM_PARAMETER_H

#include <stdint.h>

typedef struct {
    float Ri[40][100];
    float R1[40][100];
    float R2[40][100];
    float tau1[40][100];
    float tau2[40][100];
    float ocv[40][100];
    float dOCV[40][100];
} para_discharge_t;

typedef struct {
    float Ri[40][100];
    float R1[40][100];
    float R2[40][100];
    float tau1[40][100];
    float tau2[40][100];
    float ocv[40][100];
    float dOCV[40][100];
} para_charge_t;

typedef struct {
    para_discharge_t para_discharge;
    para_charge_t para_charge;
    float soc[100];
    float soh[40];
} ECM_t;

extern const ECM_t ECM;

#endif
