#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "mathlib.h"                // DSP 数学函数库
#include "dsplib.h"                 // DSP 函数库
#include "common.h"
#include "lfm.h"
#include "simulation.h"
#include "mfsk.h"

FILE *fp = NULL;
#define FILEPATH  "E:\\CCSGit\\Data\\LFMTest.bin"

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
float DataBuffer[cFFT_NUM]; // 滑动窗数据
float signal_rec[cFFT_NUM]; // 接收信号
int   lfm_sp;
NCDW  baseband[QUAD];

int main(void)
{

    float lfm_local_temp[cFFT_NUM], fft_lfm_local[cFFT_NUM];
    float xcorr[cFFT_NUM];
    maxStruct maxval;

    initProcessData();
    //    initDemodFsk();
    //    genTestLFM(signal_rec, 200);
    genLFM(lfm_local);
    memcpy((char *)&lfm_local_temp, (char *)&lfm_local, sizeof(lfm_local));
    DSPF_sp_fftSPxSP(FFT_NUM, lfm_local_temp,  TwiddleCoff, fft_lfm_local, Brev, 4, 0, FFT_NUM);
    readADC(DataBuffer, fp, FFT_NUM);

    int counter, c_num;
    int data_size = DATA_LEN * sizeof(float); // 滑动窗长度
    int fsk_datasize = 2 * SAMPLE_PER_SYMBLE * sizeof(float); // fsk一个码元的数据长度
    c_num = (cFFT_NUM - DATA_LEN)/ SLIDER_LEN;

    for(counter = 0; counter < 2; counter++)
    {
        int slider_counter = 0, sp = 0, invalid_counter = 0;
        int flag;
        int *sploc;
        sploc = malloc(c_num * sizeof(int));
        if(sploc == NULL)
        {
            printf("Out of Memory\n");
            SW_BREAKPOINT;
        }

        for(slider_counter; slider_counter < c_num; slider_counter++)
        {
            flag = 0;
            sp = slider_counter * SLIDER_LEN;
            memcpy((char *)&signal_rec, (char *)&DataBuffer[sp], data_size);

            xCorr(signal_rec, fft_lfm_local, xcorr);
            maxValue(&maxval, xcorr, cFFT_NUM);
            flag = isPeak(maxval.Val, xcorr, cFFT_NUM);
            if(flag == 1)
            {
                sploc[slider_counter] = maxval.Loc - FFT_NUM + sp;
            }
            else
            {
                sploc[slider_counter] = cFFT_NUM + 1;
                invalid_counter++;
            }
        }

        if(invalid_counter > 2)
        {
            lfm_sp = cFFT_NUM + 1;
            readADC(DataBuffer, fp, FFT_NUM);
            free(sploc);
        }
        else
        {
            //            LFMsp(sploc, invalid_counter, c_num);
            lfm_sp = sploc[counter];
            free(sploc);
        }

        // FSK 解调
        if(lfm_sp != (cFFT_NUM + 1))
        {
            int fsk_sp, counter = 0;
            readADC(DataBuffer, fp, FFT_NUM);
            fsk_sp = lfm_sp + 2 * (LFM_LENGTH + INTERVAL) - FFT_NUM;

            float fsk_data[2 * SAMPLE_PER_SYMBLE];
            float decisionvector[QUAD];
            maxStruct output;
            int data_in_bin[2 * SYMBOL_NUM];

            while(counter < SECTION_NUM)
            {
                memcpy((char *)&fsk_data,   (char *)&DataBuffer[fsk_sp], fsk_datasize);
                int i = 0;
                for(i; i < QUAD; i++)
                {
                    decisionvector[i] = SquareLawDetection(fsk_data, &baseband[i], 2 * SAMPLE_PER_SYMBLE);
                }

                maxValue(&output, decisionvector, QUAD);

                switch(output.Loc)
                {
                case 0:
                    data_in_bin[2 * (SECTION_NUM - 1 - counter)]      = 0;
                    data_in_bin[2 * (SECTION_NUM - 1 - counter) + 1 ] = 0;
                    break;
                case 1:
                    data_in_bin[2 * (SECTION_NUM - 1 - counter)]      = 0;
                    data_in_bin[2 * (SECTION_NUM - 1 - counter) + 1 ] = 1;
                    break;
                case 2:
                    data_in_bin[2 * (SECTION_NUM - 1 - counter)]      = 1;
                    data_in_bin[2 * (SECTION_NUM - 1 - counter) + 1 ] = 0;
                    break;
                case 3:
                    data_in_bin[2 * (SECTION_NUM - 1 - counter)]      = 1;
                    data_in_bin[2 * (SECTION_NUM - 1 - counter) + 1 ] = 1;
                    break;
                }

                fsk_sp = fsk_sp + 2 * SAMPLE_PER_SYMBLE;  // 更新fsk_sp，移向下一帧
                if((fsk_sp + 2 * SAMPLE_PER_SYMBLE) > cFFT_NUM) // 判断是否为可以完整读取一帧数据
                {
                    readADC(DataBuffer, fp, FFT_NUM);
                    fsk_sp = fsk_sp - FFT_NUM;
                }
                counter++;
            }

            char data_in_hex[SYMBOL_NUM];
            bin2hex(data_in_hex, data_in_bin, BIT_NUM);
            showData(data_in_hex, SYMBOL_NUM);
            SW_BREAKPOINT;
        }
        SW_BREAKPOINT;
    }
    SW_BREAKPOINT;
    return 0;
}


void initProcessData(void)
{
    memset((void *)lfm_local,        0, sizeof(lfm_local));
    memset((void *)TwiddleCoff,      0, sizeof(TwiddleCoff));
    memset((void *)signal_rec,       0, sizeof(signal_rec));
    memset((void *)DataBuffer,       0, sizeof(DataBuffer));
    fp = fopen(FILEPATH, "rb");
    if(fp == NULL)
    {
        printf("No Such File\n");
    }
    calcuTwiddle(TwiddleCoff, FFT_NUM);
}

void initDemodFsk(void)
{
    genNonCoherentDemodWave(&baseband[0], BASE_BAND_FREQ0, SAMPLE_PER_SYMBLE, SAMPLE_RATE);
    genNonCoherentDemodWave(&baseband[1], BASE_BAND_FREQ1, SAMPLE_PER_SYMBLE, SAMPLE_RATE);
    genNonCoherentDemodWave(&baseband[2], BASE_BAND_FREQ2, SAMPLE_PER_SYMBLE, SAMPLE_RATE);
    genNonCoherentDemodWave(&baseband[3], BASE_BAND_FREQ3, SAMPLE_PER_SYMBLE, SAMPLE_RATE);
}
