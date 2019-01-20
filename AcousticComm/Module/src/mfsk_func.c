#include <math.h>
#include "lfm.h"
#include "mfsk.h"
#include "common.h"

void Normalize(float *ptrData, int len)
{
    /*
     * 归一化函数
     * 将接收信号归一化
     * 函数接受两个参数，
     *      ptrData为数据段指针，
     *      len为数据段长度
     * 函数依赖函数maxValue
     */
	int counter = 0;
	maxStruct maxval;
	maxValue(&maxval, ptrData, len);
	while(counter < len)
	{
		ptrData[counter] = ptrData[counter] / maxval.Val * 2;
		counter++;
	}
}

void genNonCoherentDemodWave(NCDW *ptrData, int freq, int timelen, int fs)
{
    /*
     * 产生相干信号
     * 该方法是针对FSK各个频率产生相干信号，通过比较相关幅值大小
     * 来确定信息位
     *
     * 函数接收4个参数
     *      结构体指针ptrData为函数产生的信号，其为复信号
     *      freq：        信号频率
     *      timelen： 信号的持续时间，在数字领域即信号的采样点数
     *      fs：            采样频率
     */
	int counter = 0;
	float theta;
	while(counter < timelen)
	{
		theta = 2 * PI * freq * counter / fs;
		ptrData->deSin[2 * counter] = sin(theta);
		ptrData->deCos[2 * counter] = cos(theta);
		counter++;
	}
	//	SW_BREAKPOINT ;
}

float SquareLawDetection(float * data, const NCDW * decodesig, int ArraySize)
{
    /*
     * 包络检波
     * 函数接收三个参数
     *      数组指针data： 需要解调的fsk信号段，长度为一个码元长度
     *      结构体指针decodesig：非相干解调信号，与data同频同长度的sin，cos信号
     *      整形ArraySize： 数组长度
     */
	int counter = 0;
	float desintemp = 0, decostemp = 0, powtemp = 0;
	while(counter < ArraySize)
	{
		decostemp = data[counter] * decodesig->deCos[counter] + decostemp;
		desintemp = data[counter] * decodesig->deSin[counter] + desintemp;
		counter++;
	}

	powtemp = decostemp * decostemp + desintemp * desintemp;
	return powtemp;
}

void bin2hex(char* ptrHex, const int* ptrData, int ArraySize)
{
    /*
     * 2进制转16进制
     * 函数接收正序输入的2进制数组指针 ptrData，
     * 即数组下标越低，其阶数越高，每4位转换为
     * 16进制字符数组ptrHex；
     *
     * ArraySize 为ptrData指向的数组的大小
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
