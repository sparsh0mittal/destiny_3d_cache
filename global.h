// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.

#ifndef __DESTINY_GLOBAL_H__
#define __DESTINY_GLOBAL_H__


#include "InputParameter.h"
#include "Technology.h"
#include "MemCell.h"
#include "Wire.h"

extern InputParameter *inputParameter;
extern Technology *tech;
extern Technology *devtech;
extern MemCell *cell;
extern Technology *gtech;
extern Wire *localWire;		/* The wire type of local interconnects (for example, wire in mat) */
extern Wire *globalWire;	/* The wire type of global interconnects (for example, the ones that connect mats) */
extern MemCell **sweepCells;

const double invalid_value = 1e41;
const double infinite_ramp = 1e41;

#endif

