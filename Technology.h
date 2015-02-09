//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
// This file contains code from CACTI-3DD, (c) 2012 Hewlett-Packard Development Company, L.P.
//See LICENSE_CACTI3DD file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.
#ifndef TECHNOLOGY_H_
#define TECHNOLOGY_H_

#include <iostream>
#include "typedef.h"
#include "InputParameter.h"
#include "constant.h"
#include "MemCell.h"

using namespace std;

class Technology {
public:
	Technology();
	virtual ~Technology();

	/* Functions */
	void PrintProperty();
	void Initialize(int _featureSizeInNano, DeviceRoadmap _deviceRoadmap, InputParameter *inputParameter);
	void InterpolateWith(Technology rhs, double _alpha);

	/* Properties */
	bool initialized;	/* Initialization flag */
	int featureSizeInNano; /*Process feature size, Unit: nm */
	double featureSize;	/* Process feature size, Unit: m */
	DeviceRoadmap deviceRoadmap;	/* HP, LSTP, or LOP */
	double vdd;			/* Supply voltage, Unit: V */
    double vpp;             /* Wordline overdrive voltage. */
	double vth;				/* Threshold voltage, Unit: V */
	double vdsatNmos;		/* Velocity saturation voltage, Unit: V */
	double vdsatPmos;		/* Velocity saturation voltage, Unit: V */
	double phyGateLength;	/* Physical gate length, Unit: m */
	double capIdealGate;	/* Ideal gate capacitance, Unit: F/m */
	double capFringe;		/* Fringe capacitance, Unit: F/m */
	double capJunction;		/* Junction bottom capacitance, Cj0, Unit: F/m^2 */
	double capOverlap;		/* Overlap capacitance, Cover in MASTAR, Unit: F/m */
	double capSidewall;		/* Junction sidewall capacitance, Cjsw, Unit: F/m */
	double capDrainToChannel;	/* Junction drain to channel capacitance, Cjswg, Unit: F/m */
	double capOx;			/* Cox_elec in MASTAR, Unit: F/m^2 */
	double buildInPotential; /* Bottom junction built-in potential(PB in BSIM4 model), Unit: V */
	double effectiveElectronMobility;		 /* ueff for NMOS in MASTAR, Unit: m^2/V/s */
	double effectiveHoleMobility;            /* ueff for PMOS in MASTAR, Unit: m^2/V/s */
	double pnSizeRatio;	/* PMOS to NMOS size ratio */
	double effectiveResistanceMultiplier;	/* Extra resistance due to vdsat */
	double currentOnNmos[101];		/* NMOS saturation current, Unit: A/m */
	double currentOnPmos[101];		/* PMOS saturation current, Unit: A/m */
	double currentOffNmos[101];	/* NMOS off current (from 300K to 400K), Unit: A/m */
	double currentOffPmos[101]; /* PMOS off current (from 300K to 400K), Unit: A/m */

	double capPolywire;	/* Poly wire capacitance, Unit: F/m */

    double capTSV[NUM_TSV_TYPES];
    double resTSV[NUM_TSV_TYPES];
    double areaTSV[NUM_TSV_TYPES];

    double tsv_resistance(double resistivity, double tsv_len, double tsv_diam, double tsv_contact_resistance);
    double tsv_capacitance(double tsv_len, double tsv_diam, double tsv_pitch, double dielec_thickness, double liner_dielectric_constant, double depletion_width);
    double tsv_area(double tsv_pitch);
    TSV_type WireTypeToTSVType(int wiretype);
    void SetLayerCount(InputParameter *inputParameter, int layers); /* Recalculate TSV parameters based on layer count */

private:
    // TSV parameters from CACTI3DD model
    int layerCount; 
    double tsv_pitch[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_diameter[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_length[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_dielec_thickness[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_contact_resistance[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_depletion_width[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_liner_dielectric_constant[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_parasitic_res[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_parasitic_cap[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];
    double tsv_occupation_area[NUMBER_INTERCONNECT_PROJECTION_TYPES][NUM_TSV_TYPES];


};

#endif /* TECHNOLOGY_H_ */
