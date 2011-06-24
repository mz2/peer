/*
 *  main_testing.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 3/24/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "main_testing.h"
#include "vbfa.h"
#include "sparsefa.h"
#include "sim.h"

using namespace PEER;

int main (int argc, char * const argv[]) {
	
	int N = 50;
	int D = 100;
	int K = 10;
	double sigma = 0.1;
	double sparsity = 1E-2;
	double fpr= 0;
	double fnr = 0.1;
	sSparseSimulation sim = simulate_expressionSPARSEFA(N,D,K,sparsity,sigma,fpr,fnr);
	//count the number of sparse things?
	PMatrix Z = sim.Z;	
	int Zsum = (int)sum(Z);
	//plot diagnoses
	printf("Net size: NxG: %d x %d. Non-zero enries: %d",N,D,sum);
	

	//cVBFA vb = cVBFA(sim.expr,K);
	//vb.update();
	
	cSPARSEFA vbs = cSPARSEFA(sim.expr,sim.pi.transpose(),K);
	vbs.update();
	
	
	printf("hi");
}