//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef FUNCTIONUNIT_H_
#define FUNCTIONUNIT_H_

#include <iostream>
#include "Technology.h"

using namespace std;

class FunctionUnit {
public:
	FunctionUnit();
	virtual ~FunctionUnit();

	/* Functions */
	virtual void PrintProperty();

	/* Properties */
	double height;		/* Unit: m */
	double width;		/* Unit: m */
	double area;		/* Unit: m^2 */
	double readLatency, writeLatency;		/* Unit: s */
	double readDynamicEnergy, writeDynamicEnergy;	/* Unit: J */
	double leakage;		/* Unit: W */

	/* Optional properties (not valid for all the memory cells */
	double setLatency, resetLatency;				/* Unit: s */
    double refreshLatency;                  /* Unit: s */
	double setDynamicEnergy, resetDynamicEnergy;	/* Unit: J */
	double cellReadEnergy, cellSetEnergy, cellResetEnergy;			/* Unit: J */
    double refreshDynamicEnergy;                    /* Unit: J */

    int logical_effort(int num_gates_min, double g, double F, double *w_n, double *w_p,
                       double C_load, double p_to_n_sz_ratio, double max_w_nmos,
                       Technology tech);
};

#endif /* FUNCTIONUNIT_H_ */
