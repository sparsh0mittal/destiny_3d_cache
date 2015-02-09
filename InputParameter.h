//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef INPUTPARAMETER_H_
#define INPUTPARAMETER_H_

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include "typedef.h"

using namespace std;

class InputParameter {
public:
	InputParameter();
	virtual ~InputParameter();

	/* Functions */
	void ReadInputParameterFromFile(const std::string & inputFile);
	void PrintInputParameter();

	/* Properties */
	DesignTarget designTarget;		/* Cache, RAM, or CAM */
	OptimizationTarget optimizationTarget;	/* Either read latency, write latency, read energy, write energy, leakage, or area */
	int processNode;				/* Process node (nm) */
	int64_t capacity;				/* Memory/cache capacity, Unit: Byte */
	long wordWidth;					/* The width of each input/output word, Unit: bit */
	DeviceRoadmap deviceRoadmap;	/* ITRS roadmap: HP, LSTP, or LOP */
	vector<string> fileMemCell;		/* Input file name of memory cell type */
	int temperature;				/* The ambient temperature, Unit: K */
	double maxDriverCurrent;        /* The maximum driving current that the wordline/bitline driver can provide */
	WriteScheme writeScheme;		/* The write scheme */
	double readLatencyConstraint;	/* The allowed variation to the best read latency */
	double writeLatencyConstraint;	/* The allowed variation to the best write latency */
	double readDynamicEnergyConstraint;		/* The allowed variation to the best read dynamic energy */
	double writeDynamicEnergyConstraint;	/* The allowed variation to the best write dynamic energy */
	double leakageConstraint;		/* The allowed variation to the best leakage energy */
	double areaConstraint;			/* The allowed variation to the best leakage energy */
	double readEdpConstraint;		/* The allowed variation to the best read EDP */
	double writeEdpConstraint;		/* The allowed variation to the best write EDP */
	bool isConstraintApplied;		/* If any design constraint is applied */
	bool isPruningEnabled;			/* Whether to prune the results during the exploration */
	bool useCactiAssumption;		/* Use the CACTI assumptions on the array organization */

	int associativity;				/* Associativity, for cache design only */
	CacheAccessMode cacheAccessMode;	/* Access mode (for cache only) : normal, sequential, fast */

	long pageSize;					/* Unit: bit, For DRAM and NAND flash memory only */
	long flashBlockSize;				/* Unit: bit, For NAND flash memory only */

	RoutingMode routingMode;
	bool internalSensing;

	double maxNmosSize;				/* Default value is MAX_NMOS_SIZE in constant.h, however, user might change it, Unit: F */

	string outputFilePrefix;

	int minNumRowMat;
	int maxNumRowMat;
	int minNumColumnMat;
	int maxNumColumnMat;
	int minNumActiveMatPerRow;
	int maxNumActiveMatPerRow;
	int minNumActiveMatPerColumn;
	int maxNumActiveMatPerColumn;
	int minNumRowSubarray;
	int maxNumRowSubarray;
	int minNumColumnSubarray;
	int maxNumColumnSubarray;
	int minNumActiveSubarrayPerRow;
	int maxNumActiveSubarrayPerRow;
	int minNumActiveSubarrayPerColumn;
	int maxNumActiveSubarrayPerColumn;
	int minMuxSenseAmp;
	int maxMuxSenseAmp;
	int minMuxOutputLev1;
	int maxMuxOutputLev1;
	int minMuxOutputLev2;
	int maxMuxOutputLev2;
	int minNumRowPerSet;
	int maxNumRowPerSet;
	int minAreaOptimizationLevel;	/* This one is actually OptPriority type */
	int maxAreaOptimizationLevel;	/* This one is actually OptPriority type */
	int minLocalWireType;			/* This one is actually WireType type */
	int maxLocalWireType;			/* This one is actually WireType type */
	int minGlobalWireType;			/* This one is actually WireType type */
	int maxGlobalWireType;			/* This one is actually WireType type */
	int minLocalWireRepeaterType;		/* This one is actually WireRepeaterType type */
	int maxLocalWireRepeaterType;		/* This one is actually WireRepeaterType type */
	int minGlobalWireRepeaterType;		/* This one is actually WireRepeaterType type */
	int maxGlobalWireRepeaterType;		/* This one is actually WireRepeaterType type */
	int minIsLocalWireLowSwing;		/* This one is actually boolean */
	int maxIsLocalWireLowSwing;		/* This one is actually boolean */
	int minIsGlobalWireLowSwing;		/* This one is actually boolean */
	int maxIsGlobalWireLowSwing;		/* This one is actually boolean */

    int partitionGranularity;
    int localTsvProjection;  // 0 -- ITRS, 1 -- Industry Reported
    int globalTsvProjection; // 0 -- ITRS, 1 -- Industry Reported
    double tsvRedundancy; 

    int monolithicStackCount; // 1 or 2 valid (2 for HRRAM-style design)

    int minStackLayer;
    int maxStackLayer;
    bool forcedStackLayers;

    bool doublePrune;
    bool printAllOptimals;
    bool allowDifferentTagTech;

    int printLevel;
};

#endif /* INPUTPARAMETER_H_ */
