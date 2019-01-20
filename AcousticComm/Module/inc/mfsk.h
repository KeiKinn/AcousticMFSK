/*
 * common.h
 *
 *  Created on: 2019年1月19日
 *      Author: JingX
 *
 *  该头文件主要存放FSK信号解调相关的定义
 *  与函数定义
 */
#ifndef mfsk_H_
#define mfsk_H_

#include <stdio.h>
#include "common.h"

#define SAMPLE_PER_SYMBLE 255
#define QUAD              4
#define SYMBOL_NUM        8
#define BIT_NUM          (4 * SYMBOL_NUM) // 1个码元由4位二进制组成
#define SECTION_NUM      (2 * SYMBOL_NUM) // 1个频率代表2个二进制位
#define BASE_BAND_FREQ0   10000
#define BASE_BAND_FREQ1   12000
#define BASE_BAND_FREQ2   14000
#define BASE_BAND_FREQ3   16000

typedef struct
{
   float deSin[2 * SAMPLE_PER_SYMBLE];
   float deCos[2 * SAMPLE_PER_SYMBLE];
}NCDW;

#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}

void Normalize(float *ptrData, int len);
void genNonCoherentDemodWave(NCDW *ptrData, int freq, int timelen, int fs);
float SquareLawDetection(float * data, const NCDW * decodesig, int ArraySize);
void bin2hex(char* ptrHex, const int* ptrData, int ArraySize);
void showData(const char* ptrData, int ArraySize);

#endif
