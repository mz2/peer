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

//let's just log to standard out by default
#define OSSOLOG_STDOUT


#include "bayesnet.h"
#include "array_helper.h"


//declare external varible for vorbosity
extern int VERBOSE;

namespace PEER 
{


	int getVerbose();
	void setVerbose(int verbose);
	
	
/** Helpers */
double logdet(PMatrix m);

/** Weight node (vector normal with shared precision) */
class cWNode : public cNode {
protected: 	
public:
	PMatrix E2S;	
	PMatrix Xprec;
	PMatrix A_last, E_last, XE2S_last;
	float lndetcovS;
	
	cWNode(); // default
	cWNode(PMatrix E1); // from mean
	virtual void update(cBayesNet* net);
	double calcBound(cBayesNet* net);
	double entropy();
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
};


/** Factor node (vector normal) */
class cXNode : public cNode {
protected: 
public:
	PMatrix E2S;
	PMatrix cov;
	PMatrix prior_offset;
	PMatrix prior_prec;
	
	cXNode(); // default
	cXNode(PMatrix E1); // from mean
	cXNode(PMatrix E1, PMatrix prior_offset, PMatrix prior_prec); // from mean and prior precision
	void update(cBayesNet* net);
	double calcBound(cBayesNet* net);
	double entropy();
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
	

};


/** Weight precision node (vector Gamma) */
class cAlphaNode : public cGammaNode {
public:
	cAlphaNode() {};
	cAlphaNode(int dim, float pa, float pb, PMatrix E1): cGammaNode(dim,pa,pb,E1) {};
	virtual void update(cBayesNet* net);
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
	

};


/** Noise precision node (vector Gamma) */
class cEpsNode : public cGammaNode {
public:
	cEpsNode() {};
	cEpsNode(int dim, float pa, float pb, PMatrix E1): cGammaNode(dim,pa,pb,E1) {};
	virtual void update(cBayesNet* net);
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);
};



/** Factor node (vector normal) */
class cPhenoNode {
public:
	PMatrix E1,E2;	

	cPhenoNode() {};
	cPhenoNode(PMatrix pheno_mean,PMatrix pheno_var); // default
	void getE1(float64_t** matrix,int32_t* rows,int32_t* cols);	
};



/** Main class for variational Bayesian factor analysis */
class cVBFA : public cBayesNet {
	//declare friends:
	friend class cNode;
	friend class cWNode;
	friend class cXNode;
	friend class cPhenoNode;
	friend class cEpsNode;
	friend class cAlphaNode;
//	friend class cWNodeSparse;
	
public:
	/** Dimensions */
	int Nj; //individuals 
	int Np; //phenotypes
	int Nk; //Total number of factors
	int Nc; //Number of covariates
	
	/** prior settings **/
	double Alpha_pa;
	double Alpha_pb;
	double Eps_pa;
	double Eps_pb;
	
	/* number of iterations*/
	int Nmax_iterations; 
	/* Convergence tolerances - bound and residual variance*/
	double tolerance;
	double var_tolerance;
	/* (very high) precision of known covariates */
	double covariate_prec;
	
	bool is_initialized;
	/* initialisation parameters*/
	bool add_mean;
	intialisation_types initialisation; // type
	
	/* input data */
	PMatrix pheno_mean;
	PMatrix pheno_var;
	PMatrix covs;
	
	//initialisation of default params
	
	virtual void init_params();
	PMatrix calc_residuals();
	
public:
	/** Nodes */ 
	cWNode* W;
	cXNode* X;
	cEpsNode* Eps;
	cAlphaNode* Alpha;
    cPhenoNode* pheno;
	
	/** Inference parameters */
	
	//default constructor
	//we need this mainly for the python/R interface as they don't support function overloading easily
	cVBFA();	
	//we could think about adding swig/R constructors also, but these will be less flexible.

#ifndef SWIG
	//constructor from expression data
	cVBFA(PMatrix pheno_mean,int Nfactors);
	//constructor that takes covariates into account
	cVBFA(PMatrix pheno_mean, PMatrix covs,int Nfactors);
	//constructor that take variance and covariates into account
	cVBFA(PMatrix pheno_mean, PMatrix pheno_var, PMatrix covs, int Nfactors);
#endif
	
	//destructor
	virtual ~cVBFA();
	
	//getters
	int getNj() {return Nj;};
	int getNp() {return Np;};
	int getNk() {return Nk;};
	int getNc() {return Nc;};
	int getNmax_iterations() {return Nmax_iterations;};
	double getTolerance() {return tolerance;};
	double getVarTolerance() {return var_tolerance;}
	bool getAdd_mean() { return add_mean;}				
	double getPriorEpsA() { return Eps_pa;}
	double getPriorEpsB() { return Eps_pb;}
	double getPriorAlphaA() { return Alpha_pa;}
	double getPriorAlphaB() { return Alpha_pb;}

	//setters 
	void setNk(int Nk) {this->Nk = Nk;is_initialized=false;}
	void setAdd_mean(bool add_mean) {this->add_mean = add_mean;is_initialized=false;};
	void setNmax_iterations(int Nmax_iterations) {this->Nmax_iterations = Nmax_iterations;};
	void setTolerance(double tolerance){this->tolerance = tolerance;};
	void setVarTolerance(double tolerance) {this->var_tolerance = tolerance;};
	void setPriorAlphaA(double pa) {Alpha_pa = pa; is_initialized=false;}
	void setPriorAlphaB(double pb) {Alpha_pb = pb; is_initialized=false;}
	void setPriorEpsA(double pa) {Eps_pa = pa; is_initialized=false;}
	void setPriorEpsB(double pb) {Eps_pb = pb; is_initialized=false;}
	void setPriorAlpha(double pa,double pb){ Alpha_pa=pa;Alpha_pb = pb;is_initialized=false;}
	void setPriorEps(double pa,double pb){Eps_pa = pa;Eps_pb=pb;is_initialized=false;}
	
	//general methods:
	virtual	void init_net();	
	virtual double calcBound();
	virtual double logprob();
	virtual void update();
	

	//Interface specific methods:
#ifdef SWIG
	//swig versions of setters, avoiding matrix objects
	//setters
	void setPhenoMean(float64_t* matrix,int32_t rows,int32_t cols)
	{this->pheno_mean = array2matrix(matrix,rows,cols);is_initialized=false;}

	void setPhenoVar(float64_t* matrix,int32_t rows,int32_t cols)
	{this->pheno_var = array2matrix(matrix,rows,cols);is_initialized=false;}
	void setCovariates(float64_t* matrix,int32_t rows,int32_t cols)
	{this->covs = array2matrix(matrix,rows,cols);is_initialized=false;}	

	//getters
	void getPhenoMean(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(pheno_mean,matrix,rows,cols);}
	void getPhenoVar(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(pheno_var,matrix,rows,cols);}
	void getCovariates(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(covs,matrix,rows,cols);}

	void getX(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(X.E1,matrix,rows,cols);}
	void getW(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(W.E1,matrix,rows,cols);}
	void getEps(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(Eps.E1,matrix,rows,cols);}
	void getAlpha(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(Alpha.E1,matrix,rows,cols);}
	void getResiduals(float64_t** matrix,int32_t* rows,int32_t* cols)
	{return matrix2array(calc_residuals(),matrix,rows,cols);}
#else
	//excludede fromo the swig interface as overloade functions do not get type mapped (and we don't port PMatrix anyway)
	void setPhenoMean(const PMatrix pheno_mean) {this->pheno_mean = pheno_mean;is_initialized=false;}
	void setPhenoVar(const PMatrix pheno_var) {this->pheno_var = pheno_var;is_initialized=false;}
	void setCovariates(const PMatrix covs) { this->covs = covs;is_initialized=false;}	
	
	//getters
	PMatrix getPhenoMean(){return this->pheno_mean;}
	PMatrix getPhenoVar() {return this->pheno_var;}
	PMatrix getCovariates() {return this->covs;}

	PMatrix getX(){return X->E1;}
	PMatrix getW(){return W->E1;}
	PMatrix getAlpha(){return Alpha->E1;}
	PMatrix getEps(){return Eps->E1;}
	PMatrix getResiduals() {return calc_residuals();}
#endif
};

	
}
#endif
