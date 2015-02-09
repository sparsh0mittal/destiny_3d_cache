//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef BANKWITHOUTHTREE_H_
#define BANKWITHOUTHTREE_H_

#include "Bank.h"
#include "Mat.h"
#include "typedef.h"
#include "Comparator.h"

class BankWithoutHtree: public Bank {
public:
	BankWithoutHtree();
	virtual ~BankWithoutHtree();

	/* Functions */
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
	BankWithoutHtree & operator=(const BankWithoutHtree &);

	int numAddressBit;		   /* Number of bank address bits */
	int numWay;                  /* Number of way in a mat */
	int numAddressBitRouteToMat;  /* Number of address bits routed to mat */
	int numDataBitRouteToMat;   /* Number of data bits routed to mat */

	Mux	globalBitlineMux;
	SenseAmp globalSenseAmp;
	Comparator globalComparator;
};

#endif /* BANKWITHOUTHTREE_H_ */
