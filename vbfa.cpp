/*
 *  vbfa.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 11/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <assert.h>
#include "bayesnet.h"
#include "Eigen/Eigen"
#include <Eigen/Dense>
#include "alglib/src/specialfunctions.h"
#include "vbfa.h"

using namespace Eigen;
using namespace std;
using namespace PEER;


/** Helpers */
double PEER::logdet(PMatrix m){
	SelfAdjointEigenSolver<PMatrix> eigensolver(m);
	return eigensolver.eigenvalues().array().log().sum();
}


//verbosity level:
int VERBOSE = 1;


void PEER::setVerbose(int verbose)
{ VERBOSE = verbose;}

int PEER::getVerbose()
{return VERBOSE;}


const double PI = 3.14159;


/** Default constructors */ 
cWNode::cWNode(){}
cWNode::cWNode(PMatrix E1){
	this->E1 = E1;
	this->E2S = E1.transpose()*E1;
}
cXNode::cXNode(){}
cXNode::cXNode(PMatrix E1){
	this->E1 = E1;
	this->E2S = E1.transpose()*E1;
	this->prior_prec = PMatrix::Identity(E1.cols(), E1.cols());
	this->prior_offset = PMatrix::Zero(E1.cols(), E1.rows());
}
cXNode::cXNode(PMatrix E1, PMatrix prior_offset, PMatrix prior_prec){
	this->E1 = E1;
	this->E2S = E1.transpose()*E1;
	this->prior_prec = prior_prec;
	this->prior_offset = prior_offset;
}

void cXNode::getE1(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	matrix2array(this->E1, matrix,rows,cols);
}


void cWNode::getE1(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	matrix2array(this->E1, matrix,rows,cols);
}



void cWNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	lndetcovS = 0.;
	E2S = PMatrix::Zero(n.Nk, n.Nk);

	// for each phenotype, calculate covariance and mean of weight
	PMatrix diagAE1 = PMatrix::Zero(n.Nk,n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;
	for(int i = 0; i < n.Np; i++){
		PMatrix cov = (diagAE1 + n.X.E2S*n.Eps.E1(i,0)).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
		lndetcovS += logdet(cov);
		E1.row(i) = n.Eps.E1(i,0)*cov*n.X.E1.transpose()*n.pheno.E1.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		E2S += (cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
	}
}


double cWNode::calcBound(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	int D = E1.rows();
	int K = E1.cols();
//	cout << "WD = " << D << " K = " << K << endl;
	return -0.5*D*(K*log(2.*PI) - n.Alpha.lnE.sum()) - 0.5*(n.Alpha.E1.array()*E2S.diagonal().array()).sum() + entropy();
}


double cWNode::entropy(){
	int K = E1.cols();
	int D = E1.rows();
//	cout << "WD = " << D << " K = " << K << endl;
	return 0.5*D*K*log(2.*PI) + 0.5*lndetcovS;
}


void cXNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	// big work - calculate precision matrix
	PMatrix prec = PMatrix::Identity(n.Nk, n.Nk);
	PMatrix diagAE1 = PMatrix::Zero(n.Nk, n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;

	for(int i = 0; i < n.Np; i++){ // as we don't keep W.E2 in memory, have to recalculate it to compute the sum WE2[i]*Eps[i]
		PMatrix WE2 = (diagAE1 + n.X.E2S*n.Eps.E1(i,0)).inverse() + n.W.E1.row(i).transpose()*n.W.E1.row(i); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
		prec += n.Eps.E1(i,0)*WE2; //  only care about sum of 2nd moments
	}
	
	// Invert precision to get covariance, update moments
	PMatrix diagEpsE1 = PMatrix::Zero(n.Np, n.Np); // may need to switch this when Np gets large
	diagEpsE1.diagonal() = n.Eps.E1;
	cov = (prec + prior_prec).inverse(); 
	E1 = (n.pheno.E1*diagEpsE1*n.W.E1 + prior_offset*prior_prec)*cov;
	E2S = n.Nj*cov;
	for (int i=0;i<n.Nj; ++i){
		E2S += E1.row(i).transpose()*E1.row(i);
	} // the covariance matrix is shared by design between individuals
}


double cXNode::calcBound(cBayesNet &net){
	int K = E1.cols();
	int N = E1.rows();
//	cout << "WN = " << N << " K = " << K << endl;
	return -0.5*N*K*log(2.*PI) - 0.5*(N*cov.diagonal().sum() + ((E1.array().pow(2).matrix())*prior_prec).array().sum()) + entropy();
}


double cXNode::entropy(){
    int K = E1.cols();
	int N = E1.rows();
//	cout << "WN = " << N << " K = " << K << endl;
	return 0.5*N*(K*log(2.*PI) + K + logdet(cov));
}


void cAlphaNode::getE1(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	matrix2array(this->E1, matrix,rows,cols);
}

void cPhenoNode::getE1(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	matrix2array(this->E1, matrix,rows,cols);
}

void cAlphaNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	// cout << "Alpha Node update start, ncol=" << E1.cols() << endl; 
	b = pb + 0.5*n.W.E2S.diagonal().array();
	a = (pa + 0.5*n.Np)*(PMatrix::Ones(n.Nk, 1).array());
	updateMoments();
	// cout << "Alpha Node update end, ncol=" << E1.cols() << endl; 
}


void cEpsNode::getE1(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	matrix2array(this->E1, matrix,rows,cols);
}

void cEpsNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	a = (pa + 0.5*n.Nj)*PMatrix::Ones(n.Np,1).array();
	
	PMatrix b1 = (PMatrix::Ones(1, n.Nj)*n.pheno.E2).transpose();
	PMatrix b2 = (PMatrix::Ones(1, n.Nj)*((n.pheno.E1.array()*(n.X.E1*n.W.E1.transpose()).array()).matrix())).transpose();
	PMatrix b3 = PMatrix::Zero(n.Np,1);
	// for each phenotype, recalculate second moment of W
	PMatrix diagAE1 = PMatrix::Zero(n.Nk,n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;
	for(int i = 0; i < n.Np; i++){
		PMatrix Wcov = (diagAE1 + n.X.E2S*n.Eps.E1(i,0)).inverse();
		b3(i,0) = (n.X.E2S.array()*(Wcov + n.W.E1.row(i).transpose()*n.W.E1.row(i)).array()).sum();
	}
	
	b = pb + 0.5*b1.array() - b2.array() + 0.5*b3.array();
	updateMoments();
}


cPhenoNode::cPhenoNode(PMatrix pheno_mean,PMatrix pheno_var)
{
	E1 = pheno_mean;
	E2 = pheno_var + E1.cwiseProduct(E1);
}


/**** CVBFA *****/


/*Constructors*/

cVBFA::cVBFA() { 
	//default settings
	init_params();
};

//constructor from expression data
cVBFA::cVBFA(PMatrix pheno_mean,int Nfactors) { 
	init_params();
	this->pheno_mean = pheno_mean;
	this->Nk         = Nfactors;
}
//constructor that take covariates into account
cVBFA::cVBFA(PMatrix pheno_mean, PMatrix covs, int Nfactors) { 
	init_params();
	this->pheno_mean = pheno_mean;
	this->covs       = covs;
	this->Nk         = Nfactors;
}
//constructor that take variance and covariates into account
cVBFA::cVBFA(PMatrix pheno_mean,PMatrix pheno_var, PMatrix covs, int Nfactors) 
{ 
	init_params();
	this->pheno_mean = pheno_mean;
	this->pheno_var  = pheno_var;
	this->covs       = covs;
	this->Nk         = Nfactors;
}

// initialiser shared by constructors
void cVBFA::init_params()
{
	tolerance = 1E-3;
	Nmax_iterations = 1000;
	add_mean    = true;
	initialisation = PCA;	
	is_initialized = false;
	//prior settings
	Alpha_pa =0.001;
	Alpha_pb = 0.1;
	Eps_pa  = 0.1;
	Eps_pb  = 10;
}

/* setters */

/* getters */ 


void cVBFA::init_net()
{
	//0. extract dimensions, fill unspecified parameters 
	Nj = pheno_mean.rows();
	Np = pheno_mean.cols();
	
	if (isnull(pheno_var))
	{	
		PMatrix temp = 0.01*(PMatrix::Ones(pheno_mean.rows(), pheno_mean.cols())); // if uncertainty in expression not provided, assume pretty certain
	    pheno_var = temp;
		printf("XX %d/%d -- %d/%d\n",pheno_mean.rows(),pheno_mean.cols(),pheno_var.rows(),pheno_var.cols());	
	}
	// if no covariates, create empty structure
	if (isnull(covs)) 
	{ 
		//create an emptyp matrix of the correct shape:
		//PMatrix temp = PMatrix::Ones(pheno_mean.rows(), 1); 
		PMatrix temp = PMatrix::Ones(Nj,0);
		covs = temp;
	} 
	
	// add mean column in covariates?
	if (add_mean)
	{
		PMatrix temp = PMatrix::Ones(Nj, 1); 
		//create a larger matrix and concatenate mean column:
		int nc = covs.cols();
		covs.resize(Nj,nc+1);
		covs.block(0,nc,Nj,nc+1) = temp;
	}
	
	//Oli: is this truely still needed ? Leo, please test this quickly and remove if not, I think the main reason was this pointer madness
	PMatrix covs_original; // need to keep a copy - transformations change the matrix :S
	//note for me: in eigen "=" means create a copy of the matrix
	covs_original = covs;
	
	
	Nc = covs.cols();
	Nk = Nk + Nc;
	//how many "non covariate factors"?
	int Nfactors = Nk-Nc;
	
	//debug output:
	if(VERBOSE>=2)
	{
		printf("Initialising Net\n");
		printf("Data dimensions: Nk:%d,Nj: %d,Np: %d, Nc: %d\n", Nk,Nj,Np,Nc);
	}
	//cout << pheno_mean;
	//1. checkups of parameters passed
	assert (pheno_mean.rows()==pheno_var.rows());
	assert (pheno_mean.cols()==pheno_var.cols());
	assert (covs.rows() == pheno_mean.rows());
	assert (Nj>0);
	assert (Np>0);
	assert (Nk>0);
	//2. create nodes
	pheno = cPhenoNode(pheno_mean,pheno_var);

	//3. find ML estimates for known factors and PCA for rest
	PMatrix X0 = PMatrix::Zero(Nj,Nk);
	PMatrix W0 = PMatrix::Zero(Np,Nk);
	PMatrix residuals;
	
	if (Nc>0)
	{
		PMatrix cov_weights = covs.colPivHouseholderQr().solve(pheno.E1);
		residuals = pheno.E1 - covs_original*cov_weights;
		W0.block(0,0,Np, Nc) = cov_weights.transpose();
		X0.block(0,0,Nj,Nc) = covs_original;
	}
	else {
		residuals = pheno.E1;
	}

	
	//JacobiSVD test;
	JacobiSVD<PMatrix> svd(residuals, ComputeThinU | ComputeThinV);
	//create a diagonal matrix
	PMatrix Sdiag = svd.singularValues().asDiagonal();
	PMatrix U = svd.matrixU();
	PMatrix V = svd.matrixV();
	//dot product Sdiag V
	PMatrix SV = Sdiag*V.transpose();
	//get the factors up to the Kth component
	X0.block(0,Nc,Nj,Nfactors) = U.block(0,0,U.rows(),Nfactors);
	W0.block(0,Nc,Np,Nfactors) = SV.block(0,0,Nfactors,SV.cols()).transpose();
	
	// 4. create nodes and initialise
	PMatrix Xprec_prior = PMatrix::Identity(Nk,Nk);
	Xprec_prior.diagonal().block(0,0,1,Nc) = PMatrix::Ones(1,Nc)*10000.; // covariates are very precise
	PMatrix Xmean_prior = PMatrix::Zero(Nj, Nk);
	Xmean_prior.block(0,0,Nj,Nc) = covs_original;
	W = cWNode(W0);
	X = cXNode(X0, Xmean_prior,Xprec_prior);
	
	Alpha = cAlphaNode((int)Nk,Alpha_pa,Alpha_pb, PMatrix());
	Eps = cEpsNode(Np, Eps_pa,Eps_pb, PMatrix());
	// update precision nodes to initialise them
	Alpha.update(*this);
	Eps.update(*this);	
	if (VERBOSE>=2)
		cout << "\tAfter initi, residual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
	
	is_initialized = true;
}


// Global update
void cVBFA::update(){
	
	//auto init net if needed
	if(!is_initialized)
		init_net();
	
	double last_bound=-HUGE_VAL;
	double current_bound=-HUGE_VAL;
	double delta_bound = HUGE_VAL;
	int i=0;
	for(i=0; i < this->Nmax_iterations; ++i){
		if (VERBOSE>=1)
			printf("\titeration %d/%d\n",i,Nmax_iterations);
		
		W.update(*this);
		
		if((VERBOSE>=3) && (i > 0) )
			{cout << "\tAfter W " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;}
		Alpha.update(*this);
		if((VERBOSE>=3) && (i > 0) )
			{cout << "\tAfter A " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;}
		X.update(*this);
		if (VERBOSE>=3)
			cout << "\tAfter X " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
		Eps.update(*this);
		
		if (VERBOSE>=3)
			cout << "\tAfter E " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;

		//calc bound?
		if ((VERBOSE>=2) || (tolerance>0))
		{
			last_bound    = current_bound;
			current_bound = calcBound();
			delta_bound = abs(current_bound - last_bound);
		}
		
		//debug output?
		
		if (VERBOSE>=2)
		{
			double res_var = getResiduals().array().array().pow(2.).mean();
			printf("Residual variance: %.4f, Delta bound: %.4f\n",res_var,delta_bound);
		}
		
		//converged?
		if (delta_bound<tolerance)
			break;
	//endfor
	}
	
	//debug output on convergence?
	if (VERBOSE>=1)
	{
		if(delta_bound<tolerance)
		{
			printf("Converged after %d iterations\n", i);
		}
		else {
			printf("Maximum number of iterations reached: %d\n",i);
		}
	}
}


PMatrix cVBFA::calc_residuals()
{
	return (pheno.E1 - X.E1*W.E1.transpose());
}


double cVBFA::logprob(){
	PMatrix diagAE1 = PMatrix::Zero(Nk,Nk);
	diagAE1.diagonal() = Alpha.E1;

	float result = 0.5*(Nj*(Eps.lnE.sum() - Np*log(2.*PI)));
	for(int i=0; i < Np; ++i){
		result -= 0.5*Eps.E1(i,0)*(pheno.E1.col(i).transpose()*pheno.E1.col(i))(0,0);
		result += Eps.E1(i,0)*(pheno.E1.col(i).transpose()*(X.E1*W.E1.row(i).transpose()))(0,0);
		PMatrix WE2 = (diagAE1 + X.E2S*Eps.E1(i,0)).inverse() + W.E1.row(i).transpose()*W.E1.row(i);
		result -= 0.5*Eps.E1(i,0)*(X.E2S.array()*WE2.array()).sum();
	}
	return result;
}


double cVBFA::calcBound(){
	return logprob() + W.calcBound(*this) + Alpha.calcBound(*this) + X.calcBound(*this) + Eps.calcBound(*this);
}
