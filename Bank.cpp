//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#include "Bank.h"

Bank::Bank() {
	// TODO Auto-generated constructor stub
	initialized = false;
	invalid = false;
}

Bank::~Bank() {
	// TODO Auto-generated destructor stub
}

void Bank::PrintProperty() {
	cout << "Bank Properties:" << endl;
	FunctionUnit::PrintProperty();
}

Bank & Bank::operator=(const Bank &rhs) {
	height = rhs.height;
	width = rhs.width;
	area = rhs.area;
	readLatency = rhs.readLatency;
	writeLatency = rhs.writeLatency;
	readDynamicEnergy = rhs.readDynamicEnergy;
	writeDynamicEnergy = rhs.writeDynamicEnergy;
	resetLatency = rhs.resetLatency;
	setLatency = rhs.setLatency;
    refreshLatency = rhs.refreshLatency;
	resetDynamicEnergy = rhs.resetDynamicEnergy;
	setDynamicEnergy = rhs.setDynamicEnergy;
    refreshDynamicEnergy = rhs.refreshDynamicEnergy;
	cellReadEnergy = rhs.cellReadEnergy;
	cellSetEnergy = rhs.cellSetEnergy;
	cellResetEnergy = rhs.cellResetEnergy;
	leakage = rhs.leakage;
	initialized = rhs.initialized;
	invalid = rhs.invalid;
	numRowMat = rhs.numRowMat;
	numColumnMat = rhs.numColumnMat;
	capacity = rhs.capacity;
	blockSize = rhs.blockSize;
	associativity = rhs.associativity;
	numRowPerSet = rhs.numRowPerSet;
	numActiveMatPerRow = rhs.numActiveMatPerRow;
	numActiveMatPerColumn = rhs.numActiveMatPerColumn;
	muxSenseAmp = rhs.muxSenseAmp;
	internalSenseAmp = rhs.internalSenseAmp;
	muxOutputLev1 = rhs.muxOutputLev1;
	muxOutputLev2 = rhs.muxOutputLev2;
	areaOptimizationLevel = rhs.areaOptimizationLevel;
	memoryType = rhs.memoryType;
	numRowSubarray = rhs.numRowSubarray;
	numColumnSubarray = rhs.numColumnSubarray;
	numActiveSubarrayPerRow = rhs.numActiveSubarrayPerRow;
	numActiveSubarrayPerColumn = rhs.numActiveSubarrayPerColumn;
    stackedDieCount = rhs.stackedDieCount;
    partitionGranularity = rhs.partitionGranularity;
    routingReadLatency = rhs.routingReadLatency;
    routingWriteLatency = rhs.routingWriteLatency;
    routingResetLatency = rhs.routingResetLatency;
    routingSetLatency = rhs.routingSetLatency;
    routingRefreshLatency = rhs.routingRefreshLatency;
    routingReadDynamicEnergy = rhs.routingReadDynamicEnergy;
    routingWriteDynamicEnergy = rhs.routingWriteDynamicEnergy;
    routingResetDynamicEnergy = rhs.routingResetDynamicEnergy;
    routingSetDynamicEnergy = rhs.routingSetDynamicEnergy;
    routingRefreshDynamicEnergy = rhs.routingRefreshDynamicEnergy;
    routingLeakage = rhs.routingLeakage;
	mat = rhs.mat;
    tsvArray = rhs.tsvArray;
	return *this;
}
