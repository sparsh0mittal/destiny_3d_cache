// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef WIRE_H_
#define WIRE_H_

#include <iostream>
#include "typedef.h"
#include "SenseAmp.h"

using namespace std;

class Wire {
public:
	Wire();
	virtual ~Wire();

	/* Functions */
	void PrintProperty();
	void Initialize(int _featureSizeInNano, WireType _wireType, WireRepeaterType _wireRepeaterType,
			int _temperature, bool _isLowSwing);
	void CalculateLatencyAndPower(double _wireLength, double *delay, double *dynamicEnergy, double *leakagePower);
	void findOptimalRepeater();
	void findPenalizedRepeater(double _penalty);
	double getRepeatedWireUnitDelay();				/* Return delay per unit, Unit: s/m */
	double getRepeatedWireUnitDynamicEnergy();		/* Return dynamic energy per unit, Unit: J/m */
	double getRepeatedWireUnitLeakage();			/* Return leakage power per unit, Unit: W/m */
	Wire & operator=(const Wire &);

	/* Properties */
	bool initialized;	/* Initialization flag */
	int featureSizeInNano; /* Process feature size, Unit: nm */
	double featureSize;	/* Process feature size, Unit: m */
	WireType wireType;	/* Type of wire */
	WireRepeaterType wireRepeaterType;	/* Type of wire repeater */
	int temperature;	/* Unit: K */
	bool isLowSwing;	/* Whether to use Low Swing wire with transmitters and receivers */

	double barrierThickness;		/* Unit: m */
	double horizontalDielectric;	/* Unit: 1 */
	double wirePitch;				/* Unit: m */
	double aspectRatio;				/* Unit: 1 */
	double ildThickness;			/* Unit: m */
	double wireWidth;				/* Unit: m */
	double wireThickness;			/* Unit: m */
	double wireSpacing;				/* Unit: m */

	double repeaterSize;			/* For repeated wire only, non-repeated wire = 0, Unit: minimum driver size*/
	double repeaterSpacing;			/* For repeated wire only, non-repeated wire = inf, Unit: m */
	double repeaterHeight, repeaterWidth;	/* Unit: m */
	double repeatedWirePitch;		/* For repeated wire only, translate the repeaterSize into meter, Unit: m */

	double resWirePerUnit;			/* Unit: ohm/m */
	double capWirePerUnit;			/* Unit: F/m */

	SenseAmp * senseAmp;
};

#endif /* WIRE_H_ */
