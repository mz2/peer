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
#include "sim.h"

using namespace PEER;

int main (int argc, char * const argv[]) {
	
	int N = 10;
	int D = 100;
	int K = 50;
	double sigma = 0.1;
	double sparsity = 1E-3;
	sSimulation sim = simulate_expressionSPARSEFA(N,D,K,sparsity,sigma);
	//count the number of sparse things?
	PMatrix Z = sim.Z;
	
	int Zsum = (int)sum(Z);
	//plot diagnoses
	printf("Net size: NxG: %d x %d. Non-zero enries: %d",N,D,sum);
	
	cVBFA vb = cVBFA(sim.expr,K);
	vb.update();
	
	printf("hi");
}