#include <math.h>
#include "lfm.h"
#include "mfsk.h"

void Normalize(float *ptrData, int len)
{
    /*
     * 归一化函数
     * 将接收信号归一化
     * 函数接受两个参数，
     * ptrData为数据段指针， len为数据段长度
     * 函数依赖函数maxValue
     */
	int counter = 0;
	maxStruct maxval;
	maxValue(&maxval, ptrData, SAMPLE_PER_SYMBLE);
	while(counter < len)
	{
		ptrData[counter] = ptrData[counter] / maxval.Val * 2;
		counter++;
	}
}

void genNonCoherentDemodWave(int freq, int timelen, NCDW *ptrData)
{
    /*
     * 产生相干信号
     * 该方法是针对FSK各个频率产生相干信号，通过比较相关幅值大小
     * 来确定信息位的值
     *
     * 因为采样信号是复信号，所以产生的非相干信号亦是复信号
     */
	int counter = 0;
	float theta;
	while(counter < timelen)
	{
		theta = 2 * PI * freq * counter / SAMPLE_RATE;
		ptrData->deSin[2 * counter] = sin(theta);
		ptrData->deCos[2 * counter] = cos(theta);
		counter++;
	}
	//	SW_BREAKPOINT ;
}

float SquareLawDetection(float * data, const NCDW * decodesig)
{
    /*
     * 包络检波
     * 得到相关值的包络
     */
	int counter = 0;
	float desintemp = 0, decostemp = 0, powtemp = 0;
	while(counter < SAMPLE_PER_SYMBLE)
	{
		decostemp = data[counter] * decodesig->deCos[counter] + decostemp;
		desintemp = data[counter] * decodesig->deSin[counter] + desintemp;
		counter++;
	}

	powtemp = decostemp * decostemp + desintemp * desintemp;
	return powtemp;
}

void bin2hex(const int* ptrData, char* ptrHex, int ArraySize)
{
    /*
     * 二进制转16进制
     */
	int counter, decTemp, i = 0;
	for(counter = 0; counter < ArraySize; counter = counter + 4)
	{
		decTemp = ptrData[counter + 3] + 2 * ptrData[counter + 2] + 4 * ptrData[counter + 1] + 8 * ptrData[counter];
		if(decTemp < 10)
		{
			ptrHex[i] = '0' + decTemp;
		}
		else
		{
			ptrHex[i] = 'A' + decTemp - 10;
		}
		i++;
	}
}

void showData(const char* ptrData, int ArraySize)
{
	int counter;
	printf("Code = ");
	for(counter = 0; counter < ArraySize; counter++)
		{
			printf("%c", ptrData[counter]);
		}

	printf("\n");
}
