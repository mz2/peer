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





sSparseSimulation simulate_expressionSPARSEFA(int N, int D, int K,int NC,double sigma,double sparsity,double fpr,double fnr)
{
	//Sparse factor analysis simulation
	//1. create a prior connectivity matrix Pi whihc is random (sparsity) with FPR and FNR as specified
	//2. sample from the matrix yielding true indicator matrix Z
	//3. create data using Z, Y ~ ((W.*Z)* X) + noise
	sSparseSimulation RV;
	RV.Z = PMatrix(K,D);
	RV.pi =PMatrix(K,D); 
	RV.W = PMatrix(K,D);
	
	
	//1. create true sparsity prior
	for (int i=0;i<K;i++)
		for (int j=0;j<D;j++)
		{
			double rv = ((double)rand())/RAND_MAX;
			if (rv<sparsity)
				RV.pi(i,j) = (1-fpr);
			else 
				RV.pi(i,j) = fnr;
			//sample from the pi guy
			double rv2 = ((double)rand())/RAND_MAX;
			if (rv2<RV.pi(i,j))
				RV.Z(i,j) = 1;
			else
				RV.Z(i,j) = 0;
			//draw sparse W also
			RV.W(i,j) = RV.Z(i,j) * randn();
		}
	//2. create random factors
	RV.X = randn(N,K);
	RV.expr = RV.X*RV.W;
	//3. covariaes ?
	if (NC>0)
	{
		RV.Xcov = randn(N,NC);
		RV.Wcov = randn(NC,D);
		RV.expr+= RV.Xcov*RV.Wcov;
	}
	//noise
	RV.Eps = sigma*randn(N,D);
	RV.expr += RV.Eps;
	return RV;
}

sSimulation simulate_expressionVBFA(int N, int D, int K,int NC, double sigma)
/*
 Simulate an expresison matrix with N entries, D dimensions and K factors
 */
{	
	//1. create factors
	sSimulation RV;
	RV.X = randn(N,K);
	RV.W = randn(K,D);
	RV.Eps = sigma*randn(N,D);
	
	RV.expr = RV.X*RV.W + RV.Eps;
	if(NC>0)
	{
		RV.Xcov = randn(N,NC);
		RV.Wcov = randn(NC,D);
		RV.expr+= RV.Xcov*RV.Wcov;
	}
	return RV;
}


