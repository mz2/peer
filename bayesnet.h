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

#include "Eigen/Eigen"
#include <iostream>

using Eigen::MatrixXf;
using Eigen::VectorXf;
using namespace std;


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
	MatrixXf E1;
	MatrixXf prior;
	double bound;

	virtual double entropy();	
	virtual double calcBound(cBayesNet &net);
	virtual void update(cBayesNet &net);
};


/**
 * Dirichlet Node - simple test node to implement (not used in PEER).
 */
class cDirichletNode : public cNode {
protected:
    VectorXf a0;
	VectorXf a;
	VectorXf lnE;
	
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
	MatrixXf lnE;
	MatrixXf a;
	MatrixXf b;

	cGammaNode();
	cGammaNode(int dim, float prior_val_a, float prior_val_b, MatrixXf *E1_val);
	double entropy();	
	double calcBound(cBayesNet &net);
	void update(cBayesNet &net);
	void updateMoments();
};


#endif

