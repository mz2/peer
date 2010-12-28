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
#include "array_helper.h"

/** Helpers */
double logdet(MatrixXf m);

/** Weight node (vector normal with shared precision) */
class cWNode : public cNode {
protected: 	
public:
	MatrixXf E2S;	
	float lndetcovS;
	
	cWNode(); // default
	cWNode(MatrixXf E1); // from mean
	void update(cBayesNet &net);
	double calcBound(cBayesNet &net);
	double entropy();
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
	

};


/** Factor node (vector normal) */
class cXNode : public cNode {
protected: 
public:
	MatrixXf E2S;
	MatrixXf cov;
	MatrixXf prior_offset;
	MatrixXf prior_prec;
	
	cXNode(); // default
	cXNode(MatrixXf E1); // from mean
	cXNode(MatrixXf E1, MatrixXf prior_offset, MatrixXf prior_prec); // from mean and prior precision
	void update(cBayesNet &net);
	double calcBound(cBayesNet &net);
	double entropy();
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
	

};


/** Weight precision node (vector Gamma) */
class cAlphaNode : public cGammaNode {
public:
	cAlphaNode() {};
	cAlphaNode(int dim, float pa, float pb, MatrixXf* E1): cGammaNode(dim,pa,pb,E1) {};
	void update(cBayesNet &net);
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
	

};


/** Noise precision node (vector Gamma) */
class cEpsNode : public cGammaNode {
public:
	cEpsNode() {};
	cEpsNode(int dim, float pa, float pb, MatrixXf* E1): cGammaNode(dim,pa,pb,E1) {};
	void update(cBayesNet &net);
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
};



/** Factor node (vector normal) */
class cPhenoNode {
public:
	MatrixXf E1,E2;	

	cPhenoNode() {};
	cPhenoNode(MatrixXf pheno_mean,MatrixXf pheno_var); // default
};




/** Main class for variational Bayesian factor analysis */
class cVBFA : public cBayesNet {
	
protected:
	
public:
	/** Dimensions */
	int Nj; //individuals 
	int Np; //phenotypes
	int Nk; //Total number of factors
	int Nc; //Number of covariates
	
	/* input data */
	MatrixXf pheno_mean;
	MatrixXf pheno_var;
	MatrixXf covs;
		
	/** Nodes */ 
	cWNode W;
	cXNode X;
	cEpsNode Eps;
	cAlphaNode Alpha;
    cPhenoNode pheno;
	
	/** Inference parameters */
	int Niterations; 
	int initialisation; // type
	
	//default constructor
	//we need this mainly for the python/R interface as they don't support function overloading easily
	cVBFA();	

#ifndef SWIG
	//constructor from expression data
	cVBFA(MatrixXf pheno_mean,int Nfactors);
	//constructor that takes covariates into account
	cVBFA(MatrixXf pheno_mean, MatrixXf covs,int Nfactors);
	//constructor that take variance and covariates into account
	cVBFA(MatrixXf pheno_mean, MatrixXf pheno_var, MatrixXf covs, int Nfactors);
#endif
	
	//setters 
#ifndef SWIG
	//excludede fromo the swig interface as overloade functions do not get type mapped (and we don't port MatrixXf anyway)
	void setPhenoMean(MatrixXf pheno_mean);
	void setPhenoVar(MatrixXf pheno_var);
	void setCovariates(MatrixXf covs);
#endif
	
	void setPhenoMean(float64_t* matrix,int32_t rows,int32_t cols);
	void setPhenoVar(float64_t* matrix,int32_t rows,int32_t cols);
	void setCovariates(float64_t* matrix,int32_t rows,int32_t cols);
	void setK(int Nfactors);
	
	//getters
#ifndef SWIG
	MatrixXf getResiduals();
#endif
	void getResiduals(float64_t** matrix,int32_t* rows,int32_t* cols);
	
	void init_net();
	
	double calcBound();
	double logprob();
	void update();
};


#endif
