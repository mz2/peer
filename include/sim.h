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
	PMatrix expr;
	PMatrix W;
	PMatrix X;
	PMatrix Eps;
};



sSimulation simulate_expression(int N, int D, int K,double sigma=0.1);

