/*
 * matrix.h
 *
 *  Created on: May 4, 2026
 *      Author: CQN
 */

#ifndef INC_MATRIX_H_
#define INC_MATRIX_H_



void mat3_mul(float A[9], float B[9], float C[9]);
void mat3_add(float A[9], float B[9], float C[9]);
void mat3_trans(float A[9], float AT[9]);
void mat3_mul_vec(float A[9], float x[3], float y[3]);



#endif /* INC_MATRIX_H_ */
