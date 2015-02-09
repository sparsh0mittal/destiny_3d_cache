// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.

#include "BasicDecoder.h"
#include "formula.h"
#include "global.h"

BasicDecoder::BasicDecoder() {
	// TODO Auto-generated constructor stub
	initialized = false;
}

BasicDecoder::~BasicDecoder() {
	// TODO Auto-generated destructor stub
}

void BasicDecoder::Initialize(int _numAddressBit, double _capLoad, double _resLoad){
	/*if (initialized)
			cout << "Warning: Already initialized!" << endl;*/
	/* might be re-initialized by predecodeblock */
	if (_numAddressBit == 1) {
		numNandInput = 0;
	}
	else {
		numNandInput = _numAddressBit;
	}
	capLoad = _capLoad;
	resLoad = _resLoad;

	if (numNandInput == 0) {
		numNandGate = 0;
		double logicEffortInv = 1;
		double widthInvN = MIN_NMOS_SIZE * tech->featureSize;
		double widthInvP = tech->pnSizeRatio * MIN_NMOS_SIZE * tech->featureSize;
		double capInv = CalculateGateCap(widthInvN, *tech) + CalculateGateCap(widthInvP, *tech);
		outputDriver.Initialize(logicEffortInv, capInv, capLoad, resLoad, true, latency_first, 0);  /* Always Latency First */
	}
	else{
		double logicEffortNand;
		double capNand;
		if (numNandInput == 2) {	/* NAND2 */
			numNandGate = 4;
			widthNandN = 2 * MIN_NMOS_SIZE * tech->featureSize;
			logicEffortNand = (2+tech->pnSizeRatio) / (1+tech->pnSizeRatio);
		} else {					/* NAND3 */
			numNandGate = 8;
			widthNandN = 3 * MIN_NMOS_SIZE * tech->featureSize;
			logicEffortNand = (3+tech->pnSizeRatio) / (1+tech->pnSizeRatio);
		}
		widthNandP = tech->pnSizeRatio * MIN_NMOS_SIZE * tech->featureSize;
		capNand = CalculateGateCap(widthNandN, *tech) + CalculateGateCap(widthNandP, *tech);
		outputDriver.Initialize(logicEffortNand, capNand, capLoad, resLoad, true, latency_first, 0);  /* Always Latency First */
	}
	initialized = true;
}

void BasicDecoder::CalculateArea() {
	if (!initialized) {
		cout << "[Basic Decoder] Error: Require initialization first!" << endl;
	} else {
		outputDriver.CalculateArea();
		if (numNandInput == 0){
			height = 2 * outputDriver.height;
			width = outputDriver.width;
		}
		else {
			double hNand, wNand;
			CalculateGateArea(NAND, numNandInput, widthNandN, widthNandP, tech->featureSize*40, *tech, &hNand, &wNand);
			height = MAX(hNand, outputDriver.height);
			width = wNand + outputDriver.width;
			height *= numNandGate;
		}
		area = height * width;
	}
}

void BasicDecoder::CalculateRC() {
	if (!initialized) {
		cout << "[Basic Decoder] Error: Require initialization first!" << endl;
	} else {
		outputDriver.CalculateRC();
		if (numNandInput > 0) {
			CalculateGateCapacitance(NAND, numNandInput, widthNandN, widthNandP, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech, &capNandInput, &capNandOutput);
		}
	}
}

void BasicDecoder::CalculateLatency(double _rampInput) {
	if (!initialized) {
		cout << "[Basic Decoder] Error: Require initialization first!" << endl;
	} else {
		rampInput = _rampInput;
        if (numNandInput == 0) {
        	outputDriver.CalculateLatency(rampInput);
        	readLatency  = outputDriver.readLatency;
        	writeLatency = readLatency;
        } else {
        	double resPullDown;
        	double capLoad;
        	double tr;	/* time constant */
        	double gm;	/* transconductance */
        	double beta;	/* for horowitz calculation */
        	double rampInputForDriver;

        	resPullDown = CalculateOnResistance(widthNandN, NMOS, inputParameter->temperature, *tech) * numNandInput;
        	capLoad = capNandOutput + outputDriver.capInput[0];
        	tr = resPullDown * capLoad;
        	gm = CalculateTransconductance(widthNandN, NMOS, *tech);
        	beta = 1 / (resPullDown * gm);
        	readLatency = horowitz(tr, beta, rampInput, &rampInputForDriver);

        	outputDriver.CalculateLatency(rampInputForDriver);
        	readLatency += outputDriver.readLatency;
        	writeLatency = readLatency;
        }
        rampOutput = outputDriver.rampOutput;
	}
}

void BasicDecoder::CalculatePower() {
	if (!initialized) {
		cout << "[Basic Decoder] Error: Require initialization first!" << endl;
	} else {
		outputDriver.CalculatePower();
		double capLoad;
		if (numNandInput == 0) {
			leakage = 2 * outputDriver.leakage;
			capLoad = outputDriver.capInput[0] + outputDriver.capOutput[0];
			readDynamicEnergy = capLoad * tech->vdd * tech->vdd;
			readDynamicEnergy += outputDriver.readDynamicEnergy;
			readDynamicEnergy *= 1;	/* only one row is activated each time */
			writeDynamicEnergy = readDynamicEnergy;

		} else {
			/* Leakage power */
			leakage = CalculateGateLeakage(NAND, numNandInput, widthNandN, widthNandP,
					inputParameter->temperature, *tech) * tech->vdd;
			leakage += outputDriver.leakage;
			leakage *= numNandGate;
			/* Dynamic energy */
			capLoad = capNandOutput + outputDriver.capInput[0];
			readDynamicEnergy = capLoad * tech->vdd * tech->vdd;
			readDynamicEnergy += outputDriver.readDynamicEnergy;
			readDynamicEnergy *= 1;	/* only one row is activated each time */
			writeDynamicEnergy = readDynamicEnergy;
		}
	}
}

void BasicDecoder::PrintProperty() {
	cout << numNandInput << " to " << numNandGate << " Decoder Properties:" << endl;
	FunctionUnit::PrintProperty();
}
