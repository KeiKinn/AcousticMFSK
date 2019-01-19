/*
 * common.h
 *
 *  Created on: 2019年1月19日
 *      Author: JingX
 *  该头文件主要存放项目中公用的预编译指令
 *  与结构体定义
 */

#ifndef MODULE_INC_COMMON_H_
#define MODULE_INC_COMMON_H_

#define PI           3.1415926
#define FFT_NUM      4096
#define cFFT_NUM     (2 * FFT_NUM)
#define SLIDER_LEN   (2 * 512)   // 滑动窗
#define DATA_LEN     (2 * 2048)  // 每次处理的数据长度
#define INTERVAL     1500        // LFM与FSK信号之间的间隔点数，实数点

typedef struct
{
    float Val;
    int Loc;
}maxStruct;

typedef struct
{
   float deSin[2 * SAMPLE_PER_SYMBLE];
   float deCos[2 * SAMPLE_PER_SYMBLE];
}NCDW;

#endif /* MODULE_INC_COMMON_H_ */
