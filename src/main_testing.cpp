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
	int D = 10;
	int K = 3;
	int NC = 2;
	double sigma = 0.1;
	double sparsity = 2E-1;
	double fpr= 0;
	double fnr = 0;
	sSparseSimulation sim = simulate_expressionSPARSEFA(N,D,K,NC,sparsity,sigma,fpr,fnr);
	//count the number of sparse things?
	PMatrix Z = sim.Z;	
	int Zsum = (int) Z.sum();
	//plot diagnoses
	printf("Net size: NxG: %d x %d. Non-zero enries: %d",N,D,Zsum);
	

	VERBOSE = 1;

	
	/*
	cVBFA vb = cVBFA();
	vb.setNk(K);
	vb.setPhenoMean(sim.expr);
	vb.update();
	
	*/

	std::cout << "\n\n";
	
	std::cout << sim.Z;
	
	std::cout << "\n\n";
	
	std::cout << sim.pi;
	
	std::cout << "\n\n";
		
	PMatrix pi = sim.pi.transpose();
	
	cPEER vbs = cPEER();
	if (NC>0)
		vbs.setCovariates(sim.Xcov);
	vbs.setAdd_mean(true);
	vbs.setNk(K);
	vbs.setPhenoMean(sim.expr);
	//vbs.setSparsityPrior(pi);
	vbs.update();
	
	PVector test = vbs.getResidualVars();
	
	double* matrix;
	int rows; 
	int cols;
	vbs.getResidualVars(&matrix, &rows, &cols);
	
	std::cout << test;
	
	
	printf("hi");
}