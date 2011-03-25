/*
 *  sim.cpp
 *  peer
 *
 *  Created by Leopold Parts on 16/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "sim.h"
#include <Eigen/Eigen>
#include "ossolog.h"

using namespace Eigen;






sSimulation simulate_expressionSPARSEFA(int N, int D, int K,double sparsity,double sigma)
{
	//1. create random matrix with binary indicators and the weights
	sSimulation RV;
	RV.Z = PMatrix(K,D);
	RV.W = PMatrix(K,D);
	for (int i=0;i<K;i++)
		for (int j=0;j<D;j++)
		{
			//coin flip for the assignment of Z
			RV.Z(i,j) = (rand()<sparsity)?1:0;
			//draw W also
			RV.W(i,j) = RV.Z(i,j) * randomreal();
		}
	//2. create random factors
	RV.X = randn(N,K);
	RV.Eps = sigma*randn(N,D);
	RV.expr = RV.X*RV.W + RV.Eps;
	return RV;
}

sSimulation simulate_expressionVBFA(int N, int D, int K,double sigma)
/*
 Simulate an expresison matrix with N entries, D dimensions and K factors
 */
{	
	//1. create factors
	sSimulation result;
	result.X = randn(N,K);
	result.W = randn(K,D);
	result.Eps = sigma*randn(N,D);
	result.expr = result.X*result.W + result.Eps;
	return result;
}


