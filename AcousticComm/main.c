#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mathlib.h"                // DSP 数学函数库
#include "dsplib.h"                 // DSP 函数库
#include "module.h"

#define SW_BREAKPOINT     asm(" SWBP 0 ");

#pragma DATA_ALIGN (Brev, 16)
#pragma DATA_ALIGN (TwiddleCoff, 16)
#pragma DATA_ALIGN (lfm_local, 16)

unsigned char Brev[64] = {
        0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
        0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
        0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
        0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
        0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
        0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
        0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
        0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

float lfm_local[cFFT_NUM]; // 复数信号，虚部为0
float TwiddleCoff[cFFT_NUM]; // FFT 运算的旋转因子
float signal_rec[cFFT_NUM]; // 接收信号

int main(void)
{

    float lfm_local_temp[cFFT_NUM], fft_lfm_local[cFFT_NUM];
    float xcorr[cFFT_NUM];
    maxStruct maxval;

    initProcessData();
    genTestLFM(signal_rec, 400);
    genLFM(lfm_local);
    memcpy((char *)&lfm_local_temp, (char *)&lfm_local, sizeof(lfm_local));
    DSPF_sp_fftSPxSP(FFT_NUM, lfm_local_temp,  TwiddleCoff, fft_lfm_local, Brev, 4, 0, FFT_NUM);
    xCorr(signal_rec, fft_lfm_local, xcorr);
    maxValue(&maxval, xcorr, cFFT_NUM);

    SW_BREAKPOINT;
    return 0;
}

void initProcessData(void)
{
    memset((void *)lfm_local,        0, sizeof(lfm_local));
    memset((void *)TwiddleCoff,      0, sizeof(TwiddleCoff));
    memset((void *)signal_rec,       0, sizeof(signal_rec));

    calcuTwiddle(TwiddleCoff, FFT_NUM);
}
