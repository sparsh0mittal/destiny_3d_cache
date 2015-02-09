// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#include "Mux.h"
#include "global.h"
#include "formula.h"

Mux::Mux() {
	// TODO Auto-generated constructor stub
	initialized = false;
	capForPreviousPowerCalculation = 0;
	capForPreviousDelayCalculation = 0;
	capNMOSPassTransistor = 0;
	resNMOSPassTransistor = 0;
}

Mux::~Mux() {
	// TODO Auto-generated destructor stub
}

void Mux::Initialize(int _numInput, long long _numMux, double _capLoad, double _capInputNextStage, double _minDriverCurrent){
	if (initialized)
		cout << "[Mux] Warning: Already initialized!" << endl;

	numInput = _numInput;
	numMux = _numMux;
	capLoad = _capLoad;
	capInputNextStage = _capInputNextStage;
	minDriverCurrent = _minDriverCurrent;

	if ((numInput > 1) && (numMux > 0 )) {
		double minNMOSWidth = minDriverCurrent / tech->currentOnNmos[inputParameter->temperature - 300];
		if (cell->memCellType == MRAM || cell->memCellType == PCRAM || cell->memCellType == memristor) {
			/* Mux resistance should be small enough for voltage dividing */
			double maxResNMOSPassTransistor = cell->resistanceOn * IR_DROP_TOLERANCE;
	    	widthNMOSPassTransistor = CalculateOnResistance(tech->featureSize, NMOS, inputParameter->temperature, *tech)
					* tech->featureSize / maxResNMOSPassTransistor;
	    	if (widthNMOSPassTransistor > inputParameter->maxNmosSize * tech->featureSize) {	// Change the transistor size to avoid severe IR drop
	    		widthNMOSPassTransistor = inputParameter->maxNmosSize * tech->featureSize;
	    	}
			widthNMOSPassTransistor = MAX(MAX(widthNMOSPassTransistor,minNMOSWidth), 6 * MIN_NMOS_SIZE * tech->featureSize);
		} else {
			widthNMOSPassTransistor = MAX(6 * MIN_NMOS_SIZE * tech->featureSize, minNMOSWidth);
		}
	}

	initialized = true;
}

void Mux::CalculateArea(){
	if (!initialized) {
		cout << "[Mux] Error: Require initialization first!" << endl;
	} else {
		if ((numInput > 1) && (numMux > 0 )) {
			double h,w;
			CalculateGateArea(INV, 1, widthNMOSPassTransistor, 0, tech->featureSize*40, *tech, &h, &w);
			width = numMux * numInput * w;
			height = h;
			area = width * height;
		} else {
			height = width = area = 0;
		}
	}
}

void Mux::CalculateRC() {
	if (!initialized) {
		cout << "[Mux] Error: Require initialization first!" << endl;
	} else {
		if ((numInput > 1) && (numMux > 0 )) {
			capNMOSPassTransistor = CalculateDrainCap(widthNMOSPassTransistor, NMOS, tech->featureSize*40, *tech);
			capForPreviousPowerCalculation = capNMOSPassTransistor;
			capOutput = numInput * capNMOSPassTransistor;
			capForPreviousDelayCalculation = capOutput + capNMOSPassTransistor + capLoad;
			resNMOSPassTransistor = CalculateOnResistance(widthNMOSPassTransistor, NMOS, inputParameter->temperature, *tech);
		} else {
			;	/* nothing to do */
		}
	}
}

void Mux::CalculateLatency(double _rampInput) {  //rampInput is actually useless in Mux module
	if (!initialized) {
		cout << "[Mux] Error: Require initialization first!" << endl;
	} else {
		if ((numInput > 1) && (numMux > 0 )) {
			rampInput = _rampInput;
			double tr;
			tr = resNMOSPassTransistor * (capOutput + capLoad);
			readLatency = 2.3 * tr;
			writeLatency = readLatency;
		} else {
			readLatency = writeLatency = 0;
		}
	}
}

void Mux::CalculatePower() {
	if (!initialized) {
		cout << "[Mux] Error: Require initialization first!" << endl;
	} else {
		if ((numInput > 1) && (numMux > 0 )) {
			leakage = 0; //TO-DO
			readDynamicEnergy = (capOutput + capInputNextStage) * tech->vdd * (tech->vdd - tech->vth);
			readDynamicEnergy *= numMux;  //worst-case dynamic power analysis
			writeDynamicEnergy = readDynamicEnergy;
		} else {
			readDynamicEnergy = writeDynamicEnergy = leakage = 0;
		}
	}
}

void Mux::PrintProperty() {
	cout << "Mux Properties:" << endl;
	FunctionUnit::PrintProperty();
}

Mux & Mux::operator=(const Mux &rhs) {
	height = rhs.height;
	width = rhs.width;
	area = rhs.area;
	readLatency = rhs.readLatency;
	writeLatency = rhs.writeLatency;
	readDynamicEnergy = rhs.readDynamicEnergy;
	writeDynamicEnergy = rhs.writeDynamicEnergy;
	leakage = rhs.leakage;
	initialized = rhs.initialized;
	numInput = rhs.numInput;
	numMux = rhs.numMux;
	capLoad = rhs.capLoad;
	capInputNextStage = rhs.capInputNextStage;
	minDriverCurrent = rhs.minDriverCurrent;
    capOutput = rhs.capOutput;
	widthNMOSPassTransistor = rhs.widthNMOSPassTransistor;
	resNMOSPassTransistor = rhs.resNMOSPassTransistor;
	capNMOSPassTransistor = rhs.capNMOSPassTransistor;
	capForPreviousDelayCalculation = rhs.capForPreviousDelayCalculation;
	capForPreviousPowerCalculation = rhs.capForPreviousPowerCalculation;
	rampInput = rhs.rampInput;
	rampOutput = rhs.rampOutput;

	return *this;
}
