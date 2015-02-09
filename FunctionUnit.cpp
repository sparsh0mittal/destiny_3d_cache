//Copyright (c) 2015-2016, UT-Battelle, LLC. See LICENSE file in the top-level directory
// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#include "FunctionUnit.h"
#include "formula.h"

#include <cassert>

FunctionUnit::FunctionUnit() {
	height = width = 0;
	area = 0;
	readLatency = writeLatency = 0;
	readDynamicEnergy = writeDynamicEnergy = 0;
	leakage = 0;

	resetLatency = setLatency = 0;
    refreshLatency = 0;
	resetDynamicEnergy = setDynamicEnergy = 0;
    refreshDynamicEnergy = 0;
	cellReadEnergy = 0;
	cellSetEnergy = cellResetEnergy = 0;
}

FunctionUnit::~FunctionUnit() {
	// TODO Auto-generated destructor stub
}

void FunctionUnit::PrintProperty() {
	cout << "Area = " << height*1e6 << "um x " << width*1e6 << "um = " << area*1e6 << "mm^2" << endl;
	cout << "Timing:" << endl;
	cout << " -  Read Latency = " << readLatency*1e9 << "ns" << endl;
	cout << " - Write Latency = " << writeLatency*1e9 << "ns" << endl;
	cout << "Power:" << endl;
	cout << " -  Read Dynamic Energy = " << readDynamicEnergy*1e12 << "pJ" << endl;
	cout << " - Write Dynamic Energy = " << writeDynamicEnergy*1e12 << "pJ" << endl;
	cout << " - Leakage Power = " << leakage*1e3 << "mW" << endl;
}

int FunctionUnit::logical_effort(
    int num_gates_min,
    double g,
    double F,
    double * w_n,
    double * w_p,
    double C_load,
    double p_to_n_sz_ratio,
    double max_w_nmos,
    Technology tech)
{
  int num_gates = (int) (log(F) / log(fopt));

  // check if num_gates is odd. if so, add 1 to make it even
  num_gates+= (num_gates % 2) ? 1 : 0;
  num_gates = MAX(num_gates, num_gates_min);

  // recalculate the effective fanout of each stage
  double f = pow(F, 1.0 / num_gates);
  int    i = num_gates - 1;
  double C_in = C_load / f;
  w_n[i]  = (1.0 / (1.0 + p_to_n_sz_ratio)) * C_in / CalculateGateCap(1, tech);
  w_n[i]  = MAX(w_n[i], MIN_NMOS_SIZE * tech.featureSize);
  w_p[i]  = p_to_n_sz_ratio * w_n[i];

  if (w_n[i] > max_w_nmos) // && !g_ip->is_3d_mem)
  {
    //double C_ld = gate_C((1 + p_to_n_sz_ratio) * max_w_nmos, 0, is_dram_, false, is_wl_tr_);
    double C_ld = CalculateGateCap((1 + p_to_n_sz_ratio) * max_w_nmos, tech);
    //F = g * C_ld / gate_C(w_n[0] + w_p[0], 0, is_dram_, false, is_wl_tr_);
    F = g * C_ld / CalculateGateCap(w_n[0] + w_p[0], tech);
    num_gates = (int) (log(F) / log(fopt)) + 1;
    num_gates+= (num_gates % 2) ? 1 : 0;
    num_gates = MAX(num_gates, num_gates_min);
    f = pow(F, 1.0 / (num_gates - 1));
    i = num_gates - 1;
    w_n[i]  = max_w_nmos;
    w_p[i]  = p_to_n_sz_ratio * w_n[i];
  }

  for (i = num_gates - 2; i >= 1; i--)
  {
    w_n[i] = MAX(w_n[i+1] / f, MIN_NMOS_SIZE * tech.featureSize);
    w_p[i] = p_to_n_sz_ratio * w_n[i];
  }

  assert(num_gates <= MAX_NUMBER_GATES_STAGE);
  return num_gates;
}


