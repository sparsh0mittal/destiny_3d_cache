// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <cassert>
#include "InputParameter.h"
#include "MemCell.h"
#include "RowDecoder.h"
#include "Precharger.h"
#include "OutputDriver.h"
#include "SenseAmp.h"
#include "Technology.h"
#include "BasicDecoder.h"
#include "PredecodeBlock.h"
#include "SubArray.h"
#include "Mat.h"
#include "BankWithHtree.h"
#include "BankWithoutHtree.h"
#include "Wire.h"
#include "Result.h"
#include "formula.h"
#include "macros.h"
#include "TSV.h"

using namespace std;

InputParameter *inputParameter;
Technology *tech;
Technology *devtech;
MemCell *cell;
Wire *localWire;
Wire *globalWire;
MemCell **sweepCells;

void applyConstraint();
int nvsim(ofstream& outputFile, string inputFileName, long long& numSolution, Result *bestDataResults, Result *bestTagResults);

void tsvVerif(InputParameter *inputParameter)
{
    int techNodes[] = {90,65,45,32,22};
    int nodeCount = 5;
    bool interpolate = true;
    bool buffered = false;

    // 4 types of TSVs at each node
    // -- Aggressive interconnect, ITRS projection
    // -- Conservative interconnect, ITRS projection
    // -- Aggressive interconnect, Industry reported
    // -- Conservative interconnect, Industry reported
    for (int wiretype = 0; wiretype < 2; wiretype++) {
        for (int layers = 2; layers <= 16; layers *= 2) {
            for (int node = 0; node < nodeCount; node++) {

                // 0 -- Aggressive, 1 -- Conservative
                inputParameter->maxLocalWireType = wiretype;
                inputParameter->maxGlobalWireType = wiretype;
                
                // 0 -- ITRS, 1 -- Industry Reported
                inputParameter->localTsvProjection = 0;
                inputParameter->globalTsvProjection = 1;

                tech = new Technology();
                tech->Initialize(techNodes[node], HP, inputParameter);
                tech->SetLayerCount(inputParameter, layers);

                if (interpolate) {
                    Technology techHigh;
                    double alpha = 0;
                    if (techNodes[node] > 65) { // 65 nm < technology node <= 90 nm
                        techHigh.Initialize(90, inputParameter->deviceRoadmap, inputParameter);
                        alpha = (techNodes[node] - 65.0) / 25;
                    } else if (techNodes[node] > 45) { // 45 nm < technology node <= 65 nm
                        techHigh.Initialize(65, inputParameter->deviceRoadmap, inputParameter);
                        alpha = (techNodes[node] - 45.0) / 20;
                    } else if (techNodes[node] >= 32) { // 32 nm < technology node <= 45 nm
                        techHigh.Initialize(45, inputParameter->deviceRoadmap, inputParameter);
                        alpha = (techNodes[node] - 32.0) / 13;
                    } else if (techNodes[node] >= 22) { // 22 nm < technology node <= 32 nm
                        techHigh.Initialize(32, inputParameter->deviceRoadmap, inputParameter);
                        alpha = (techNodes[node] - 22.0) / 10;
                    }

                    tech->InterpolateWith(techHigh, alpha);
                }

                TSV tsv1;

                tsv1.Initialize(Fine, buffered);
                tsv1.CalculateArea();
                tsv1.CalculateLatencyAndPower(infinite_ramp, infinite_ramp);

                TSV tsv2;

                tsv2.Initialize(Coarse, buffered);
                tsv2.CalculateArea();
                tsv2.CalculateLatencyAndPower(infinite_ramp, infinite_ramp);

                cout << layers << "," << techNodes[node] << "," << wiretype << ",0,"
                     << tsv1.res << "," << tsv1.cap * 1e12 << "e-12," << tsv1.C_load_TSV * 1e12 << "e-12," << tsv1.num_gates << ","
                     << tsv1.area * 1e12 << "e-12," << tsv1.readDynamicEnergy * 1e12 << "e-12,"
                     << tsv1.readLatency * 1e12 << "e-12," << tsv1.leakage * 1e12 << endl; 
                cout << layers << "," << techNodes[node] << "," << wiretype << ",1,"
                     << tsv2.res << "," << tsv2.cap * 1e12 << "e-12," << tsv2.C_load_TSV * 1e12 << "e-12," << tsv2.num_gates << ","
                     << tsv2.area * 1e12 << "e-12," << tsv2.readDynamicEnergy * 1e12 << "e-12,"
                     << tsv2.readLatency * 1e12 << "e-12," << tsv2.leakage * 1e12 << endl; 
            }
        }
    }

    exit(0);
}

int main(int argc, char *argv[])
{
	cout << fixed << setprecision(3);
	string inputFileName;

	if (argc == 1) {
		inputFileName = "nvsim.cfg";
		cout << "Default configuration file (nvsim.cfg) is loaded" << endl;
	} else {
		inputFileName = argv[1];
		cout << "User-defined configuration file (" << inputFileName << ") is loaded" << endl;
	}
	cout << endl;

	inputParameter = new InputParameter();
	RESTORE_SEARCH_SIZE;
	inputParameter->ReadInputParameterFromFile(inputFileName);

    //tsvVerif(inputParameter);

	tech = new Technology();
	tech->Initialize(inputParameter->processNode, inputParameter->deviceRoadmap, inputParameter);

	Technology techHigh;
	double alpha = 0;
	if (inputParameter->processNode > 200){
		// TO-DO: technology node > 200 nm
	} else if (inputParameter->processNode > 120) { // 120 nm < technology node <= 200 nm
		techHigh.Initialize(200, inputParameter->deviceRoadmap, inputParameter);
		alpha = (inputParameter->processNode - 120.0) / 60;
	} else if (inputParameter->processNode > 90) { // 90 nm < technology node <= 120 nm
		techHigh.Initialize(120, inputParameter->deviceRoadmap, inputParameter);
		alpha = (inputParameter->processNode - 90.0) / 30;
	} else if (inputParameter->processNode > 65) { // 65 nm < technology node <= 90 nm
		techHigh.Initialize(90, inputParameter->deviceRoadmap, inputParameter);
		alpha = (inputParameter->processNode - 65.0) / 25;
	} else if (inputParameter->processNode > 45) { // 45 nm < technology node <= 65 nm
		techHigh.Initialize(65, inputParameter->deviceRoadmap, inputParameter);
		alpha = (inputParameter->processNode - 45.0) / 20;
	} else if (inputParameter->processNode >= 32) { // 32 nm < technology node <= 45 nm
		techHigh.Initialize(45, inputParameter->deviceRoadmap, inputParameter);
		alpha = (inputParameter->processNode - 32.0) / 13;
	} else if (inputParameter->processNode >= 22) { // 22 nm < technology node <= 32 nm
		techHigh.Initialize(32, inputParameter->deviceRoadmap, inputParameter);
		alpha = (inputParameter->processNode - 22.0) / 10;
	} else {
		//TO-DO: technology node < 22 nm
	}

	tech->InterpolateWith(techHigh, alpha);

    /* Open output file for full_exploration. */
    ofstream outputFile;
	string outputFileName;
	if (inputParameter->optimizationTarget == full_exploration) {
		/*stringstream temp;
		temp << inputParameter->outputFilePrefix << "_" << inputParameter->capacity / 1024 << "K_" << inputParameter->wordWidth
				<< "_" << inputParameter->associativity;
		if (inputParameter->internalSensing)
			temp << "_IN";
		else
			temp << "_EX";
		//if (cell->readMode)
		//	temp << "_VOL";
		//else
		//	temp << "_CUR";
		temp << ".csv";
		outputFileName = temp.str();*/
        int extIdx = inputFileName.find_last_of("."); 
        outputFileName = inputFileName.substr(0, extIdx) + ".csv";
		outputFile.open(outputFileName.c_str(), ofstream::out | ofstream::trunc);
        if (!outputFile.is_open()) {
            cout << "Could not open file " << outputFileName << "!" << endl;
            exit(-1);
        }
	}

    int numCellTypes = inputParameter->fileMemCell.size();
    sweepCells = new MemCell*[numCellTypes];

    Result **bestDataResults = new Result * [numCellTypes];
    Result **bestTagResults = new Result * [numCellTypes];

    int failures = 0;
    long long totalSolutions = 0;
    for (int cellIdx = 0; cellIdx < numCellTypes; cellIdx++) {
        long long solutions = 0;
        sweepCells[cellIdx] = new MemCell();
        sweepCells[cellIdx]->ReadCellFromFile(inputParameter->fileMemCell[cellIdx]);
        sweepCells[cellIdx]->ApplyPVT();
    //	cell->CellScaling(inputParameter->processNode);
        cell = sweepCells[cellIdx];

        /* In most cases device technology is the same as the peripheral technology. */
        devtech = tech;

        if (cell->memCellType == eDRAM && false) {
            devtech = new Technology();
            devtech->Initialize(inputParameter->processNode, EDRAM, inputParameter);
        }

        if (cellIdx == 0) // Print once only
            inputParameter->PrintInputParameter();

        cout << "Using cell file: " << inputParameter->fileMemCell[cellIdx] << endl;
    
        bestDataResults[cellIdx] = new Result[(int)full_exploration];
        bestTagResults[cellIdx] = new Result[(int)full_exploration];

        /* Find the best solutions, pruned solutions, or all solutions for this cell type. */
        if (nvsim(outputFile, inputFileName, solutions, bestDataResults[cellIdx], bestTagResults[cellIdx])) {
            failures++;
        } else {
            totalSolutions += solutions;
        }

        if (cell->memCellType == eDRAM && false) {
            delete devtech;
        }
    }

    /* Compare against results from previous cell types. */
    if (inputParameter->optimizationTarget == full_exploration 
        && inputParameter->isPruningEnabled
        && inputParameter->doublePrune) {
        /* Pick the best of the best over all the cell types for each x,y,z pruning triple. */
    } else if (inputParameter->optimizationTarget != full_exploration) {
        /* Pick the best of the best for each optimization target. */
        for (int cellIdx = 1; cellIdx < numCellTypes; cellIdx++) {
            for (int tgtIdx = 0; tgtIdx < (int)full_exploration; tgtIdx++) {
                bool updated = bestDataResults[0][tgtIdx].compareAndUpdate(bestDataResults[cellIdx][tgtIdx]);
                
                if (updated && !inputParameter->allowDifferentTagTech) {
                    /* Make sure these match the case in compareAndUpdate. */
                    *(bestTagResults[0][tgtIdx].bank) = *(bestTagResults[cellIdx][tgtIdx].bank);
                    *(bestTagResults[0][tgtIdx].localWire) = *(bestTagResults[cellIdx][tgtIdx].localWire);
                    *(bestTagResults[0][tgtIdx].globalWire) = *(bestTagResults[cellIdx][tgtIdx].globalWire);
                    bestDataResults[0][tgtIdx].cellTech = sweepCells[cellIdx];
                    bestTagResults[0][tgtIdx].cellTech = sweepCells[cellIdx];
                } else if (updated && inputParameter->allowDifferentTagTech) {
                    bestDataResults[0][tgtIdx].cellTech = sweepCells[cellIdx];
                }

                /* Find the best tag for each target as well. */
                if (inputParameter->allowDifferentTagTech) {
                    updated = bestTagResults[0][tgtIdx].compareAndUpdate(bestTagResults[cellIdx][tgtIdx]);
                    if (updated) 
                        bestTagResults[0][tgtIdx].cellTech = sweepCells[cellIdx];
                }
            }
        }
    }

	if (inputParameter->optimizationTarget != full_exploration) {
		if (totalSolutions > 0 && !inputParameter->printAllOptimals) {
            cell = bestDataResults[0][inputParameter->optimizationTarget].cellTech;
            //cell->PrintCell();

			if (inputParameter->designTarget == cache)
				bestDataResults[0][inputParameter->optimizationTarget].printAsCache(bestTagResults[0][inputParameter->optimizationTarget], inputParameter->cacheAccessMode);
			else
				bestDataResults[0][inputParameter->optimizationTarget].print();
        } else if (totalSolutions > 0) {
            //cell->PrintCell();

            for (int tgtIdx = 0; tgtIdx < (int)full_exploration; tgtIdx++) {
                cell = bestDataResults[0][tgtIdx].cellTech;

                if (inputParameter->designTarget == cache)
                    bestDataResults[0][tgtIdx].printAsCache(bestTagResults[0][tgtIdx], inputParameter->cacheAccessMode);
                else
                    bestDataResults[0][tgtIdx].print();
            } 
		} else {
			cout << "No valid solutions." << endl;
		}
		cout << endl << "Finished!" << endl;
	} else {
		cout << endl << outputFileName << " generated successfully!" << endl;
		if (inputParameter->isPruningEnabled) {
			cout << "The results are pruned" << endl;
		} else {
			int solutionMultiplier = 1;
			if (inputParameter->designTarget == cache)
				solutionMultiplier = 8;
			cout << totalSolutions * solutionMultiplier << " solutions in total" << endl;
		}
	}

	if (outputFile.is_open())
		outputFile.close();

	return 0;
}

int nvsim(ofstream& outputFile, string inputFileName, long long& numSolution, Result *bestDataResults, Result *bestTagResults)
{
	applyConstraint();

	int numRowMat, numColumnMat, numActiveMatPerRow, numActiveMatPerColumn;
	int numRowSubarray, numColumnSubarray, numActiveSubarrayPerRow, numActiveSubarrayPerColumn;
	int muxSenseAmp, muxOutputLev1, muxOutputLev2, numRowPerSet;
	int areaOptimizationLevel;							/* actually BufferDesignTarget */
	int localWireType, globalWireType;					/* actually WireType */
	int localWireRepeaterType, globalWireRepeaterType;	/* actually WireRepeaterType */
	int isLocalWireLowSwing, isGlobalWireLowSwing;		/* actually boolean value */
    int stackedDieCount, partitionGranularity;

	long long capacity;
	long blockSize;
	int associativity;

    long long numDesigns = 0;

	/* for cache data array, memory array */
	//Result *bestDataResults = new Result[(int)full_exploration];	/* full_exploration is always set as the last element in the enum, so if full_exploration is 8, what we want here is a 0-7 array, which is correct */
	Bank *dataBank;
	for (int i = 0; i < (int)full_exploration; i++) {
		bestDataResults[i].optimizationTarget = (OptimizationTarget)i;
        bestDataResults[i].cellTech = cell;
    }

	/* for cache tag array only */
	//Result *bestTagResults = new Result[(int)full_exploration];	/* full_exploration is always set as the last element in the enum, so if full_exploration is 8, what we want here is a 0-7 array, which is correct */
	Bank *tagBank = NULL;
	for (int i = 0; i < (int)full_exploration; i++) {
		bestTagResults[i].optimizationTarget = (OptimizationTarget)i;
        bestTagResults[i].cellTech = cell;
    }

	localWire = new Wire();
	globalWire = new Wire();

    partitionGranularity = inputParameter->partitionGranularity;

	/* search tag first */
	if (inputParameter->designTarget == cache) {
		/* need to design the tag array */
		REDUCE_SEARCH_SIZE;
		/* calculate the tag configuration */
		int numDataSet = inputParameter->capacity * 8 / inputParameter->wordWidth / inputParameter->associativity;
		int numIndexBit = (int)(log2(numDataSet) + 0.1);
		int numOffsetBit = (int)(log2(inputParameter->wordWidth / 8) + 0.1);
		INITIAL_BASIC_WIRE;
		/* Simulate tag */
		BIGFOR {
            tech->SetLayerCount(inputParameter, stackedDieCount);
			blockSize = TOTAL_ADDRESS_BIT - numIndexBit - numOffsetBit;
			blockSize += 2;		/* add dirty bits and valid bits */
			if (blockSize / (numActiveMatPerRow * numActiveMatPerColumn * numActiveSubarrayPerRow * numActiveSubarrayPerColumn) == 0) {
				/* To aggressive partitioning */
				continue;
			}
			if (blockSize % (numActiveMatPerRow * numActiveMatPerColumn * numActiveSubarrayPerRow * numActiveSubarrayPerColumn)) {
				blockSize = (blockSize / (numActiveMatPerRow * numActiveMatPerColumn * numActiveSubarrayPerRow * numActiveSubarrayPerColumn) + 1)
						* (numActiveMatPerRow * numActiveMatPerColumn * numActiveSubarrayPerRow * numActiveSubarrayPerColumn);
			}
			capacity = (long long)inputParameter->capacity * 8 / inputParameter->wordWidth * blockSize;
			associativity = inputParameter->associativity;
			CALCULATE(tagBank, tag);
            numDesigns++;
			if (!tagBank->invalid) {
				Result tempResult;
				VERIFY_TAG_CAPACITY;
				numSolution++;
				UPDATE_BEST_TAG;
			}
			delete tagBank;
		}

		if (numSolution > 0) {
			Bank * trialBank;
			Result tempResult;
			/* refine local wire type */
			REFINE_LOCAL_WIRE_FORLOOP {
				localWire->Initialize(inputParameter->processNode, (WireType)localWireType,
						(WireRepeaterType)localWireRepeaterType, inputParameter->temperature,
						(bool)isLocalWireLowSwing);
				for (int i = 0; i < (int)full_exploration; i++) {
					LOAD_GLOBAL_WIRE(bestTagResults[i]);
					TRY_AND_UPDATE(bestTagResults[i], tag);
				}
			}
			/* refine global wire type */
			REFINE_GLOBAL_WIRE_FORLOOP {
				globalWire->Initialize(inputParameter->processNode, (WireType)globalWireType,
						(WireRepeaterType)globalWireRepeaterType, inputParameter->temperature,
						(bool)isGlobalWireLowSwing);
				for (int i = 0; i < (int)full_exploration; i++) {
					LOAD_LOCAL_WIRE(bestTagResults[i]);
					TRY_AND_UPDATE(bestTagResults[i], tag);
				}
			}
		}

		if (numSolution == 0) {
			cout << "No valid solutions for tags." << endl;
			cout << endl << "Finished!" << endl;
			if (localWire) delete localWire;
			if (globalWire) delete globalWire;
			//outputFile.close();
			return 1;
		} else {
			numSolution = 0;
            numDesigns = 0;
			RESTORE_SEARCH_SIZE;
			inputParameter->ReadInputParameterFromFile(inputFileName);	/* just for restoring the search space */
			applyConstraint();
		}
	}

	/* adjust cache data array parameters according to the access mode */
	capacity = (long long)inputParameter->capacity * 8;
	blockSize = inputParameter->wordWidth;
	associativity = inputParameter->associativity;
	if (inputParameter->designTarget == cache) {
		switch (inputParameter->cacheAccessMode) {
		case sequential_access_mode:
			/* already knows which way to access */
			associativity = 1;
			break;
		case fast_access_mode:
			/* load the entire set as a single word */
			blockSize *= associativity;
			associativity = 1;
			break;
		default:	/* Normal */
			/* Normal access does not allow one set be distributed into multiple rows
			 * otherwise, the row activation has to be delayed until the hit signals arrive.
			 */
			inputParameter->minNumRowPerSet = inputParameter->maxNumRowPerSet = 1;
		}
	}

	/* adjust block size is it is SLC NAND flash or DRAM memory chip */
	if (inputParameter->designTarget == RAM_chip && (cell->memCellType == SLCNAND || cell->memCellType == DRAM)) {
		blockSize = inputParameter->pageSize;
		associativity = 1;
	}

	INITIAL_BASIC_WIRE;
	BIGFOR {
		if (blockSize / (numActiveMatPerRow * numActiveMatPerColumn * numActiveSubarrayPerRow * numActiveSubarrayPerColumn) == 0) {
			/* To aggressive partitioning */
			continue;
		}
        //if (capacity / (numRowMat * numColumnMat * numRowSubarray * numColumnSubarray) < 32*32) {
        //    // Require at least 32x32 subarrays.
        //    continue;
        //}
		CALCULATE(dataBank, data);
        numDesigns++;
		if (!dataBank->invalid) {
			Result tempResult;
			VERIFY_DATA_CAPACITY;
			numSolution++;
			UPDATE_BEST_DATA;
			if (inputParameter->optimizationTarget == full_exploration && !inputParameter->isPruningEnabled) {
				OUTPUT_TO_FILE;
			}
		}
		delete dataBank;
	}

	if (numSolution > 0) {
		Bank * trialBank;
		Result tempResult;
		/* refine local wire type */
		REFINE_LOCAL_WIRE_FORLOOP {
			localWire->Initialize(inputParameter->processNode, (WireType)localWireType,
					(WireRepeaterType)localWireRepeaterType, inputParameter->temperature,
					(bool)isLocalWireLowSwing);
			for (int i = 0; i < (int)full_exploration; i++) {
				LOAD_GLOBAL_WIRE(bestDataResults[i]);
				TRY_AND_UPDATE(bestDataResults[i], data);
			}
			if (inputParameter->optimizationTarget == full_exploration && !inputParameter->isPruningEnabled) {
				OUTPUT_TO_FILE;
			}
		}
		/* refine global wire type */
		REFINE_GLOBAL_WIRE_FORLOOP {
			globalWire->Initialize(inputParameter->processNode, (WireType)globalWireType,
					(WireRepeaterType)globalWireRepeaterType, inputParameter->temperature,
					(bool)isGlobalWireLowSwing);
			for (int i = 0; i < (int)full_exploration; i++) {
				LOAD_LOCAL_WIRE(bestDataResults[i]);
				TRY_AND_UPDATE(bestDataResults[i], data);
			}
			if (inputParameter->optimizationTarget == full_exploration && !inputParameter->isPruningEnabled) {
				OUTPUT_TO_FILE;
			}
		}
	}

	if (inputParameter->optimizationTarget == full_exploration && inputParameter->isPruningEnabled) {
		/* pruning is enabled */
		Result **** pruningResults;
		/* pruningResults[x][y][z] points to the result which is optimized for x, with constraint on y with z overhead */
		pruningResults = new Result***[(int)full_exploration];	/* full_exploration is always set as the last element in the enum, so if full_exploration is 8, what we want here is a 0-7 array, which is correct */
		for (int i = 0; i < (int)full_exploration; i++) {
			pruningResults[i] = new Result**[(int)full_exploration];
			for (int j = 0; j < (int)full_exploration; j++) {
				pruningResults[i][j] = new Result*[3];		/* 10%, 20%, and 30% overhead */
				for (int k = 0; k < 3; k++)
					pruningResults[i][j][k] = new Result;
			}
		}

		/* assign the constraints */
		for (int i = 0; i < (int)full_exploration; i++)
			for (int j = 0; j < (int)full_exploration; j++)
				for (int k = 0; k < 3; k++) {
					pruningResults[i][j][k]->optimizationTarget = (OptimizationTarget)i;
					*(pruningResults[i][j][k]->localWire) = *(bestDataResults[i].localWire);
					*(pruningResults[i][j][k]->globalWire) = *(bestDataResults[i].globalWire);
					switch ((OptimizationTarget)j) {
					case read_latency_optimized:
						pruningResults[i][j][k]->limitReadLatency = bestDataResults[j].bank->readLatency * (1 + (k + 1.0) / 10);
						break;
					case write_latency_optimized:
						pruningResults[i][j][k]->limitWriteLatency = bestDataResults[j].bank->writeLatency * (1 + (k + 1.0) / 10);
						break;
					case read_energy_optimized:
						pruningResults[i][j][k]->limitReadDynamicEnergy = bestDataResults[j].bank->readDynamicEnergy * (1 + (k + 1.0) / 10);
						break;
					case write_energy_optimized:
						pruningResults[i][j][k]->limitWriteDynamicEnergy = bestDataResults[j].bank->writeDynamicEnergy * (1 + (k + 1.0) / 10);
						break;
					case read_edp_optimized:
						pruningResults[i][j][k]->limitReadEdp = bestDataResults[j].bank->readLatency * bestDataResults[j].bank->readDynamicEnergy * (1 + (k + 1.0) / 10);
						break;
					case write_edp_optimized:
						pruningResults[i][j][k]->limitWriteEdp = bestDataResults[j].bank->writeLatency * bestDataResults[j].bank->writeDynamicEnergy * (1 + (k + 1.0) / 10);
						break;
					case area_optimized:
						pruningResults[i][j][k]->limitArea = bestDataResults[j].bank->area * (1 + (k + 1.0) / 10);
						break;
					case leakage_optimized:
						pruningResults[i][j][k]->limitLeakage = bestDataResults[j].bank->leakage * (1 + (k + 1.0) / 10);
						break;
					default:
						/* nothing should happen here */
						cout << "Warning: should not happen" << endl;
					}
				}

		for (int i = 0; i < (int)full_exploration; i++) {
			bestDataResults[i].printAsCacheToCsvFile(bestTagResults[i], inputParameter->cacheAccessMode, outputFile);
		}
		cout << "Pruning done" << endl;
		/* Run pruning here */
		/* TO-DO */

		/* delete */
		for (int i = 0; i < (int)full_exploration; i++) {
			for (int j = 0; j < (int)full_exploration; j++) {
				for (int k = 0; k < 3; k++)
					delete pruningResults[i][j][k];
				delete [] pruningResults[i][j];
			}
			delete [] pruningResults[i];
		}
	}

	/* If design constraint is applied */
	if (inputParameter->optimizationTarget != full_exploration && inputParameter->isConstraintApplied) {
		double allowedDataReadLatency = bestDataResults[read_latency_optimized].bank->readLatency * (inputParameter->readLatencyConstraint + 1);
		double allowedDataWriteLatency = bestDataResults[write_latency_optimized].bank->writeLatency * (inputParameter->writeLatencyConstraint + 1);
		double allowedDataReadDynamicEnergy = bestDataResults[read_energy_optimized].bank->readDynamicEnergy * (inputParameter->readDynamicEnergyConstraint + 1);
		double allowedDataWriteDynamicEnergy = bestDataResults[write_energy_optimized].bank->writeDynamicEnergy * (inputParameter->writeDynamicEnergyConstraint + 1);
		double allowedDataLeakage = bestDataResults[leakage_optimized].bank->leakage * (inputParameter->leakageConstraint + 1);
		double allowedDataArea = bestDataResults[area_optimized].bank->area * (inputParameter->areaConstraint + 1);
		double allowedDataReadEdp = bestDataResults[read_edp_optimized].bank->readLatency
				* bestDataResults[read_edp_optimized].bank->readDynamicEnergy * (inputParameter->readEdpConstraint + 1);
		double allowedDataWriteEdp = bestDataResults[write_edp_optimized].bank->writeLatency
				* bestDataResults[write_edp_optimized].bank->writeDynamicEnergy * (inputParameter->writeEdpConstraint + 1);
		for (int i = 0; i < (int)full_exploration; i++) {
			APPLY_LIMIT(bestDataResults[i]);
		}

		numSolution = 0;
		INITIAL_BASIC_WIRE;
		BIGFOR {
			if (blockSize / (numActiveMatPerRow * numActiveMatPerColumn * numActiveSubarrayPerRow * numActiveSubarrayPerColumn) == 0) {
				/* To aggressive partitioning */
				continue;
			}
			CALCULATE(dataBank, data);
            numDesigns++;
			if (!dataBank->invalid && dataBank->readLatency <= allowedDataReadLatency && dataBank->writeLatency <= allowedDataWriteLatency
					&& dataBank->readDynamicEnergy <= allowedDataReadDynamicEnergy && dataBank->writeDynamicEnergy <= allowedDataWriteDynamicEnergy
					&& dataBank->leakage <= allowedDataLeakage && dataBank->area <= allowedDataArea
					&& dataBank->readLatency * dataBank->readDynamicEnergy <= allowedDataReadEdp && dataBank->writeLatency * dataBank->writeDynamicEnergy <= allowedDataWriteEdp) {
				Result tempResult;
				VERIFY_DATA_CAPACITY;
				numSolution++;
				UPDATE_BEST_DATA;
			}
			delete dataBank;
		}
	}

    cout << "numSolutions = " << numSolution << " / numDesigns = " << numDesigns << endl;

	if (localWire) delete localWire;
	if (globalWire) delete globalWire;

    return 0;
}

void applyConstraint() {
	/* Check functions that are not yet implemented */
	if (inputParameter->designTarget == CAM_chip) {
		cout << "[ERROR] CAM model is still under development" << endl;
		exit(-1);
	}
	if (cell->memCellType == DRAM) {
		cout << "[ERROR] DRAM model is still under development" << endl;
		exit(-1);
	}
	if (cell->memCellType == MLCNAND) {
		cout << "[ERROR] MLC NAND flash model is still under development" << endl;
		exit(-1);
	}

	if (inputParameter->designTarget != cache && inputParameter->associativity > 1) {
		cout << "[WARNING] Associativity setting is ignored for non-cache designs" << endl;
		inputParameter->associativity = 1;
	}

	if (!isPow2(inputParameter->associativity)) {
		cout << "[ERROR] The associativity value has to be a power of 2 in this version" << endl;
		//exit(-1);
	}

	if (inputParameter->routingMode == h_tree && inputParameter->internalSensing == false) {
		cout << "[ERROR] H-tree does not support external sensing scheme in this version" << endl;
		exit(-1);
	}
/*
	if (inputParameter->globalWireRepeaterType != repeated_none && inputParameter->internalSensing == false) {
		cout << "[ERROR] Repeated global wire does not support external sensing scheme" << endl;
		exit(-1);
	}
*/

	/* TO-DO: more rules to add here */
}
