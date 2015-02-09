//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef BANKWITHHTREE_H_
#define BANKWITHHTREE_H_

#include "Bank.h"

class BankWithHtree: public Bank {
public:
	BankWithHtree();
	virtual ~BankWithHtree();
	void Initialize(int _numRowMat, int _numColumnMat, long long _capacity,
			long _blockSize, int _associativity, int _numRowPerSet, int _numActiveMatPerRow,
			int _numActiveMatPerColumn, int _muxSenseAmp, bool _internalSenseAmp, int _muxOutputLev1, int _muxOutputLev2,
			int _numRowSubarray, int _numColumnSubarray,
			int _numActiveSubarrayPerRow, int _numActiveSubarrayPerColumn,
			BufferDesignTarget _areaOptimizationLevel, MemoryType _memoryType,
            int _stackedDieCount, int _partitionGranularity, int monolithicStackCount);
	void CalculateArea();
	void CalculateRC();
	void CalculateLatencyAndPower();
	BankWithHtree & operator=(const BankWithHtree &);

	int numAddressBit;		/* Number of bank address bits */
	int numDataDistributeBit;	/* Number of bank data bits (these bits will be distributed along with the address) */
	int numDataBroadcastBit;	/* Number of bank data bits (these bits will be broadcasted at every node) */

	int levelHorizontal;			/* The number of horizontal levels */
	int levelVertical;				/* The number of vertical levels */
	int * numHorizontalAddressBitToRoute;  /* The number of horizontal bits to route on level x */
	int * numHorizontalDataDistributeBitToRoute;	/* The number of horizontal data-in bits to route on level x */
	int * numHorizontalDataBroadcastBitToRoute;		/* The number of horizontal data-out bits to route on level x */
	int * numHorizontalWire;        /* The number of horizontal wire tiers on level x */
	int * numSumHorizontalWire;     /* The number of total horizontal wire groups on level x */
	int * numActiveHorizontalWire;  /* The number of active horizontal wire groups on level x */
	double * lengthHorizontalWire;	/* The length of horizontal wires on level x, Unit: m */
	int * numVerticalAddressBitToRoute;	/* The number of vertical address bits to route on level x */
	int * numVerticalDataDistributeBitToRoute;	/* The number of vertical data-in bits to route on level x */
	int * numVerticalDataBroadcastBitToRoute;	/* The number of vertical data-out bits to route on level x */
	int * numVerticalWire;          /* The number of vertical wire tiers on level x */
	int * numSumVerticalWire;       /* The number of total vertical wire groups on level x */
    int * numActiveVerticalWire;    /* The number of active vertical wire groups on level x */
	double * lengthVerticalWire;	/* The length of vertical wires on level x, Unit: m */

};

#endif /* BANKWITHHTREE_H_ */
