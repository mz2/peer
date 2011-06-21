/*
 *  sparsefa.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "sparsefa.h"
#include "ossolog.h"
#include "sparsefa.h"


using namespace Eigen;
using namespace std;
using namespace PEER;


/* sparse W node class */
cWNodeSparse::cWNodeSparse()
{
}


cWNodeSparse::cWNodeSparse(PMatrix pi)
{
	this->pi = pi;
}

cWNodeSparse::cWNodeSparse(PMatrix E1,PMatrix CE1,PMatrix pi)
{
	this->E1 = E1;
	this->E2S = E1.transpose()*E1;

	this->pi = pi;
	this->C  = CE1;
	
	
}


void cWNodeSparse::update(cBayesNet &net)
{
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
		PMatrix E2 = (cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
		E2S += E2;
		Xprec += n.Eps.E1(i,0)*E2;
	}
	
	// store values needed to calculate covariance for each p
	E_last = n.Eps.E1;
	XE2S_last = n.X.E2S;
	A_last = n.Alpha.E1;	
}




cSPARSEFA::cSPARSEFA() { 
	//default settings
	init_params();
	//ovewrite initialization
	initialisation = RANDN;
};


//constructor from expression data
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior,int Nfactors)
: cVBFA(pheno_mean, Nfactors)
{
	this->pi = sparsity_prior;
}
//constructor that takes covariates into account
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior, PMatrix covs,int Nfactors)
: cVBFA(pheno_mean, covs, Nfactors)
{
	this->pi = sparsity_prior;
}
//constructor that take variance and covariates into account
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean, PMatrix pheno_var,PMatrix sparsity_prior, PMatrix covs, int Nfactors)
: cVBFA(pheno_mean, pheno_var, covs,Nfactors)
{
	this->pi = sparsity_prior;
}



void cSPARSEFA::init_params()
{
	//call enhirted method first
	//cVBFA::init_params();
	
	
	//0. extract dimensions, fill unspecified parameters 
	Nj = pheno_mean.rows();
	Np = pheno_mean.cols();
	
	if (isnull(pheno_var))
	{	
		pheno_var = 0.01*(PMatrix::Ones(pheno_mean.rows(), pheno_mean.cols())); // if uncertainty in expression not provided, assume pretty certain
		printf("XX %d/%d -- %d/%d\n",pheno_mean.rows(),pheno_mean.cols(),pheno_var.rows(),pheno_var.cols());	
	}
	// if no covariates, create empty structure
	if (isnull(covs)) 
	{ 
		//create an emptyp matrix of the correct shape:
		covs = PMatrix::Ones(Nj,0);
	} 
	
	// add mean column in covariates?
	if (add_mean)
	{
		//create a larger matrix and concatenate mean column:
		int nc = covs.cols();
		covs.resize(Nj,nc+1);
		covs.block(0,nc,Nj,nc+1) = PMatrix::Ones(Nj, 1); 
	}
	
	
	Nc = covs.cols();
	Nk = Nk + Nc;
	//how many "non covariate factors"?
	int Nfactors = Nk-Nc;
	
	//debug output:
	if(VERBOSE>=2)
	{
		ULOG_INFO("Initialising Net");
		ULOG_INFO("Initialising Net\n");
		ULOG_INFO("Data dimensions: Nk:%d,Nj: %d,Np: %d, Nc: %d\n", Nk,Nj,Np,Nc);
	}
	//cout << pheno_mean;
	//1. checkups of parameters passed
	assert (pheno_mean.rows()==pheno_var.rows());
	assert (pheno_mean.cols()==pheno_var.cols());
	assert (covs.rows() == pheno_mean.rows());
	assert (Nj>0);
	assert (Np>0);
	assert (Nk>0);
	//check that we don't have more factors than individuals or genes
	assert (Nk<Np);
	assert (Nk<Nj);
	//check that we don't have covaraites (not supported)
	if (Nc>0)
		ULOG_ERR("Covaraites not supported yet");
	assert (Nc==0);
			
	//2. create nodes
	pheno = cPhenoNode(pheno_mean,pheno_var);
	
	//3. find ML estimates for known factors and PCA for rest
	PMatrix X0 = PMatrix::Zero(Nj,Nk);
	PMatrix W0 = PMatrix::Zero(Np,Nk);
	PMatrix residuals;
	
	if (Nc>0)
	{
		PMatrix cov_weights = covs.colPivHouseholderQr().solve(pheno.E1);
		residuals = pheno.E1 - covs*cov_weights;
		W0.block(0,0,Np, Nc) = cov_weights.transpose();
		X0.block(0,0,Nj,Nc) = covs;
	}
	else {
		residuals = pheno.E1;
	}
	
	//4.initialize the model
	
	
	
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
	Xprec_prior.diagonal().block(0,0,Nc,1) = PMatrix::Ones(Nc,1)*covariate_prec;
	PMatrix Xmean_prior = PMatrix::Zero(Nj, Nk);
	Xmean_prior.block(0,0,Nj,Nc) = covs;
	W = cWNodeSparse(W0);
	X = cXNode(X0, Xmean_prior,Xprec_prior);
	
	Alpha = cAlphaNode((int)Nk,Alpha_pa,Alpha_pb, PMatrix());
	Eps = cEpsNode(Np, Eps_pa,Eps_pb, PMatrix());
	// update precision nodes to initialise them
	Alpha.update(*this);
	//Eps.update(*this);	
	if (VERBOSE>=2)
	{
		cout << "\tAfter initi, residual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
	}
		
	is_initialized = true;
	
	
		
}


void cSPARSEFA::init_net()
{
	
	cVBFA::init_net();
}


void cSPARSEFA::update()
{
	cVBFA::update();
}