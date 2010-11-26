/*
 *  vbfa.h
 *  peer
 *
 *  Created by Oliver Stegle on 11/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __VBFA_H__
#define __VBFA_H__

#include "bayesnet.h"

/** Weight node (vector normal with shared precision) */
class cWNode : public cNode {
protected: MatrixXf E2;	
public:
	cWNode(); // default
	cWNode(MatrixXf E1); // from mean
	void update(cBayesNet *net);
	double calcBound(cBayesNet *net);
	double entropy();
};


/** Factor node (vector normal) */
class cXNode : public cNode {
protected: MatrixXf E2;	
public:
	cXNode(); // default
	cXNode(MatrixXf E1); // from mean
	void update(cBayesNet *net);
	double calcBound(cBayesNet *net);
	double entropy();
};


/** Weight precision node (vector Gamma) */
class cAlphaNode : public cGammaNode {
public:
	void update(cBayesNet *net);
};


/** Noise precision node (vector Gamma) */
class cEpsNode : public cGammaNode {
public:
	void update(cBayesNet *net);
};



/** Factor node (vector normal) */
class cPhenoNode {
protected: MatrixXf E1,E2;	
public:
	cPhenoNode(); // default
};




/** Main class for variational Bayesian factor analysis */
class cVBFA : public cBayesNet {

protected:
	/** Dimensions */
	int Nj; //individuals 
	int Np; //phenotypes
	int Nk; //factors
	
	/** Nodes */ 
	cWNode W;
	cXNode X;
	cEpsNode Eps;
	cAlphaNode Alpha;
    cPhenoNode pheno;
	
	
	
    	
public:
	/** Inference parameters */
	int Niterations; 
	int initialisation; // type

	//default constructor
	//cVBFA();
	
	//constructor from expression data
	cVBFA(MatrixXf pheno_mean);
	//constructor that take variance into account
	cVBFA(MatrixXf pheno_mean,MatrixXf pheno_var);
	

	void update();
	
	
};

#endif