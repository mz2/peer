/*
 *  sparsefa.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


/* TODOs
 
 setter and getter for sigma off
 
 */


#include "sparsefa.h"
#include "ossolog.h"
#include "sparsefa.h"
#include "bayesnet.h"


using namespace Eigen;
using namespace std;
using namespace PEER;


/* sparse W node class */
cWNodeSparse::cWNodeSparse()
{
}


cWNodeSparse::cWNodeSparse(PMatrix E1,PMatrix pi,cBayesNet& net)
{
	cSPARSEFA n = (cSPARSEFA&)net;

	//0. parameters for precisions values we need
	tauOff = pow(n.sigmaOff,-2);
	tauOn = pow(n.sigmaOn,-2);
	
	//1. moment from W and prior on link pi
	this->E1 = E1;
	this->pi = pi;
	//logarithmated version
	this->lpi = log(pi);
	//2. derive initialization of C:
	this->C  = PMatrix(pi);
	this->Coff = PMatrix::Ones(C.rows(),C.cols())-C;
	//3. initialisation of second moments based on prior
	
	E2S = PMatrix::Zero(n.Nk, n.Nk);
	Xprec = PMatrix::Zero(n.Nk,n.Nk);
	for (int i=0;i<pi.rows();i++)
	{
		//1. contribution from sparistiy prior
		PMatrix pprior = PMatrix::Zero(n.Nk,n.Nk);
		//pprior = diag(1.0*self.C[d,:,1] + self.C[d,:,0]*(tauOff))
		pprior.diagonal() = (this->C.row(i)*tauOn + this->Coff.row(i)*tauOff);	
		PMatrix cov = (pprior).inverse();
		PMatrix E2 = (cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
		E2S += E2;
		Xprec += n.Eps.E1(i,0)*E2;
	}
}


void cWNodeSparse::update(cBayesNet &net)
{
	cSPARSEFA n = (cSPARSEFA&)net;
	lndetcovS = 0.;
	E2S = PMatrix::Zero(n.Nk, n.Nk);
	//TODO: don't we need this?
	Xprec = PMatrix::Zero(n.Nk,n.Nk);
	
	// for each phenotype, calculate covariance and mean of weight
	for(int i = 0; i < n.Np; i++){
		//procesion matrix, combining info from prior, noise level and incoming S
		//1. contribution from sparistiy prior
		PMatrix pprior = PMatrix::Zero(n.Nk,n.Nk);
		//pprior = diag(1.0*self.C[d,:,1] + self.C[d,:,0]*(tauOff))
		pprior.diagonal() = (this->C.row(i)*tauOn + this->Coff.row(i)*tauOff);
		//2. incoming message from X and data:
		PMatrix pin = n.X.E2S*n.Eps.E1(i,0);
		//3. add and invert
		PMatrix cov = (pin+pprior).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
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







//constructor from expression data
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior,int Nfactors)
: cVBFA(pheno_mean, Nfactors)
{
	init_params();
	this->pi = sparsity_prior;
}
//constructor that takes covariates into account
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior, PMatrix covs,int Nfactors)
: cVBFA(pheno_mean, covs, Nfactors)
{
	init_params();
	this->pi = sparsity_prior;
}
//constructor that take variance and covariates into account
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean, PMatrix pheno_var,PMatrix sparsity_prior, PMatrix covs, int Nfactors)
: cVBFA(pheno_mean, pheno_var, covs,Nfactors)
{
	init_params();
	this->pi = sparsity_prior;
}


void cSPARSEFA::init_params() { 
	//default settings
	//ovewrite initialization
	initialisation = PRIOR;
	sigmaOff = 1E-4;
	sigmaOn = 1.0;
	add_mean = false;
};

void cSPARSEFA::init_net()
{
	//call enhirted method first	
	
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
	assert (pi.rows()==Np);
	assert (pi.cols()==Nk);
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
	
	//stating initialization of X0/W0
	PMatrix X0 = randn(Nj,Nk);
	PMatrix W0 = randn(Np,Nk);
		
	//3. find ML estimates for known factors and PCA for rest
	PMatrix residuals;
	
	if (Nc>0)
	//initialize W and X parts that are representing known factors
	{
		PMatrix cov_weights = covs.colPivHouseholderQr().solve(pheno.E1);
		residuals = pheno.E1 - covs*cov_weights;
		W0.block(0,0,Np, Nc) = cov_weights.transpose();
		X0.block(0,0,Nj,Nc) = covs;
	}
	else {
		//data to use for initialization
		residuals = pheno.E1;
	}
	
	//create complete binary prior matrix inlcuding known effects
	PMatrix pi = PMatrix(Np,Nk);
	pi.block(0,Nc,Np,Nfactors) = this->pi;
		
	//4.initialize the model
	/*continue here: initialize model by sampling from prior!*/
	if (initialisation==PRIOR)
	{
		for (int i=0;i<Np;i++)
			for(int j=Nc;j<Nk;j++)
			{
				double rv = ((double)rand())/RAND_MAX;
				if (rv<pi(i,j))
					W0(i,j)*=this->sigmaOn;
				else
					W0(i,j)*=this->sigmaOff;
					
			}
	}
	else if (initialisation==PCA)
	{	
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
	}
	
	// more
	PMatrix Xprec_prior = PMatrix::Identity(Nk,Nk);
	Xprec_prior.diagonal().block(0,0,Nc,1) = PMatrix::Ones(Nc,1)*covariate_prec;
	PMatrix Xmean_prior = PMatrix::Zero(Nj, Nk);
	Xmean_prior.block(0,0,Nj,Nc) = covs;

	//need Eps for W init
	Eps = cEpsNode(Np, Eps_pa,Eps_pb, PMatrix());
	W = cWNodeSparse(W0,pi,*this);
	X = cXNode(X0, Xmean_prior,Xprec_prior);
	// update precision nodes to initialise them
	//Alpha.update(*this);
	if (VERBOSE>=2)
	{
		cout << "\tAfter initi, residual variance " << (pheno.E1 - X.E1*W.E1.transpose()).array().pow(2.).mean() << endl;
	}
	is_initialized = true;
	
}



double cSPARSEFA::calcBound()
{
	return 0;
}

double cSPARSEFA::logprob()
{
	return 0;
}



void cSPARSEFA::update()
{
	// Global update
		
	//auto init net if needed
	if(!is_initialized)
		init_net();
		
	double last_bound=-HUGE_VAL;
	double last_residual_var = HUGE_VAL;
	double current_bound=-HUGE_VAL;
	double current_residual_var = -HUGE_VAL;
	double delta_bound = HUGE_VAL;
	double delta_residual_var = HUGE_VAL;
	int i=0;
	for(i=0; i < this->Nmax_iterations; ++i)
	{
		if (VERBOSE>=1)
			printf("\titeration %d/%d\n",i,Nmax_iterations);
			
		((cWNodeSparse) this->W).update(*this);		
		if((VERBOSE>=3) && (i > 0) )
		{cout << "\tAfter W " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;}
		X.update(*this);
		if (VERBOSE>=3)
			cout << "\tAfter X " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;
		Eps.update(*this);		
		if (VERBOSE>=3)
			cout << "\tAfter E " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;
			
		//calc bound?
		if ((VERBOSE>=2) || (tolerance>0))
		{
			last_bound    = current_bound;
			last_residual_var = current_residual_var;
			current_bound = calcBound();
			current_residual_var = calc_residuals().array().pow(2).mean();
			delta_bound = (current_bound - last_bound); // bound should increase
			delta_residual_var = last_residual_var - current_residual_var; // variance should decrease
		}
			
		//debug output?
			
		if (VERBOSE>=2)
		{
			double res_var = getResiduals().array().array().pow(2.).mean();
			ULOG_INFO("Residual variance: %.4f, Delta bound: %.4f, Delta var(residuals): %.4f\n",res_var,delta_bound, delta_residual_var);
		}
			
		//converged?
		if (abs(delta_bound)<tolerance)
			break;
		if (abs(delta_residual_var)<var_tolerance)
			break;
	//endfor
	}
		
	//debug output on convergence?
	if (VERBOSE>=1)
	{
		if(abs(delta_bound)<tolerance)
		{
			ULOG_INFO("Converged (bound) after %d iterations\n", i);
		}
		else if(abs(delta_residual_var) < var_tolerance){
			ULOG_INFO("Converged (var(residuals)) after %d iterations\n", i);
		}
		else {
			ULOG_INFO("Maximum number of iterations reached: %d\n",i);
		}
	}
	
}