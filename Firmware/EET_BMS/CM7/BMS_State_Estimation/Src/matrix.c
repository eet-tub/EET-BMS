/*
 * matrix.c
 *
 *  Created on: May 4, 2026
 *      Author: CQN
 */

#include "matrix.h"

// C = A * B (3x3)
void mat3_mul(float A[9], float B[9], float C[9])
{
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            C[i*3+j] = 0;
            for(int k=0;k<3;k++){
                C[i*3+j] += A[i*3+k] * B[k*3+j];
            }
        }
    }
}

// C = A + B
void mat3_add(float A[9], float B[9], float C[9])
{
    for(int i=0;i<9;i++){
        C[i] = A[i] + B[i];
    }
}

// AT = transpose(A)
void mat3_trans(float A[9], float AT[9])
{
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            AT[j*3+i] = A[i*3+j];
        }
    }
}

// y = A * x
void mat3_mul_vec(float A[9], float x[3], float y[3])
{
    for(int i=0;i<3;i++){
        y[i] =
            A[i*3+0]*x[0] +
            A[i*3+1]*x[1] +
            A[i*3+2]*x[2];
    }
}
