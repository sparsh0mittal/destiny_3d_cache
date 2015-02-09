//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#include "BankWithoutHtree.h"
#include "formula.h"
#include "global.h"
#include <math.h>

BankWithoutHtree::BankWithoutHtree() {
	// TODO Auto-generated constructor stub
	initialized = false;
	invalid = false;
}

BankWithoutHtree::~BankWithoutHtree() {
	// TODO Auto-generated destructor stub
}


void BankWithoutHtree::Initialize(int _numRowMat, int _numColumnMat, long long _capacity,
		long _blockSize, int _associativity, int _numRowPerSet, int _numActiveMatPerRow,
		int _numActiveMatPerColumn, int _muxSenseAmp, bool _internalSenseAmp, int _muxOutputLev1, int _muxOutputLev2,
		int _numRowSubarray, int _numColumnSubarray,
		int _numActiveSubarrayPerRow, int _numActiveSubarrayPerColumn,
		BufferDesignTarget _areaOptimizationLevel, MemoryType _memoryType,
        int _stackedDieCount, int _partitionGranularity, int monolithicStackCount) {
	if (initialized) {
		/* Reset the class for re-initialization */
		initialized = false;
		invalid = false;
	}

	if (!_internalSenseAmp) {
		if (cell->memCellType == DRAM || cell->memCellType == eDRAM) {
			invalid = true;
			cout << "[BankWithoutHtree] Error: DRAM does not support external sense amplification!" << endl;
			return;
		} else if (globalWire->wireRepeaterType != repeated_none) {
			invalid = true;
			initialized = true;
			return;
		}
	}

	numRowMat = _numRowMat;
	numColumnMat = _numColumnMat;
	capacity = _capacity;
	blockSize = _blockSize;
	associativity = _associativity;
	numRowPerSet = _numRowPerSet;
	internalSenseAmp = _internalSenseAmp;
	areaOptimizationLevel = _areaOptimizationLevel;
	memoryType = _memoryType;
    stackedDieCount = _stackedDieCount;
    partitionGranularity = _partitionGranularity;
	numWay = 1;	/* default value for non-cache design */

	/* Calculate the physical signals that are required in routing. Use double during the calculation to avoid overflow */
    if (stackedDieCount > 1 /*&& partitionGranularity == 0*/) {
        numAddressBit = (int)(log2((double)capacity / blockSize / associativity / stackedDieCount) + 0.1);
    } else {
        numAddressBit = (int)(log2((double)capacity / blockSize / associativity) + 0.1);
    }

	if (_numActiveMatPerRow > numColumnMat) {
		cout << "[Bank] Warning: The number of active subarray per row is larger than the number of subarray per row!"  << endl;
		cout << _numActiveMatPerRow << " > " << numColumnMat << endl;
		numActiveMatPerRow = numColumnMat;
	} else {
		numActiveMatPerRow = _numActiveMatPerRow;
	}
	if (_numActiveMatPerColumn > numRowMat) {
		cout << "[Bank] Warning: The number of active subarray per column is larger than the number of subarray per column!"  << endl;
		cout << _numActiveMatPerColumn << " > " << numRowMat << endl;
		numActiveMatPerColumn = numRowMat;
	} else {
		numActiveMatPerColumn = _numActiveMatPerColumn;
	}
	muxSenseAmp = _muxSenseAmp;
	muxOutputLev1 = _muxOutputLev1;
	muxOutputLev2 = _muxOutputLev2;

	numRowSubarray = _numRowSubarray;
	numColumnSubarray = _numColumnSubarray;
	if (_numActiveSubarrayPerRow > numColumnSubarray) {
		cout << "[Bank] Warning: The number of active subarray per row is larger than the number of subarray per row!"  << endl;
		cout << _numActiveSubarrayPerRow << " > " << numColumnSubarray << endl;
		numActiveSubarrayPerRow = numColumnSubarray;
	} else {
		numActiveSubarrayPerRow = _numActiveSubarrayPerRow;
	}
	if (_numActiveSubarrayPerColumn > numRowSubarray) {
		cout << "[Bank] Warning: The number of active subarray per column is larger than the number of subarray per column!"  << endl;
		cout << _numActiveSubarrayPerColumn << " > " << numRowSubarray << endl;
		numActiveSubarrayPerColumn = numRowSubarray;
	} else {
		numActiveSubarrayPerColumn = _numActiveSubarrayPerColumn;
	}

	/* The number of address bits that are used to power gate inactive mats */
	int numAddressForGating = (int)(log2(numRowMat * numColumnMat / numActiveMatPerColumn / numActiveMatPerRow)+0.1);
	numAddressBitRouteToMat = numAddressBit - numAddressForGating;	/* Only use the effective address bits in the following calculation */
	numDataBitRouteToMat = blockSize;


	if (memoryType == data) { /* Data array */
		numDataBitRouteToMat = blockSize / numActiveMatPerColumn / numActiveMatPerRow;
		if (numRowPerSet > associativity) {
			/* There is no enough ways to distribute into multiple rows */
			invalid = true;
			initialized = true;
			return;
		}
		numWay = associativity;
		int numWayPerRow = numWay / numRowPerSet;	/* At least 1, otherwise it is invalid, and returned already */
		if (numWayPerRow > 1) {		/* multiple ways per row, needs extra mux level */
			/* Do mux level recalculation to contain the multiple ways */
			if (cell->memCellType == DRAM || cell->memCellType == eDRAM) {
				/* for DRAM, mux before sense amp has to be 1, only mux output1 and mux output2 can be used */
				int numWayPerRowInLog = (int)(log2((double)numWayPerRow) + 0.1);
				int extraMuxOutputLev2 = (int)pow(2, numWayPerRowInLog / 2);
				int extraMuxOutputLev1 = numWayPerRow / extraMuxOutputLev2;
				muxOutputLev1 *= extraMuxOutputLev1;
				muxOutputLev2 *= extraMuxOutputLev2;
			} else {
				/* for non-DRAM, all mux levels can be used */
				int numWayPerRowInLog = (int)(log2((double)numWayPerRow) + 0.1);
				int extraMuxOutputLev2 = (int)pow(2, numWayPerRowInLog / 3);
				int extraMuxOutputLev1 = extraMuxOutputLev2;
				int extraMuxSenseAmp = numWayPerRow / extraMuxOutputLev1 / extraMuxOutputLev2;
				muxSenseAmp *= extraMuxSenseAmp;
				muxOutputLev1 *= extraMuxOutputLev1;
				muxOutputLev2 *= extraMuxOutputLev2;
			}
		}
	} else if (memoryType == tag) { /* Tag array */
		if (numRowPerSet > 1) {
			/* tag array cannot have multiple rows to contain ways in a set, otherwise the bitline has to be shared */
			invalid = true;
			initialized = true;
			return;
		}
		numDataBitRouteToMat = blockSize;
		numWay = associativity / numActiveMatPerColumn / numActiveMatPerRow;
		if (numWay < 1) {
			/* This mat does not contain at least one way */
			invalid = true;
			initialized = true;
			return;
		}
	} else { /* CAM */
		numDataBitRouteToMat = blockSize;
		numWay = 1;
	}


    // NVSIM3D - Give each mat only the number of subarrays per layer
	//mat.Initialize(numRowSubarray, numColumnSubarray, numAddressBitRouteToMat, numDataBitRouteToMat,
	//		numWay, numRowPerSet, false, numActiveSubarrayPerRow, numActiveSubarrayPerColumn,
	//		muxSenseAmp, internalSenseAmp, muxOutputLev1, muxOutputLev2, areaOptimizationLevel, memoryType);
	mat.Initialize(numRowSubarray, numColumnSubarray, numAddressBitRouteToMat, numDataBitRouteToMat,
			numWay, numRowPerSet, false, numActiveSubarrayPerRow, numActiveSubarrayPerColumn,
			muxSenseAmp, internalSenseAmp, muxOutputLev1, muxOutputLev2, areaOptimizationLevel, memoryType, 
            stackedDieCount, partitionGranularity, monolithicStackCount);
	/* Check if mat is under a legal configuration */
	if (mat.invalid) {
		invalid = true;
		initialized = true;
		return;
	}

	mat.CalculateArea();

	if (!internalSenseAmp) {
		bool voltageSense = true;
		double senseVoltage;
		senseVoltage = cell->minSenseVoltage;
		if (cell->memCellType == SRAM) {
			/* SRAM, DRAM, and eDRAM all use voltage sensing */
			voltageSense = true;
		} else if (cell->memCellType == MRAM || cell->memCellType == PCRAM || cell->memCellType == memristor || cell->memCellType == FBRAM) {
			voltageSense = cell->readMode;
		} else {/* NAND flash */
			// TO-DO
		}

		int numSenseAmp;
		if (memoryType == data)
			numSenseAmp = blockSize;
		else
			numSenseAmp = blockSize * associativity;

		globalSenseAmp.Initialize(numSenseAmp, !voltageSense, senseVoltage, mat.width * numColumnMat / numSenseAmp);
		if (globalSenseAmp.invalid) {
			invalid = true;
			initialized = true;
			return;
		}
		globalSenseAmp.CalculateRC();
		globalBitlineMux.Initialize(numRowMat * numColumnMat / numActiveMatPerColumn / numActiveMatPerRow, numSenseAmp, globalSenseAmp.capLoad, globalSenseAmp.capLoad, 0);
		globalBitlineMux.CalculateRC();

		if (memoryType == tag)
			globalComparator.Initialize(blockSize, 0 /* TO-DO: only for test */);
	}

	/* Reset the mux values for correct printing */
	muxSenseAmp = _muxSenseAmp;
	muxOutputLev1 = _muxOutputLev1;
	muxOutputLev2 = _muxOutputLev2;

    /* Initialize TSV connections. */
    if (stackedDieCount > 1 /*&& partitionGranularity == 0*/) {
        TSV_type tsv_type = tech->WireTypeToTSVType(inputParameter->maxGlobalWireType);
        tsvArray.Initialize(tsv_type);
    }

	initialized = true;
}

void BankWithoutHtree::CalculateArea() {
	if (!initialized) {
		cout << "[BankWithoutHtree] Error: Require initialization first!" << endl;
	} else if (invalid) {
		height = width = area = invalid_value;
	} else {
		height = mat.height * numRowMat;
		width = mat.width * numColumnMat;

		int numWireSharingWidth;
		double effectivePitch;
		if (globalWire->wireRepeaterType == repeated_none) {
			numWireSharingWidth = 1;
			effectivePitch = 0;		/* assume that the wire is built on another metal layer, there does not cause silicon area */
			//effectivePitch = globalWire->wirePitch;
		} else {
			numWireSharingWidth = (int)floor(globalWire->repeaterSpacing / globalWire->repeaterHeight);
			effectivePitch = globalWire->repeatedWirePitch;
		}

		width += ceil((double)numRowMat * numColumnMat * numAddressBitRouteToMat / numWireSharingWidth) * effectivePitch;

		if (!internalSenseAmp) {
			globalSenseAmp.CalculateArea();
			height += globalSenseAmp.height;
			globalBitlineMux.CalculateArea();
			height += globalBitlineMux.height;
			if (memoryType == tag) {
				globalComparator.CalculateArea();
				height += associativity * globalComparator.area / width;
			}
		}

		/* Determine if the aspect ratio meets the constraint */
		if (memoryType == data)
			if (height / width > CONSTRAINT_ASPECT_RATIO_BANK || width / height > CONSTRAINT_ASPECT_RATIO_BANK) {
				/* illegal */
				invalid = true;
				height = width = area = invalid_value;
				return;
			}

		area = height * width;

        /* Initialize TSV connections. */
        if (stackedDieCount > 1 /*&& partitionGranularity == 0*/) {
            tsvArray.CalculateArea();

            //int numControlBits = (int)(log2((double)stackedDieCount + 0.1));
            int numControlBits = stackedDieCount;
            int numAddressBits = (int)(log2((double)capacity / blockSize / associativity / stackedDieCount) + 0.1);
            int numDataBits = blockSize * 2; // Read and write TSVs

            // Fine-granularity has predecoders on logic layer
            if (partitionGranularity == 1) {
                numAddressBits = 0;
            }

            double redundancyFactor = inputParameter->tsvRedundancy;
            tsvArray.numTotalBits = (int)((double)(numControlBits + numAddressBits + numDataBits) * redundancyFactor);
            tsvArray.numAccessBits = (int)((double)(numControlBits + numAddressBits + blockSize) * redundancyFactor);

            // We're not adding in a particular dimension (width/height) so increase the total
            area += tsvArray.numTotalBits * tsvArray.area;
        }
	}
}

void BankWithoutHtree::CalculateRC() {
	if (!initialized) {
		cout << "[BankWithoutHtree] Error: Require initialization first!" << endl;
	} else if (!invalid) {
		mat.CalculateRC();
		if (!internalSenseAmp) {
			globalBitlineMux.CalculateRC();
			globalSenseAmp.CalculateRC();
			if (memoryType == tag)
				globalComparator.CalculateRC();
		}
	}
}

void BankWithoutHtree::CalculateLatencyAndPower() {
	if (!initialized) {
		cout << "[BankWithoutHtree] Error: Require initialization first!" << endl;
	} else if (invalid) {
		readLatency = writeLatency = invalid_value;
		readDynamicEnergy = writeDynamicEnergy = invalid_value;
		leakage = invalid_value;
	} else {
		double latency = 0;
		double energy = 0;
		double leakageWire = 0;

		mat.CalculateLatency(infinite_ramp);
		mat.CalculatePower();
		readLatency = resetLatency = setLatency = writeLatency = 0;
        refreshLatency = mat.refreshLatency * numColumnMat; // TOTAL refresh time for all Mats
		readDynamicEnergy = writeDynamicEnergy = resetDynamicEnergy = setDynamicEnergy = 0;
        refreshDynamicEnergy = mat.refreshDynamicEnergy * numRowMat * numColumnMat;
		leakage = 0;

		double lengthWire;
		lengthWire = mat.height * (numRowMat + 1);
		for (int i = 0; i < numRowMat; i++) {
			lengthWire -= mat.height;
			if (internalSenseAmp) {
				double numBitRouteToMat = 0;
				globalWire->CalculateLatencyAndPower(lengthWire, &latency, &energy, &leakageWire);
				if (i == 0){
					readLatency += latency;
					writeLatency += latency;
                    refreshLatency += latency;
				}
				if (i < numActiveMatPerColumn) {
					if (memoryType == tag)
						numBitRouteToMat = numAddressBitRouteToMat + numDataBitRouteToMat + numWay;
					else
						numBitRouteToMat = numAddressBitRouteToMat + numDataBitRouteToMat;
					readDynamicEnergy += energy * numBitRouteToMat * numActiveMatPerRow;
					writeDynamicEnergy += energy * numBitRouteToMat * numActiveMatPerRow;
                    refreshDynamicEnergy += energy * numBitRouteToMat * numActiveMatPerRow;
				}
				leakage += leakageWire * numBitRouteToMat * numColumnMat;
			} else {
				double resLocalBitline, capLocalBitline, resBitlineMux, capBitlineMux;
				capBitlineMux = globalBitlineMux.capNMOSPassTransistor;
				resBitlineMux = globalBitlineMux.resNMOSPassTransistor;
				resLocalBitline = mat.subarray.resBitline + 3 * resBitlineMux;
				capLocalBitline = mat.subarray.capBitline + 6 * capBitlineMux;
				double resGlobalBitline, capGlobalBitline;
				resGlobalBitline = lengthWire * globalWire->resWirePerUnit;
				capGlobalBitline = lengthWire * globalWire->capWirePerUnit;
				double capGlobalBitlineMux;
				capGlobalBitlineMux = globalBitlineMux.capForPreviousDelayCalculation;
				if (cell->memCellType == SRAM) {
					double vpre = cell->readVoltage;	/* This value should be equal to resetVoltage and setVoltage for SRAM */
					if (i == 0) {
						latency = resLocalBitline * capGlobalBitline / 2 +
								(resLocalBitline + resGlobalBitline) * (capGlobalBitline / 2 + capGlobalBitlineMux);
						latency *= log(vpre / (vpre - globalSenseAmp.senseVoltage));
						latency += resLocalBitline * capGlobalBitline / 2;
						globalBitlineMux.CalculateLatency(1e20);
						latency += globalBitlineMux.readLatency;
						globalSenseAmp.CalculateLatency(1e20);
						writeLatency += latency;
						latency += globalSenseAmp.readLatency;
						readLatency += latency;
					}
					if (i <  numActiveMatPerColumn) {
						energy = capGlobalBitline * tech->vdd * tech->vdd * numAddressBitRouteToMat;
						readDynamicEnergy += energy;
						writeDynamicEnergy += energy;
						readDynamicEnergy += capGlobalBitline * vpre * vpre * numWay;
						writeDynamicEnergy += capGlobalBitline * vpre * vpre * numDataBitRouteToMat;
					}
				} else if (cell->memCellType == MRAM || cell->memCellType == PCRAM || cell->memCellType == memristor || cell->memCellType == FBRAM) {
					double vWrite = MAX(fabs(cell->resetVoltage), fabs(cell->setVoltage));
					double tau, latencyOff, latencyOn;
					double vPre = mat.subarray.voltagePrecharge;
					double vOn = mat.subarray.voltageMemCellOn;
					double vOff = mat.subarray.voltageMemCellOff;
					if (i == 0) {
						tau = resBitlineMux * capGlobalBitline / 2 + (resBitlineMux + resGlobalBitline)
								* (capGlobalBitline + capLocalBitline) / 2 + (resBitlineMux + resGlobalBitline
										+ resLocalBitline) * capLocalBitline / 2;
						writeLatency += 0.63 * tau;
						if (cell->readMode == false) {	/* current-sensing */
							/* Use ICCAD 2009 model */
							resLocalBitline += mat.subarray.resMemCellOff;
							tau = resGlobalBitline * capGlobalBitline / 2 *
									(resLocalBitline + resGlobalBitline / 3) / (resLocalBitline + resGlobalBitline);
							readLatency += 0.63 * tau;
						} else {						/* voltage-sensing */
							if (cell->readVoltage == 0) {  /* Current-in voltage sensing */
								resLocalBitline += mat.subarray.resMemCellOn;
								tau = resLocalBitline * capGlobalBitline + (resLocalBitline + resGlobalBitline) * capGlobalBitline / 2;
								latencyOn = tau * log((vPre - vOn)/(vPre - vOn - globalSenseAmp.senseVoltage));
								resLocalBitline += cell->resistanceOff - cell->resistanceOn;
								tau = resLocalBitline * capGlobalBitline + (resLocalBitline + resGlobalBitline) * capGlobalBitline / 2;
								latencyOff = tau * log((vOff - vPre)/(vOff - vPre - globalSenseAmp.senseVoltage));
							} else {   /*Voltage-in voltage sensing */
								resLocalBitline += mat.subarray.resEquivalentOn;
								tau = resLocalBitline * capGlobalBitline + (resLocalBitline + resGlobalBitline) * capGlobalBitline / 2;
								latencyOn = tau * log((vPre - vOn)/(vPre - vOn - globalSenseAmp.senseVoltage));
								resLocalBitline += mat.subarray.resEquivalentOff - mat.subarray.resEquivalentOn;
								tau = resLocalBitline * capGlobalBitline + (resLocalBitline + resGlobalBitline) * capGlobalBitline / 2;
								latencyOff = tau * log((vOff - vPre)/(vOff - vPre - globalSenseAmp.senseVoltage));
							}
							readLatency -= mat.subarray.bitlineDelay;
							if ((latencyOn + mat.subarray.bitlineDelayOn) > (latencyOff + mat.subarray.bitlineDelayOff))
								readLatency += latencyOn + mat.subarray.bitlineDelayOn;
							else
								readLatency += latencyOff + mat.subarray.bitlineDelayOff;
						}
					}
					if (i <  numActiveMatPerColumn) {
						energy = capGlobalBitline * tech->vdd * tech->vdd * numAddressBitRouteToMat;
						readDynamicEnergy += energy;
						writeDynamicEnergy += energy;
						writeDynamicEnergy += capGlobalBitline * vWrite * vWrite * numDataBitRouteToMat;
						if (cell->readMode) { /*Voltage-in voltage sensing */
							readDynamicEnergy += capGlobalBitline * (vPre * vPre - vOn * vOn )* numDataBitRouteToMat;
						}
					}
				}

			}
		}
		if (!internalSenseAmp) {
			globalBitlineMux.CalculateLatency(1e40);
			globalSenseAmp.CalculateLatency(1e40);
			readLatency += globalBitlineMux.readLatency + globalSenseAmp.readLatency;
			writeLatency += globalBitlineMux.writeLatency + globalSenseAmp.writeLatency;
			globalBitlineMux.CalculatePower();
			globalSenseAmp.CalculatePower();
			readDynamicEnergy += (globalBitlineMux.readDynamicEnergy + globalSenseAmp.readDynamicEnergy) * numActiveMatPerRow;
			writeDynamicEnergy += (globalBitlineMux.writeDynamicEnergy + globalSenseAmp.writeDynamicEnergy) * numActiveMatPerRow;
			leakage += (globalBitlineMux.leakage + globalSenseAmp.leakage) * numColumnMat;
			if (memoryType == tag) {
				globalComparator.CalculateLatency(1e40);
				readLatency += globalComparator.readLatency;
				globalComparator.CalculatePower();
				readDynamicEnergy += numWay * globalComparator.readDynamicEnergy;
				leakage += associativity * globalComparator.leakage;
			}
		}
	}

	/* only 1/A wires are activated in fast mode cache write */
	if (inputParameter->designTarget == cache && inputParameter->cacheAccessMode == fast_access_mode)
		writeDynamicEnergy /= inputParameter->associativity;

	readLatency += mat.readLatency;
	resetLatency = writeLatency + mat.resetLatency;
	setLatency = writeLatency + mat.setLatency;
	writeLatency += mat.writeLatency;
	readDynamicEnergy += mat.readDynamicEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	cellReadEnergy = mat.cellReadEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	cellSetEnergy = mat.cellSetEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	cellResetEnergy = mat.cellResetEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	resetDynamicEnergy = writeDynamicEnergy + mat.resetDynamicEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	setDynamicEnergy = writeDynamicEnergy + mat.setDynamicEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	writeDynamicEnergy += mat.writeDynamicEnergy * numActiveMatPerRow * numActiveMatPerColumn;
	leakage += mat.leakage * numRowMat * numColumnMat;

    /* Why is this stuff outside the else conditional? */
    routingReadLatency = readLatency - mat.readLatency;
    routingWriteLatency = writeLatency - mat.writeLatency;
    routingResetLatency = resetLatency - mat.resetLatency;
    routingSetLatency = setLatency - mat.setLatency;
    routingRefreshLatency = refreshLatency - mat.refreshLatency;

    routingReadDynamicEnergy = readDynamicEnergy - mat.readDynamicEnergy * numActiveMatPerColumn * numActiveMatPerRow;
    routingWriteDynamicEnergy = writeDynamicEnergy - mat.writeDynamicEnergy * numActiveMatPerColumn * numActiveMatPerRow;
    routingResetDynamicEnergy = resetDynamicEnergy - mat.resetDynamicEnergy * numActiveMatPerColumn * numActiveMatPerRow;
    routingSetDynamicEnergy = setDynamicEnergy - mat.setDynamicEnergy * numActiveMatPerColumn * numActiveMatPerRow;
    routingRefreshDynamicEnergy = refreshDynamicEnergy - mat.refreshDynamicEnergy * numActiveMatPerColumn * numActiveMatPerRow;

    routingLeakage = leakage - mat.leakage * numColumnMat * numRowMat;

    /* For non-Htree bank, each layer contains an exact copy of this bank. */
    if (initialized & !invalid && stackedDieCount > 1) {
        leakage *= stackedDieCount;

        double tsvReadRampInput;
        double tsvWriteRampInput;

        // Normally senseAmpMuxLev2 is the last driver from Mat
        // or mux from global sense amp if used
        //tsvReadRampInput = mat.subarray.senseAmpMuxLev2.rampOutput;
        tsvReadRampInput = 1e20;

        // Bank is the end unit for NVSIM, so we assume something external
        // is fully driving the input data values
        tsvWriteRampInput = infinite_ramp; 

        // Add TSV energy ~ Assume outside of bank area
        // Use comparator for tag read ramp input with internal sensing
        tsvArray.CalculateLatencyAndPower(tsvReadRampInput, tsvWriteRampInput); 

        //int numControlBits = (int)(log2((double)stackedDieCount + 0.1));
        int numControlBits = stackedDieCount;
        int numAddressBits = (int)(log2((double)capacity / blockSize / associativity / stackedDieCount) + 0.1);
        int numDataBits = blockSize * 2; // Read and write TSVs

        // Fine-granularity has predecoders on logic layer
        if (partitionGranularity == 1) {
            numAddressBits = 0;
        }

        double redundancyFactor = inputParameter->tsvRedundancy;
        tsvArray.numTotalBits = (int)((double)(numControlBits + numAddressBits + numDataBits) * redundancyFactor);
        tsvArray.numAccessBits = (int)((double)(numControlBits + numAddressBits + blockSize) * redundancyFactor);
        tsvArray.numReadBits = (int)((double)(numControlBits + numAddressBits) * redundancyFactor);
        tsvArray.numDataBits = (int)((double)(blockSize) * redundancyFactor);

        // Always assume worst case going to furthest die
        readLatency += (stackedDieCount-1) * tsvArray.readLatency
                     + (stackedDieCount-1) * tsvArray.writeLatency;
        writeLatency += (stackedDieCount-1) * tsvArray.writeLatency; 
        resetLatency += (stackedDieCount-1) * tsvArray.writeLatency; 
        setLatency += (stackedDieCount-1) * tsvArray.writeLatency; 
        refreshLatency += (stackedDieCount-1) * tsvArray.writeLatency;

        // Also assume worst energy
        readDynamicEnergy += tsvArray.numReadBits * (stackedDieCount-1) * tsvArray.writeDynamicEnergy + tsvArray.numDataBits * tsvArray.readDynamicEnergy * (stackedDieCount-1);
        writeDynamicEnergy += tsvArray.numAccessBits * (stackedDieCount-1) * tsvArray.writeDynamicEnergy;
        resetDynamicEnergy += tsvArray.numAccessBits * (stackedDieCount-1) * tsvArray.resetDynamicEnergy;
        setDynamicEnergy += tsvArray.numAccessBits * (stackedDieCount-1) * tsvArray.setDynamicEnergy;
        refreshDynamicEnergy += tsvArray.numReadBits * (stackedDieCount-1) * tsvArray.writeDynamicEnergy;

        leakage += tsvArray.numTotalBits * (stackedDieCount-1) * tsvArray.leakage;
    }

    if (cell->memCellType == eDRAM) {
        if (refreshLatency > cell->retentionTime) {
            invalid = true;
        }
    }
}

BankWithoutHtree & BankWithoutHtree::operator=(const BankWithoutHtree &rhs) {
	Bank::operator=(rhs);
	numAddressBit = rhs.numAddressBit;
	numAddressBitRouteToMat = rhs.numAddressBitRouteToMat;
	numDataBitRouteToMat = rhs.numDataBitRouteToMat;
	return *this;
}

