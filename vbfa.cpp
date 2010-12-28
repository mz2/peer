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


/** Helpers */
double logdet(MatrixXf m){
	SelfAdjointEigenSolver<MatrixXf> eigensolver(m);
	return eigensolver.eigenvalues().array().log().sum();
}





const double PI = 3.14159;


/** Default constructors */ 
cWNode::cWNode(){}
cWNode::cWNode(MatrixXf E1){
	this->E1 = E1;
	this->E2S = E1.transpose()*E1;
}
cXNode::cXNode(){}
cXNode::cXNode(MatrixXf E1){
	this->E1 = E1;
	this->E2S = E1.transpose()*E1;
	this->prior_prec = MatrixXf::Identity(E1.cols(), E1.cols());
	this->prior_offset = MatrixXf::Zero(E1.cols(), E1.rows());
}
cXNode::cXNode(MatrixXf E1, MatrixXf prior_offset, MatrixXf prior_prec){
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
	E2S = MatrixXf::Zero(n.Nk, n.Nk);

	// for each phenotype, calculate covariance and mean of weight
	MatrixXf diagAE1 = MatrixXf::Zero(n.Nk,n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;
	for(int i = 0; i < n.Np; i++){
		MatrixXf cov = (diagAE1 + n.X.E2S*n.Eps.E1(i,0)).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
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
	MatrixXf prec = MatrixXf::Identity(n.Nk, n.Nk);
	MatrixXf diagAE1 = MatrixXf::Zero(n.Nk, n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;

	for(int i = 0; i < n.Np; i++){ // as we don't keep W.E2 in memory, have to recalculate it to compute the sum WE2[i]*Eps[i]
		MatrixXf WE2 = (diagAE1 + n.X.E2S*n.Eps.E1(i,0)).inverse() + n.W.E1.row(i).transpose()*n.W.E1.row(i); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
		prec += n.Eps.E1(i,0)*WE2; //  only care about sum of 2nd moments
	}
	
	// Invert precision to get covariance, update moments
	MatrixXf diagEpsE1 = MatrixXf::Zero(n.Np, n.Np); // may need to switch this when Np gets large
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


void cAlphaNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	// cout << "Alpha Node update start, ncol=" << E1.cols() << endl; 
	b = pb + 0.5*n.W.E2S.diagonal().array();
	a = (pa + 0.5*n.Np)*(MatrixXf::Ones(n.Nk, 1).array());
	updateMoments();
	// cout << "Alpha Node update end, ncol=" << E1.cols() << endl; 
}


void cEpsNode::getE1(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	matrix2array(this->E1, matrix,rows,cols);
}

void cEpsNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	a = (pa + 0.5*n.Nj)*MatrixXf::Ones(n.Np,1).array();
	
	MatrixXf b1 = (MatrixXf::Ones(1, n.Nj)*n.pheno.E2).transpose();
	MatrixXf b2 = (MatrixXf::Ones(1, n.Nj)*((n.pheno.E1.array()*(n.X.E1*n.W.E1.transpose()).array()).matrix())).transpose();
	MatrixXf b3 = MatrixXf::Zero(n.Np,1);
	// for each phenotype, recalculate second moment of W
	MatrixXf diagAE1 = MatrixXf::Zero(n.Nk,n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;
	for(int i = 0; i < n.Np; i++){
		MatrixXf Wcov = (diagAE1 + n.X.E2S*n.Eps.E1(i,0)).inverse();
		b3(i,0) = (n.X.E2S.array()*(Wcov + n.W.E1.row(i).transpose()*n.W.E1.row(i)).array()).sum();
	}
	
	b = pb + 0.5*b1.array() - b2.array() + 0.5*b3.array();
	updateMoments();
}


cPhenoNode::cPhenoNode(MatrixXf pheno_mean,MatrixXf pheno_var)
{
	E1 = pheno_mean;
	E2 = pheno_var + E1.cwiseProduct(E1);
}


/**** CVBFA *****/


/*Constructors*/

cVBFA::cVBFA() { 
};

//constructor from expression data
cVBFA::cVBFA(MatrixXf pheno_mean,int Nfactors) { 
	this->pheno_mean = pheno_mean;
	this->Nk         = Nfactors;
}
//constructor that take covariates into account
cVBFA::cVBFA(MatrixXf pheno_mean, MatrixXf covs, int Nfactors) { 
	this->pheno_mean = pheno_mean;
	this->covs       = covs;
	this->Nk         = Nfactors;
}
//constructor that take variance and covariates into account
cVBFA::cVBFA(MatrixXf pheno_mean,MatrixXf pheno_var, MatrixXf covs, int Nfactors) 
{ 
	this->pheno_mean = pheno_mean;
	this->pheno_var  = pheno_var;
	this->covs       = covs;
	this->Nk         = Nfactors;
}

/*
cVBFA::cVBFA(MatrixXf *pheno_mean,int Nfactors) { init_net(pheno_mean, (MatrixXf *)NULL, (MatrixXf *)NULL, Nfactors); }
//constructor that take covariates into account
cVBFA::cVBFA(MatrixXf *pheno_mean, MatrixXf *covs, int Nfactors) { init_net(pheno_mean, (MatrixXf *)NULL, covs, Nfactors); }
//constructor that take variance and covariates into account
cVBFA::cVBFA(MatrixXf *pheno_mean,MatrixXf *pheno_var, MatrixXf *covs, int Nfactors) { init_net(pheno_mean, pheno_var, covs, Nfactors); }
*/


/* setters */



void cVBFA::setPhenoMean(float64_t* matrix,int32_t rows,int32_t cols)
{
	this->setPhenoMean(array2matrix(matrix,rows,cols));
};

void cVBFA::setPhenoVar(float64_t* matrix,int32_t rows,int32_t cols)
{
	this->setPhenoVar(array2matrix(matrix,rows,cols));
};

void cVBFA::setCovariates(float64_t* matrix,int32_t rows,int32_t cols)
{
	this->setCovariates(array2matrix(matrix,rows,cols));
};


void cVBFA::setPhenoMean(MatrixXf m)
{
	this->pheno_mean = m;
}

void cVBFA::setPhenoVar(MatrixXf pheno_var)
{
	this->pheno_var = pheno_var;
}
void cVBFA::setCovariates(MatrixXf covs)
{
	this->covs = covs;
}



void cVBFA::setK(int Nfactors)
{
	this->Nk = Nfactors;
};


/* getters */ 


void cVBFA::init_net()
{
	//0. extract dimensions, fill unspecified parameters 
	Nj = pheno_mean.rows();
	Np = pheno_mean.cols();
	Niterations = 10;
	
	if (isnull(pheno_var))
	{	
		MatrixXf temp = 0.01*(MatrixXf::Ones(pheno_mean.rows(), pheno_mean.cols())); // if uncertainty in expression not provided, assume pretty certain
	    pheno_var = temp;
		printf("XX %d/%d -- %d/%d",pheno_mean.rows(),pheno_mean.cols(),pheno_var.rows(),pheno_var.cols());	
	}
	if (isnull(covs)) 
	{ 
		MatrixXf temp = MatrixXf::Ones(pheno_mean.rows(), 1); 
		covs = temp;
	} // if no covariates, insert one for mean
	
	//Oli: is this truely still needed ? Leo, please test this quickly and remove if not, I think the main reason was this pointer madness
	MatrixXf covs_original; // need to keep a copy - transformations change the matrix :S
	covs_original = covs;
	
	
	Nc = covs.cols();
	Nk = Nk + Nc;
	//how many "non covariate factors"?
	int Nfactors = Nk-Nc;
	
	//debug output:
	//printf("Nk:%d,Nj: %d,Np: %d", Nk,Nj,Np);
	//cout << pheno_mean;

	//1. checkups of parameters passed
	assert (pheno_mean.rows()==pheno_var.rows());
	assert (pheno_mean.cols()==pheno_var.cols());
	assert (covs.rows() == pheno_mean.rows());
	//2. create nodes
	pheno = cPhenoNode(pheno_mean,pheno_var);

	//3. find ML estimates for known factors and PCA for rest
	MatrixXf X0 = MatrixXf::Zero(Nj,Nk);
	MatrixXf W0 = MatrixXf::Zero(Np,Nk);
	MatrixXf cov_weights = covs.colPivHouseholderQr().solve(pheno.E1);
	MatrixXf residuals = pheno.E1 - covs_original*cov_weights;
	W0.block(0,0,Np, Nc) = cov_weights.transpose();
	X0.block(0,0,Nj,Nc) = covs_original;
	//JacobiSVD test;
	JacobiSVD<MatrixXf> svd(residuals, ComputeThinU | ComputeThinV);
	//create a diagonal matrix
	MatrixXf Sdiag = svd.singularValues().asDiagonal();
	MatrixXf U = svd.matrixU();
	MatrixXf V = svd.matrixV();
	//dot product Sdiag V
	MatrixXf SV = Sdiag*V.transpose();
	//get the factors up to the Kth component
	X0.block(0,Nc,Nj,Nfactors) = U.block(0,0,U.rows(),Nfactors);
	W0.block(0,Nc,Np,Nfactors) = SV.block(0,0,Nfactors,SV.cols()).transpose();
	
	// 4. create nodes and initialise
	MatrixXf Xprec_prior = MatrixXf::Identity(Nk,Nk);
	Xprec_prior.diagonal().block(0,0,1,Nc) = MatrixXf::Ones(1,Nc)*10000.; // covariates are very precise
	MatrixXf Xmean_prior = MatrixXf::Zero(Nj, Nk);
	Xmean_prior.block(0,0,Nj,Nc) = covs_original;
	W = cWNode(W0);
	X = cXNode(X0, Xmean_prior,Xprec_prior);
	Alpha = cAlphaNode((int)Nk, (float)0.001, (float)0.1, (MatrixXf *)NULL);
	Eps = cEpsNode(Np, 0.1, 10, (MatrixXf *)NULL);
	// update precision nodes to initialise them
	Alpha.update(*this);
	Eps.update(*this);	
	cout << "\tAfter initi, residual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
}


// Global update
void cVBFA::update(){
	for(int i=0; i < this->Niterations; ++i){
		cout << "\titeration" << i << endl;
		W.update(*this);
		if(i > 0) {cout << "\tAfter W " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;}
		Alpha.update(*this);
		if(i > 0) {cout << "\tAfter A " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;}
		X.update(*this);
		cout << "\tAfter X " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
		Eps.update(*this);
		cout << "\tAfter E " << calcBound() << "\tResidual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
	}
//	cout << "Update of cVBFA done, " << this->Niterations << " iterations elapsed" << endl;
}


MatrixXf cVBFA::getResiduals()
{
	return (pheno.E1 - X.E1*W.E1.transpose());
}

void cVBFA::getResiduals(float64_t** matrix,int32_t* rows,int32_t* cols)
{
	MatrixXf r = this->getResiduals();
	matrix2array(r, matrix,rows,cols);
}

double cVBFA::logprob(){
	MatrixXf diagAE1 = MatrixXf::Zero(Nk,Nk);
	diagAE1.diagonal() = Alpha.E1;

	float result = 0.5*(Nj*(Eps.lnE.sum() - Np*log(2.*PI)));
	for(int i=0; i < Np; ++i){
		result -= 0.5*Eps.E1(i,0)*(pheno.E1.col(i).transpose()*pheno.E1.col(i))(0,0);
		result += Eps.E1(i,0)*(pheno.E1.col(i).transpose()*(X.E1*W.E1.row(i).transpose()))(0,0);
		MatrixXf WE2 = (diagAE1 + X.E2S*Eps.E1(i,0)).inverse() + W.E1.row(i).transpose()*W.E1.row(i);
		result -= 0.5*Eps.E1(i,0)*(X.E2S.array()*WE2.array()).sum();
	}
	return result;
}


double cVBFA::calcBound(){
	return logprob() + W.calcBound(*this) + Alpha.calcBound(*this) + X.calcBound(*this) + Eps.calcBound(*this);
}
