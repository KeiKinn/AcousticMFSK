/*
 * Simulation.c
 *
 *  Created on: 2019年1月14日
 *      Author: JingX
 */
#include "simulation.h"

void readADC(float *ptrDataBuffer, float *ptrData, FILE *fp, int ArraySize)
{
    /* 读取数据
     * 滑动式读取以保证数据不会中断
     * ArraySize = N
     * ptrDataBuffer是应用于程序处理的数据，长度为2N
     * ptrData 是从数据中读取到的数据，长度为N
     *
     * 核心原理：
     * 首先从数据流中读取长度为N的数据置入ptrDataBuffer
     * 然后将ptrDataBuffer尾部长度为N的数据放入头部为N的数据段中
     * 最后将ptrData放入到ptrDataBuffer的尾部
     */
    fread(ptrDataBuffer, sizeof(float), ArraySize, fp);
    memcpy((char *)&ptrData[0], (char *)&ptrData[ArraySize], ArraySize * sizeof(float));
    memcpy((char *)&ptrData[ArraySize], (char *)&ptrDataBuffer[0], ArraySize * sizeof(float));
}
