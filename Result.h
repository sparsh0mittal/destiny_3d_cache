//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef RESULT_H_
#define RESULT_H_

#include "BankWithHtree.h"
#include "BankWithoutHtree.h"
#include "Wire.h"

class Result {
public:
	Result();
	virtual ~Result();

	/* Functions */
	void print(int indent = 0);
	void printAsCache(Result &tagBank, CacheAccessMode cacheAccessMode);
	void reset();
    bool nearReference(double reference, double measured);
	void printToCsvFile(ofstream &outputFile);
	void printAsCacheToCsvFile(Result &tagBank, CacheAccessMode cacheAccessMode, ofstream &outputFile);
	bool compareAndUpdate(Result &newResult);
    string printOptimizationTarget();

	OptimizationTarget optimizationTarget;	/* Exploration should not be assigned here */

	Bank * bank;
	Wire * localWire;		/* TO-DO: this one has the same name as one of the global variables */
	Wire * globalWire;

	double limitReadLatency;			/* The maximum allowable read latency, Unit: s */
	double limitWriteLatency;			/* The maximum allowable write latency, Unit: s */
	double limitReadDynamicEnergy;		/* The maximum allowable read dynamic energy, Unit: J */
	double limitWriteDynamicEnergy;		/* The maximum allowable write dynamic energy, Unit: J */
	double limitReadEdp;				/* The maximum allowable read EDP, Unit: s-J */
	double limitWriteEdp;				/* The maximum allowable write EDP, Unit: s-J */
	double limitArea;					/* The maximum allowable area, Unit: m^2 */
	double limitLeakage;				/* The maximum allowable leakage power, Unit: W */
    MemCell *cellTech;
};

#endif /* RESULT_H_ */
