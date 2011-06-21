/*
 *  sim.h
 *  peer
 *
 *  Created by Leopold Parts on 16/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <Eigen/Eigen>
#include "array_helper.h"

using namespace Eigen;


struct sSimulation {
	//resulting expression 
	PMatrix expr;
	//weights
	PMatrix W;
	//latent factors
	PMatrix X;
	//noise
	PMatrix Eps;
};


struct sSparseSimulation: sSimulation {
	//true prior on connectivity matrix
	PMatrix pi;
	//true simulated connectivity matrix
	PMatrix Z;
};

//simulate from standard linear model
sSimulation simulate_expressionVBFA(int N, int D, int K,double sigma=0.1);
//simulate from a sparse model
sSparseSimulation simulate_expressionSPARSEFA(int N, int D, int K,double sigma=0.1,double sparsity=0.001,double fpr=0.1,double fnr=0.1);
