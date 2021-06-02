/*
 * FIR.c
 *
 *  Created on: May 29, 2021
 *      Author: ib119
 */

#include "FIR.h"

int SXPP_FIR_update(SXPP_FIR* fir, int value){
	fir->vals[fir->next] = value;
	long out = 0;
	for(int i = 0; i < fir->size; i++){
		int vi = (fir->next + i)%fir->size;
		out += fir->vals[vi] * fir->coeff[i];
//		printf(" (%d: < + %d*%d = %d) ", i, fir->vals[vi], fir->coeff[i], out);
	}
//	printf("\n");
	fir->next = (fir->next+fir->size-1)%fir->size;
	out /= 10000;
	return out;
}
