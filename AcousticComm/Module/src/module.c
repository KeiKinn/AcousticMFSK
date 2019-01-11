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
//                    theta1 = 2 * PI * i / n;
//                    x_t = cos(theta1);
//                    y_t = sin(theta1);
//                    w[k] = (float)x_t;
//                    w[k + 1] = (float)y_t;
//
//                    theta2 = 4 * PI * i/n;
//                    x_t = cos(theta2);
//                    y_t = sin(theta2);
//                    w[k + 2] = (float)x_t;
//                    w[k + 3] = (float)y_t;
//
//                    theta3 = 6 * PI * i / n;
//                    x_t = cos(theta3);
//                    y_t = sin(theta3);
//                    w[k + 4] = (float)x_t;
//                    w[k + 5] = (float)y_t;
//                    k += 6;
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
/*
 * for(counter = 0; counter < FFT_NUM; counter++)
	{
		float temp;
		temp = ptrCorr[counter];
		ptrCorr[counter] = ptrCorr[counter + FFT_NUM];
		ptrCorr[counter + FFT_NUM] = temp;
	}
	*/

	RightShift(ptrCorr, cFFT_NUM, FFT_NUM);
}

void cmplx2real(float *real, float *cmplx)
{
	/*
	 * 将复数fft转换为实数fft，方便画图确定是否运算正确。
	 */
	int i;
	for(i = 0; i < FFT_NUM; i++)
	{
		real[i] = cmplx[2 * i] * cmplx[2 * i] + cmplx[2 * i + 1] * cmplx[2 * i + 1];
	}

}


void maxValue(maxStruct *ptrMax, const float *ptrData, int ArraySize)
{
    int counter = 0;
    ptrMax->Val = -1;
    ptrMax->Loc = -1;
//    while(counter < ArraySize)
//    {
//        DataTemp[counter] = * (ptrData + counter);
//        counter++;
//    }

    for (counter = 0; counter < ArraySize; counter ++)
    {
        if(ptrMax->Val < ptrData[counter])
        {
            ptrMax->Val = ptrData[counter];
            ptrMax->Loc = counter;
        }
    }

}

void genTestLFM(float *ptrTest, int offset)
{
	genLFM(ptrTest);
	RightShift(ptrTest, cFFT_NUM, offset);
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
