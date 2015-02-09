//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from CACTI-3DD, (c) 2012 Hewlett-Packard Development Company, L.P.
//See LICENSE_CACTI3DD file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.

#ifndef TSV_H_
#define TSV_H_

#include "formula.h"
#include "global.h"
#include <cmath>
#include <iostream>
#include <list>


class TSV : public FunctionUnit {
public:
    TSV();
    ~TSV();
    TSV& operator=(const TSV&);

    void Initialize(TSV_type tsv_type, bool buffered = false);
    void CalculateArea();
    void CalculateLatencyAndPower(double _rampInputRead, double _rampInputWrite);

    double res; //TSV resistance
    double cap; //TSV capacitance
    double C_load_TSV; //The intrinsic load plus the load TSV is driving
    double min_area;
    double F;

    int num_gates;
    double w_TSV_n[MAX_NUMBER_GATES_STAGE];
    double w_TSV_p[MAX_NUMBER_GATES_STAGE];
    int numTotalBits; // Total number of TSVs
    int numAccessBits; // Number of input bits for a write
    int numReadBits; // Number of input bits for a read
    int numDataBits; // Number of output bits for a read

    double TSV_metal_area;
    double buffer_area;
    double buffer_area_height;
    double buffer_area_width;

    unsigned int tsv_type;
    bool initialized;
    bool invalid;

private:
    void _CalculateLatencyAndPower(double _rampInput, double &_dynamicEnergy, double &_latency);

};


#endif /* TSV_H_ */
