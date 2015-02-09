//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.

#ifndef PRECHARGER_H_
#define PRECHARGER_H_

#include "FunctionUnit.h"
#include "OutputDriver.h"

class Precharger: public FunctionUnit {
public:
	Precharger();
	virtual ~Precharger();

	/* Functions */
	void PrintProperty();
	void Initialize(double _voltagePrecharge, int _numColumn, double _capBitline, double _resBitline);
	void CalculateArea();
	void CalculateRC();
	void CalculateLatency(double _rampInput);
	void CalculatePower();
	Precharger & operator=(const Precharger &);

	/* Properties */
	bool initialized;	/* Initialization flag */
	OutputDriver outputDriver;
	double voltagePrecharge;  /* Precharge Voltage */
	double capBitline, resBitline;
	double capLoadInv;
	double capOutputBitlinePrecharger;
	double capWireLoadPerColumn, resWireLoadPerColumn;
	double enableLatency;
	int numColumn;			/* Number of columns */
	double widthPMOSBitlinePrecharger, widthPMOSBitlineEqual;
	double widthInvNmos, widthInvPmos;
	double capLoadPerColumn;
	double rampInput, rampOutput;

};

#endif /* PRECHARGER_H_ */
