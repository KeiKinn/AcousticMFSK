/*
 * hello_lfm.h
 *
 *  Created on: 2019-1-4
 *      Author: JingX
 */

#ifndef HELLO_LFM_H_
#define HELLO_LFM_H_

#include <math.h>
#include "dsplib.h"

#define LFM_FRES     10000
#define LFM_FREE     20000
#define SAMPLE_RATE  100000
#define LFM_LENGTH   1500
#define TEST_LENGTH  4096
#define PI           3.1415926
#define FFT_NUM      4096
#define cFFT_NUM     2 * FFT_NUM

typedef struct
{
    float Val;
    int Loc;
}maxStruct;

void initProcessData(void);
void calcuTwiddle(float *w, int n);
void genLFM(float *ptrDesArray);
void cmplx2real(float *real, float *cmplx);
void xCorr(const float *ptrSigy,const float *ptrSigx, float *ptrCorr);
void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize);


void genTestLFM(float *ptrTest, int offset);
void Reverse(float *arr, int b, int e);
void RightShift(float *arr, int N, int K);


#endif /* HELLO_LFM_H_ */
