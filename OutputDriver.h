//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef OUTPUTDRIVER_H_
#define OUTPUTDRIVER_H_

#include "FunctionUnit.h"
#include "constant.h"
#include "typedef.h"

class OutputDriver: public FunctionUnit {
public:
	OutputDriver();
	virtual ~OutputDriver();

	/* Functions */
	void PrintProperty();
	void Initialize(double _logicEffort, double _inputCap, double _outputCap, double _outputRes,
			bool _inv, BufferDesignTarget _areaOptimizationLevel, double _minDriverCurrent);
	void CalculateArea();
	void CalculateRC();
	void CalculateLatency(double _rampInput);
	void CalculatePower();
	OutputDriver & operator=(const OutputDriver &);

	/* Properties */
	bool initialized;	/* Initialization flag */
	bool invalid;      /*Invalidatio flag */
	double logicEffort;	/* The logic effort of the gate that needs this driver */
	double inputCap;	/* Input capacitance, unit: F */
	double outputCap;	/* Output capacitance, unit: F */
	double outputRes;	/* Output resistance, unit: ohm */
	bool inv;			/* Whether the invert chain causes a flip */
	int numStage;		/* Number of inverter chain stages */
	BufferDesignTarget areaOptimizationLevel; /* 0 for latency, 2 for area */
	double minDriverCurrent; /* Minimum driving current should be provided */
	double widthNMOS[MAX_INV_CHAIN_LEN];
	double widthPMOS[MAX_INV_CHAIN_LEN];
	double capInput[MAX_INV_CHAIN_LEN];
	double capOutput[MAX_INV_CHAIN_LEN];
	double rampInput, rampOutput;
};

#endif /* OUTPUTDRIVER_H_ */
