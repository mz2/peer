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







sSimulation simulate_expression(int N, int D, int K,double sigma)
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


