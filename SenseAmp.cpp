//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#include "SenseAmp.h"
#include "formula.h"
#include "global.h"

SenseAmp::SenseAmp() {
	// TODO Auto-generated constructor stub
	initialized = false;
	invalid = false;
}

SenseAmp::~SenseAmp() {
	// TODO Auto-generated destructor stub
}

void SenseAmp::Initialize(long long _numColumn, bool _currentSense, double _senseVoltage, double _pitchSenseAmp) {
	if (initialized)
		cout << "[Sense Amp] Warning: Already initialized!" << endl;

	numColumn = _numColumn;
	currentSense = _currentSense;
	senseVoltage = _senseVoltage;
	pitchSenseAmp = _pitchSenseAmp;

	if (pitchSenseAmp <= tech->featureSize * 3) {
		/* too small, cannot do the layout */
		invalid = true;
	}

	initialized = true;
}

void SenseAmp::CalculateArea() {
	if (!initialized) {
		cout << "[Sense Amp] Error: Require initialization first!" << endl;
	} else if (invalid) {
		height = width = area = invalid_value;
	} else {
		height = width = area = 0;
		if (currentSense) {	/* current-sensing needs IV converter */
			area += IV_CONVERTER_AREA * tech->featureSize * tech->featureSize;
		}
		/* the following codes are transformed from CACTI 6.5 */
		double tempHeight = 0;
		double tempWidth = 0;

		CalculateGateArea(INV, 1, 0, W_SENSE_P * tech->featureSize,
				pitchSenseAmp, *tech, &tempWidth, &tempHeight);	/* exchange width and height for senseamp layout */
		width = MAX(width, tempWidth);
		height += 2 * tempHeight;
		CalculateGateArea(INV, 1, 0, W_SENSE_ISO * tech->featureSize,
				pitchSenseAmp, *tech, &tempWidth, &tempHeight);	/* exchange width and height for senseamp layout */
		width = MAX(width, tempWidth);
		height += tempHeight;
		height += 2 * MIN_GAP_BET_SAME_TYPE_DIFFS * tech->featureSize;

		CalculateGateArea(INV, 1, W_SENSE_N * tech->featureSize, 0,
				pitchSenseAmp, *tech, &tempWidth, &tempHeight);	/* exchange width and height for senseamp layout */
		width = MAX(width, tempWidth);
		height += 2 * tempHeight;
		CalculateGateArea(INV, 1, W_SENSE_EN * tech->featureSize, 0,
				pitchSenseAmp, *tech, &tempWidth, &tempHeight);	/* exchange width and height for senseamp layout */
		width = MAX(width, tempWidth);
		height += tempHeight;
		height += 2 * MIN_GAP_BET_SAME_TYPE_DIFFS * tech->featureSize;

		height += MIN_GAP_BET_P_AND_N_DIFFS * tech->featureSize;

		/* transformation so that width meets the pitch */
		height = height * width / pitchSenseAmp;
		width = pitchSenseAmp;

		/* Add additional area if IV converter exists */
		height += area / width;
		width *= numColumn;

		area = height * width;
	}
}

void SenseAmp::CalculateRC() {
	if (!initialized) {
		cout << "[Sense Amp] Error: Require initialization first!" << endl;
	} else if (invalid) {
		readLatency = writeLatency = invalid_value;
	} else {
		capLoad = CalculateGateCap((W_SENSE_P + W_SENSE_N) * tech->featureSize, *tech)
				+ CalculateDrainCap(W_SENSE_N * tech->featureSize, NMOS, pitchSenseAmp, *tech)
				+ CalculateDrainCap(W_SENSE_P * tech->featureSize, PMOS, pitchSenseAmp, *tech)
				+ CalculateDrainCap(W_SENSE_ISO * tech->featureSize, PMOS, pitchSenseAmp, *tech)
				+ CalculateDrainCap(W_SENSE_MUX * tech->featureSize, NMOS, pitchSenseAmp, *tech);
	}
}

void SenseAmp::CalculateLatency(double _rampInput) {	/* _rampInput is actually no use in SenseAmp */
	if (!initialized) {
		cout << "[Sense Amp] Error: Require initialization first!" << endl;
	} else {
		readLatency = writeLatency = 0;
		if (currentSense) {	/* current-sensing needs IV converter */
			/* all the following values achieved from HSPICE */
			if (tech->featureSize >= 179e-9)
				readLatency += 0.46e-9;		/* 120nm */
			else if (tech->featureSize >= 119e-9)
				readLatency += 0.49e-9;		/* 120nm */
			else if (tech->featureSize >= 89e-9)
				readLatency += 0.53e-9;		/* 90nm */
			else if (tech->featureSize >= 64e-9)
				readLatency += 0.62e-9;		/* 65nm */
			else if (tech->featureSize >= 44e-9)
				readLatency += 0.80e-9;		/* 45nm */
			else if (tech->featureSize >= 31e-9)
				readLatency += 1.07e-9;		/* 32nm */
			else
				readLatency += 1.45e-9;     /* below 22nm */
		}

		/* Voltage sense amplifier */
		double gm = CalculateTransconductance(W_SENSE_N * tech->featureSize, NMOS, *tech)
				+ CalculateTransconductance(W_SENSE_P * tech->featureSize, PMOS, *tech);
		double tau = capLoad / gm;
		readLatency += tau * log(tech->vdd / senseVoltage);
        refreshLatency = readLatency;
	}
}

void SenseAmp::CalculatePower() {
	if (!initialized) {
		cout << "[Sense Amp] Error: Require initialization first!" << endl;
	} else if (invalid) {
		readDynamicEnergy = writeDynamicEnergy = leakage = invalid_value;
	} else {
		readDynamicEnergy = writeDynamicEnergy = 0;
		leakage = 0;
		if (currentSense) {	/* current-sensing needs IV converter */
			/* all the following values achieved from HSPICE */
			if (tech->featureSize >= 119e-9) {			/* 120nm */
				readDynamicEnergy += 8.52e-14;	/* Unit: J */
				leakage += 1.40e-8;				/* Unit: W */
			} else if (tech->featureSize >= 89e-9) {	/* 90nm */
				readDynamicEnergy += 8.72e-14;
				leakage += 1.87e-8;
			} else if (tech->featureSize >= 64e-9) {	/* 65nm */
				readDynamicEnergy += 9.00e-14;
				leakage += 2.57e-8;
			} else if (tech->featureSize >= 44e-9) {	/* 45nm */
				readDynamicEnergy += 10.26e-14;
				leakage += 4.41e-9;
			} else if (tech->featureSize >= 31e-9) {	/* 32nm */
				readDynamicEnergy += 12.56e-14;
				leakage += 12.54e-8;
			} else {                                    /* TO-DO, need calibration below 22nm */
				readDynamicEnergy += 15e-14;
				leakage += 15e-8;
			}
		}

		/* Voltage sense amplifier */
		readDynamicEnergy += capLoad * tech->vdd * tech->vdd;
		double idleCurrent =  CalculateGateLeakage(INV, 1, W_SENSE_EN * tech->featureSize, 0,
				inputParameter->temperature, *tech) * tech->vdd;
		leakage += idleCurrent * tech->vdd;

		readDynamicEnergy *= numColumn;
		leakage *= numColumn;

        refreshDynamicEnergy = readDynamicEnergy;
	}
}

void SenseAmp::PrintProperty() {
	cout << "Sense Amplifier Properties:" << endl;
	FunctionUnit::PrintProperty();
}

SenseAmp & SenseAmp::operator=(const SenseAmp &rhs) {
	height = rhs.height;
	width = rhs.width;
	area = rhs.area;
	readLatency = rhs.readLatency;
	writeLatency = rhs.writeLatency;
    refreshLatency = rhs.refreshLatency;
	readDynamicEnergy = rhs.readDynamicEnergy;
	writeDynamicEnergy = rhs.writeDynamicEnergy;
    refreshDynamicEnergy = rhs.refreshDynamicEnergy;
	resetLatency = rhs.resetLatency;
	setLatency = rhs.setLatency;
	resetDynamicEnergy = rhs.resetDynamicEnergy;
	setDynamicEnergy = rhs.setDynamicEnergy;
	cellReadEnergy = rhs.cellReadEnergy;
	cellSetEnergy = rhs.cellSetEnergy;
	cellResetEnergy = rhs.cellResetEnergy;
	leakage = rhs.leakage;
	initialized = rhs.initialized;
	invalid = rhs.invalid;
	numColumn = rhs.numColumn;
	currentSense = rhs.currentSense;
	senseVoltage = rhs.senseVoltage;
	capLoad = rhs.capLoad;
	pitchSenseAmp = rhs.pitchSenseAmp;

	return *this;
}
