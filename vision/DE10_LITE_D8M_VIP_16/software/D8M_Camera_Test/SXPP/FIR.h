/*
 * FIR.h
 *
 *  Created on: May 29, 2021
 *      Author: IBricchi
 */

#ifndef FIR_H_
#define FIR_H_

typedef struct{
	int* coeff;
	int* vals;
	int next;
	int size;
} SXPP_FIR;

int SXPP_FIR_update(SXPP_FIR* fir, int val);

#endif /* FIR_H_ */
