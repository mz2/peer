/*
 *  sim.h
 *  peer
 *
 *  Created by Leopold Parts on 16/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include <Eigen/Eigen>

using namespace Eigen;
using Eigen::MatrixXf;


struct sSimulation {
	MatrixXf expr;
	MatrixXf W;
	MatrixXf X;
	MatrixXf Eps;
};



MatrixXf randn(int n, int m);
sSimulation simulate_expression(int N, int D, int K,double sigma=0.1);

