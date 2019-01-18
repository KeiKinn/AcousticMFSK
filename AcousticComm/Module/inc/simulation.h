/*
 * simulation.h
 *
 *  Created on: 2019Äê1ÔÂ14ÈÕ
 *      Author: JingX
 */

#ifndef MODULE_INC_SIMULATION_H_
#define MODULE_INC_SIMULATION_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "module.h"

void readADC(float *ptrData, FILE *fp, int ArraySize);
void genTestLFM(float *ptrTest, int offset);

#endif /* MODULE_INC_SIMULATION_H_ */
