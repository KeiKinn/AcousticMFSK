/*
 * hello_lfm_func.c
 *
 *  Created on: 2019-1-7
 *      Author: JingX
 */
#include "module.h"
#define SW_BREAKPOINT     asm(" SWBP 0 ");

extern float TwiddleCoff[cFFT_NUM];
extern unsigned char Brev[64];

void genLFM(float *ptrDesArray)
{
	/*
	 * 产生起始频率为LFM_FRES的LFM信号
	 * 因为CCS中的FFT均为复数FFT
	 * 故产生的信号格式为偶数项为数据，奇数项为0
	 */
	float theta, coff;
	coff = (LFM_FREE - LFM_FRES) / LFM_LENGTH * SAMPLE_RATE;
	int counter;
	for(counter = 0; counter < LFM_LENGTH; counter++)
	{
		//			theta = 2 * PI * LFM_FRES * counter / 100000;
		theta =  2 * PI * (LFM_FRES + 0.5 * coff * counter / SAMPLE_RATE) * counter / SAMPLE_RATE;
		ptrDesArray[2 * counter] = cos(theta);
	}
}


void calcuTwiddle(float *w, int n)
{
    int i,j,k;
    double theta1,theta2,theta3;

    for(j = 1, k = 0; j<=n>>2; j = j<<2)
    {
            for(i = 0; i <n>>2;i += j)
            {
                    theta1 = 2 * PI * i / n;
                    w[k]     = (float)cos(theta1);
                    w[k + 1] = (float)sin(theta1);

                    theta2 = 4 * PI * i / n;
                    w[k + 2] = (float)cos(theta2);
                    w[k + 3] = (float)sin(theta2);

                    theta3 = 6 * PI * i / n;
                    w[k + 4] = (float)cos(theta3);
                    w[k + 5] = (float)sin(theta3);
                    k += 6;
            }
    }

}

void xCorr(const float *ptrSigy, const float *ptrFFTx, float *ptrCorr)
{
	/*
	 * 互相关运算
	 * 将接收信号ptrSigy与本地信号ptrSigx做互相关，获取相关峰
	 * 使用FFT运算方法，注意FFT函数随芯片变化而变化，注意适配
	 * 对本地信号做FFT后取共轭，这样运算的结果为ptrSigy相对ptrSigx的时延
	 */
	float sigy_temp[cFFT_NUM];
	float fft_sigx[cFFT_NUM], fft_sigy[cFFT_NUM], xcorr_temp[cFFT_NUM];
	int counter;

	for(counter = 0; counter < cFFT_NUM; counter++)
	{
		fft_sigx[counter] = ptrFFTx[counter];
		sigy_temp[counter] = ptrSigy[counter];
	}
	DSPF_sp_fftSPxSP(FFT_NUM, sigy_temp, TwiddleCoff, fft_sigy, Brev, 4, 0, FFT_NUM);
//	float real_fft_sigy[FFT_NUM];
//	cmplx2real(real_fft_sigy, fft_sigy);
	for(counter = 0; counter < FFT_NUM; counter++)
	{
		/* FFT之后，求FFT_x的共轭与FFT_y的乘积
		 * x = a + jb; y = c + jd
		 * conj(x).*y = (ac + bd) + j(ad - bc)
		 */
		float a, b, c, d;
		a = fft_sigx[2 * counter];
		b = fft_sigx[2 * counter +  1];
		c = fft_sigy[2 * counter];
		d = fft_sigy[2 * counter + 1];

		xcorr_temp[2 * counter] = a*c + b*d;
		xcorr_temp[2 * counter + 1] = a*d - b*c;
	}
//	float real_fft_sigy[FFT_NUM];
//	cmplx2real(real_fft_sigy, xcorr_temp);
	DSPF_sp_ifftSPxSP(FFT_NUM, xcorr_temp, TwiddleCoff, ptrCorr,  Brev, 4, 0, FFT_NUM);

	/// --- xcorr前半段与后半段交换 ---//
	RightShift(ptrCorr, cFFT_NUM, FFT_NUM);
}

void cmplx2real(float *real, float *cmplx)
{
	/* 复数转实数操作
	 * 比如将复数fft转换为实数fft，方便画图确定是否运算正确。
	 */
	int i;
	for(i = 0; i < FFT_NUM; i++)
	{
		real[i] = cmplx[2 * i] * cmplx[2 * i] + cmplx[2 * i + 1] * cmplx[2 * i + 1];
	}

}

void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize)
{
    /* 最大值找寻
     * 函数接收3个参数：
     *      ptrMax 为 maxStruct 结构体指针
     *      ptrData 为目标数组指针
     *      ArraySize 为目标数组大小
     * 函数找寻ptrData中的最大值，并将最大值Value及Location存入ptrMax中
     */
    int counter = 0;
    ptrMax->Val = -1;
    ptrMax->Loc = -1;

    for (counter = 0; counter < ArraySize; counter ++)
    {
        if(ptrMax->Val < ptrData[counter])
        {
            ptrMax->Val = ptrData[counter];
            ptrMax->Loc = counter;
        }
    }
}

int isPeak(float ptrMaxVal, float *ptrData, int ArraySize)
{
    int bigger_counter = 0;
    int counter = 0;
    ptrMaxVal = ptrMaxVal / 3;
    for(counter; counter < ArraySize; counter++)
    {
        if(ptrMaxVal < ptrData[counter])
        {
            bigger_counter++;
        }
    }

    if(bigger_counter < 20 && bigger_counter > 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int LFMsp(int *ptrLoc, const int ptrFlag, const int ArraySize)
{
/*
 * 函数用于返回LFM信号的起始位置
 * 函数假设信号一定是滑入待处理信号段的
 * 对于待处理段起始部分包含了部分信号的情况并未纳入考虑
 */
    int counter = ptrFlag;
    for(counter; counter < ArraySize; counter++)
    {
        ;
    }

    return 1;
}

void RightShift(float *arr, int N, int K)
{
    K = K % N ;
    Reverse(arr, 0, N-K-1);     //前面N-K部分逆序
    Reverse(arr, N-K, N-1);     //后面K部分逆序
    Reverse(arr, 0, N-1);       //全部逆序
}

void Reverse(float *arr, int b, int e)      //逆序排列
{
    for( ; b < e; b++, e--)    //从数组的前、后一起遍历
    {
        float temp = arr[e];
        arr[e] = arr[b];
        arr[b] = temp;
    }
}
