//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef SUBARRAY_H_
#define SUBARRAY_H_

#include "FunctionUnit.h"
#include "RowDecoder.h"
#include "Precharger.h"
#include "SenseAmp.h"
#include "Mux.h"
#include "typedef.h"

class SubArray: public FunctionUnit {
public:
	SubArray();
	virtual ~SubArray();

	/* Functions */
	void PrintProperty();
	void Initialize(long long _numRow, long long _numColumn, bool _multipleRowPerSet, bool _split,
			int _muxSenseAmp, bool _internalSenseAmp, int _muxOutputLev1, int _muxOutputLev2,
			BufferDesignTarget _areaOptimizationLevel, int _num3DLevels);
	void CalculateArea();
	//void CalculateRC();
	void CalculateLatency(double _rampInput);
	void CalculatePower();
	SubArray & operator=(const SubArray &);

	/* Properties */
	bool initialized;	/* Initialization flag */
	bool invalid;		/* Indicate that the current configuration is not valid, pass down to all the sub-components */
	bool internalSenseAmp; /* Indicate whether sense amp is within subarray */
	long long numRow;			/* Number of rows */
	long long numColumn;		/* Number of columns */
	bool multipleRowPerSet;		/* For cache design, whether a set is partitioned into multiple wordlines */
	bool split;			/* Whether the row decoder is at the middle of subarrays */
	int muxSenseAmp;	/* How many bitlines connect to one sense amplifier */
	int muxOutputLev1;	/* How many sense amplifiers connect to one output bit, level-1 */
	int muxOutputLev2;	/* How many sense amplifiers connect to one output bit, level-2 */
	BufferDesignTarget areaOptimizationLevel;
    int num3DLevels; /* Number of monolithic 3D levels in the subarray. */

	bool voltageSense;	/* Whether the sense amplifier is voltage-sensing */
	double senseVoltage;/* Minimum sensible voltage */
	double voltagePrecharge;
	long long numSenseAmp;	/* Number of sense amplifiers */
	double lenWordline;	/* Length of wordlines, Unit: m */
	double lenBitline;	/* Length of bitlines, Unit: m */
	double capWordline;	/* Wordline capacitance, Unit: F */
	double capBitline;	/* Bitline capacitance, Unit: F */
	double resWordline;	/* Wordline resistance, Unit: ohm */
	double resBitline;	/* Bitline resistance, Unit: ohm */
	double resCellAccess; /* Resistance of access device, Unit: ohm */
	double capCellAccess; /* Capacitance of access device, Unit: ohm */
	double resMemCellOff;  /* HRS resistance, Unit: ohm */
	double resMemCellOn;   /* LRS resistance, Unit: ohm */
	double voltageMemCellOff; /* Voltage drop on HRS during read operation, Unit: V */
	double voltageMemCellOn;   /* Voltage drop on LRS druing read operation, Unit: V */
	double resInSerialForSenseAmp; /* Serial resistance of voltage-in voltage sensing as a voltage divider, Unit: ohm */
	double resEquivalentOn;          /* resInSerialForSenseAmp in parallel with resMemCellOn, Unit: ohm */
	double resEquivalentOff;          /* resInSerialForSenseAmp in parallel with resMemCellOn, Unit: ohm */
	double bitlineDelay;	/* Bitline delay, Unit: s */
	double chargeLatency;	/* The bitline charge delay during write operations, Unit: s */
	double columnDecoderLatency;	/* The worst-case mux latency, Unit: s */
	double bitlineDelayOn;  /* Bitline delay of LRS, Unit: s */
	double bitlineDelayOff; /* Bitline delay of HRS, Unit: s */

	RowDecoder	rowDecoder;
	RowDecoder	bitlineMuxDecoder;
	Mux			bitlineMux;
	RowDecoder	senseAmpMuxLev1Decoder;
	Mux			senseAmpMuxLev1;
	RowDecoder	senseAmpMuxLev2Decoder;
	Mux			senseAmpMuxLev2;
	Precharger	precharger;
	SenseAmp	senseAmp;
};

#endif /* SUBARRAY_H_ */
