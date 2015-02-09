//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from CACTI-3DD, (c) 2012 Hewlett-Packard Development Company, L.P.
//See LICENSE_CACTI3DD file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.

#include "TSV.h"
#include "formula.h"
#include <cassert>

TSV::TSV()
{
    for (int i = 0; i < MAX_NUMBER_GATES_STAGE; i++) {
        w_TSV_n[i] = 0;
        w_TSV_p[i] = 0;
    }

    numTotalBits = 0;
    numAccessBits = 0;

    initialized = false;
    invalid = true;
}

TSV::~TSV()
{
}

void TSV::Initialize(TSV_type tsv_type, bool buffered)
{
    int num_gates_min = 1;
    double min_w_pmos = tech->pnSizeRatio * MIN_NMOS_SIZE * tech->featureSize;
    num_gates = 1;

    cap = tech->capTSV[tsv_type];
    res = tech->resTSV[tsv_type];
    min_area = tech->areaTSV[tsv_type] * 1e-12;

    if (!buffered) {
        num_gates = 0;
    } else {
        double first_buf_stg_coef = 5; // To tune the total buffer delay.
        w_TSV_n[0] = MIN_NMOS_SIZE * first_buf_stg_coef * tech->featureSize;
        w_TSV_p[0] = w_TSV_n[0] * tech->pnSizeRatio;

        //BEOL parasitics in Katti's E modeling and charac. of TSV.  Needs further detailed values.
        //double res_beol    = 0.1;//inaccurate
        //double cap_beol    = 1e-15;

        //C_load_TSV    = cap_beol + cap + cap_beol + gate_C(g_tp.min_w_nmos_ + min_w_pmos, 0);

        C_load_TSV    = cap + CalculateGateCap(MIN_NMOS_SIZE * tech->featureSize + min_w_pmos, *tech); //+ 57.5e-15;
#ifdef NVSIM3DDEBUG
            cout << " The input cap of 1st buffer: " << CalculateGateCap(w_TSV_n[0] + w_TSV_p[0], *tech) * 1e15 << " fF";
#endif

        F = C_load_TSV / CalculateGateCap(w_TSV_n[0] + w_TSV_p[0], *tech);
#ifdef NVSIM3DDEBUG
        cout<<"\nF  is "<<F<<" \n";
#endif

        //Obtain buffer chain stages using logic effort function. Does stage number have to be even?
        num_gates = logical_effort(
            num_gates_min,
            1,
            F,
            w_TSV_n,
            w_TSV_p,
            C_load_TSV,
            tech->pnSizeRatio,
            MAX_NMOS_SIZE * tech->featureSize,
            *tech
        );
    }

    initialized = true;
    if (num_gates > MAX_NUMBER_GATES_STAGE) {
        invalid = true;
    }
}

void TSV::CalculateArea()
{
    //Obtain the driver chain area and leakage power for TSV
    double Vdd = tech->vdd;
    double cumulative_area = 0;
    double cumulative_curr = 0;  // cumulative leakage current
    double cumulative_curr_Ig = 0;  // cumulative leakage current
    buffer_area_height = 50 * tech->featureSize; // the assigned height for memory cell (5um), is it correct to use it here?
    double temperature = (double)inputParameter->temperature;
    double tempWidth, tempHeight;


    int i;
    for (i = 0; i < num_gates; i++)
    {
        cumulative_area += CalculateGateArea(INV, 1, w_TSV_n[i], w_TSV_p[i], tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech, &tempHeight, &tempWidth);

#ifdef NVSIM3DDEBUG
        cout << "\n\tArea up to the " << i+1 << " stages is: " << cumulative_area * 1e12 << " um2";
#endif
        cumulative_curr += CalculateGateLeakage(INV, 1, w_TSV_n[i], w_TSV_p[i], temperature, *tech);
        // TODO -- What's the difference?
        cumulative_curr_Ig += CalculateGateLeakage(INV, 1, w_TSV_n[i], w_TSV_p[i], temperature, *tech);
    }
    //power.readOp.leakage = cumulative_curr * Vdd;
    leakage = cumulative_curr_Ig * Vdd;

    buffer_area = cumulative_area;
    buffer_area_width = buffer_area / buffer_area_height;

    TSV_metal_area = min_area * 3.1416/16;

    // TODO -- Understand this better
    if( buffer_area < (min_area - TSV_metal_area) )
        area = min_area;
    else
        area = buffer_area + TSV_metal_area;

}


void TSV::CalculateLatencyAndPower(double _rampInputRead, double _rampInputWrite)
{
    assert(_rampInputRead != 0 && _rampInputWrite != 0);

    // Assume we are using the same TSV type/size/etc. we are just driving in a
    // different direction
    _CalculateLatencyAndPower(_rampInputRead, readDynamicEnergy, readLatency);
    _CalculateLatencyAndPower(_rampInputWrite, writeDynamicEnergy, writeLatency);

    // reset, set, erase are same driving direction as write
    resetDynamicEnergy = setDynamicEnergy = writeDynamicEnergy;
    resetLatency = setLatency = writeLatency;
}


void TSV::_CalculateLatencyAndPower(double _rampInput, double &_dynamicEnergy, double &_latency)
{
    //Buffer chain delay and Dynamic Power
    double delay = 0.0;
    double rd, tf, this_delay, c_load, c_intrinsic; 
    double capInput, capOutput;
    double rampInput, rampOutput;
    double beta = 0.5; /* Carried over from CACTI3DD. */

    if (num_gates > 0) {
        rd = CalculateOnResistance(w_TSV_n[0], NMOS, inputParameter->temperature, *tech);
        CalculateGateCapacitance(INV, 1, w_TSV_n[1], w_TSV_p[1], tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech, &capInput, &capOutput);

        c_load = capInput + capOutput;
        c_intrinsic = CalculateDrainCap(w_TSV_p[0], PMOS, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech) 
                     + CalculateDrainCap(w_TSV_n[0], NMOS, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech);
        tf = rd * (c_intrinsic + c_load);

        // NVSIM3D - _rampInput should be subarray's senseAmpMuxLev2 rampOutput
        rampInput = _rampInput;
        this_delay = horowitz(tf, beta, rampInput, &rampOutput);
        delay += this_delay;

        double Vdd = tech->vdd;
        _dynamicEnergy = (c_load + c_intrinsic) * Vdd * Vdd;

        int i;
        for (i = 1; i < num_gates - 1; ++i)
        {
            rd = CalculateOnResistance(w_TSV_n[i], NMOS, inputParameter->temperature, *tech);
            CalculateGateCapacitance(INV, 1, w_TSV_n[i+1], w_TSV_p[i+1], tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech, &capInput, &capOutput);

            c_load = capInput + capOutput;
            c_intrinsic = CalculateDrainCap(w_TSV_p[i], PMOS, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech) 
                       + CalculateDrainCap(w_TSV_n[i], NMOS, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech);

            tf = rd * (c_intrinsic + c_load);
            rampInput = rampOutput;
            this_delay = horowitz(tf, beta, rampInput, &rampOutput);
            delay += this_delay;

            _dynamicEnergy += (c_load + c_intrinsic) * Vdd * Vdd;
        }

        // add delay of final inverter that drives the TSV
        i = num_gates - 1;
        c_load = C_load_TSV;

        rd = CalculateOnResistance(w_TSV_n[i], NMOS, inputParameter->temperature, *tech);
        c_intrinsic = CalculateDrainCap(w_TSV_p[i], PMOS, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech) 
                     + CalculateDrainCap(w_TSV_n[i], NMOS, tech->featureSize * MAX_TRANSISTOR_HEIGHT, *tech);

        double R_TSV_out = res;
        tf = rd * (c_intrinsic + c_load) + R_TSV_out * c_load / 2;

        rampInput = rampOutput;
        this_delay = horowitz(tf, beta, rampInput, &rampOutput);
        delay  += this_delay;

        _dynamicEnergy += (c_load + c_intrinsic) * Vdd * Vdd;

        _latency = delay;
    } else {
        rampInput = _rampInput;
        c_load = cap;
        double R_TSV_out = res;
        tf = R_TSV_out * c_load / 2;
        double Vdd = tech->vdd;

        this_delay = horowitz(tf, beta, rampInput, &rampOutput);
        delay  += this_delay;

        _dynamicEnergy += (c_load) * Vdd * Vdd;

        _latency = delay;
    }
}

TSV& TSV::operator=(const TSV& rhs) {
	height = rhs.height;
	width = rhs.width;
	area = rhs.area;

	readLatency = rhs.readLatency;
	writeLatency = rhs.writeLatency;
	readDynamicEnergy = rhs.readDynamicEnergy;
	writeDynamicEnergy = rhs.writeDynamicEnergy;
	resetLatency = rhs.resetLatency;
	setLatency = rhs.setLatency;

	resetDynamicEnergy = rhs.resetDynamicEnergy;
	setDynamicEnergy = rhs.setDynamicEnergy;
	cellReadEnergy = rhs.cellReadEnergy;
	cellSetEnergy = rhs.cellSetEnergy;
	cellResetEnergy = rhs.cellResetEnergy;
	leakage = rhs.leakage;

    res = rhs.res;
    cap = rhs.cap;
    C_load_TSV = rhs.C_load_TSV;
    min_area = rhs.min_area; 
    F = rhs.F;

    num_gates = rhs.num_gates;
    memcpy(w_TSV_n, rhs.w_TSV_n, sizeof(double)*MAX_NUMBER_GATES_STAGE);
    memcpy(w_TSV_p, rhs.w_TSV_p, sizeof(double)*MAX_NUMBER_GATES_STAGE);
    numTotalBits = rhs.numTotalBits;
    numAccessBits = rhs.numAccessBits;
    numReadBits = rhs.numReadBits;
    numDataBits = rhs.numDataBits;

    TSV_metal_area = rhs.TSV_metal_area;
    buffer_area = rhs.buffer_area;
    buffer_area_height = rhs.buffer_area_height;
    buffer_area_width = rhs.buffer_area_width;

    tsv_type = rhs.tsv_type;
    initialized = rhs.initialized;
    invalid = rhs.invalid;

    return *this;
}


