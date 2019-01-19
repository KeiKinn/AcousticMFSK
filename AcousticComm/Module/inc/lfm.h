/*
 * hello_lfm.h
 *
 *  Created on: 2019-1-4
 *      Author: JingX
 */

#ifndef LFM_H_
#define LFM_H_

#include <math.h>
#include "dsplib.h"
#include "common.h"

#define LFM_FRES     10000
#define LFM_FREE     20000
#define SAMPLE_RATE  100000
#define LFM_LENGTH   2000

void initProcessData(void);
void calcuTwiddle(float *w, int n);
void genLFM(float *ptrDesArray);
void cmplx2real(float *real, float *cmplx);
void xCorr(const float *ptrSigy,const float *ptrSigx, float *ptrCorr);
void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize);
int  isPeak(float ptrMaxVal, float *ptrData, int ArraySize);
int  LFMsp(int *ptrLoc, const int ptrFlag, const int ArraySize);
void Reverse(float *arr, int b, int e);
void RightShift(float *arr, int N, int K);


#endif /* LFM_H_ */
