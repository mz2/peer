/*
 *  bayesnet.h
 *  peer
 *
 *  Created by Leopold Parts on 03/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __BAYESNET_H__
#define __BAYESNET_H__

#include <Eigen/Eigen>
#include "array_helper.h"
#include <iostream>

using namespace std;
using namespace Eigen;


class cBayesNet {
public:
	int N;
	int components;
};


/**
 * Abstract base class for all Bayes Net nodes
 */
class cNode {
public:
	PMatrix E1;
	PMatrix prior;
	double bound;

	virtual double entropy();	
	virtual double calcBound(cBayesNet &net);
	virtual void update(cBayesNet &net);

#ifndef SWIG
	PMatrix getE1();
#endif	
	};


/**
 * Dirichlet Node - simple test node to implement (not used in PEER).
 */
class cDirichletNode : public cNode {
protected:
    PVector a0;
	PVector a;
	PVector lnE;
	
public:
	cDirichletNode(int dim, float prior);
	double entropy();	
	double calcBound(cBayesNet &net);
	void update(cBayesNet &net);
};


/**
 * Gamma Node - prior for precisions
 */
class cGammaNode : public cNode {
public:
    double pa;
	double pb;
	PMatrix lnE;
	PMatrix a;
	PMatrix b;

	cGammaNode();
	cGammaNode(int dim, float prior_val_a, float prior_val_b, PMatrix E1_val);
	double entropy();	
	double calcBound(cBayesNet &net);
	void update(cBayesNet &net);
	void updateMoments();
	
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
};


#endif

