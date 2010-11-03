/*
 *  bayesnet.h
 *  peer
 *
 *  Created by Leopold Parts on 03/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "Eigen/Eigen"

using Eigen::MatrixXf;
using Eigen::VectorXf;


class cBayesNet {
};


/**
 * Abstract base class for all Bayes Net nodes
 */
class cNode {
protected:
	MatrixXf E1;
	MatrixXf prior;
	double bound;
public:
	virtual double entropy();	
	virtual double calcBound(cBayesNet net);
	virtual void update(cBayesNet net);
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
	double calcBound(cBayesNet net);
	void update(cBayesNet net);
};

