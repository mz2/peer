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
#include "sparsefa.h"
#include "bayesnet.h"


using namespace Eigen;
using namespace std;
using namespace PEER;



//sparsity posterior:
void cSPARSEFA::getZ(float64_t** matrix,int32_t* rows,int32_t* cols)
{
  if(!is_initialized)
    {
      (*rows) = 0;
      (*cols) = 0;
      return;
    }
  cWNodeSparse* W_ = (cWNodeSparse*) (W);
  return matrix2array(W_->C,matrix,rows,cols);
}

void cSPARSEFA::setSparsityPrior(const PMatrix sparsity_prior) {this->pi = sparsity_prior;is_initialized=false;}

PMatrix cSPARSEFA::getSparsityPrior(){return this->pi;}

PMatrix cSPARSEFA::getZ()
{
  if(!is_initialized)
    return PMatrix();
  cWNodeSparse* W_ = (cWNodeSparse*)W;
  return W_->C;
}

/* sparse W node class */
cWNodeSparse::cWNodeSparse()
{
}


cWNodeSparse::cWNodeSparse(PMatrix E1,PMatrix pi,cBayesNet* net)
{
	updateC = true;
	
	cSPARSEFA* n = (cSPARSEFA*)net;

	//0. parameters for precisions values we need
	tauOff = pow(n->sigmaOff,-2);
	tauOn = pow(n->sigmaOn,-2);
	
	//1. moment from W and prior on link pi
	this->E1 = E1;
	this->pi = pi;
	//logarithmated version
	this->lpi = log(pi);
	PMatrix pi_ = PMatrix::Ones(pi.rows(),pi.cols())-pi;
	this->lpi_off = log(pi_);
	//2. derive initialization of C:
	this->C  = PMatrix(pi);
	this->Coff = PMatrix::Ones(C.rows(),C.cols())-C;
	//diagonal prior on covariance
	CovPriorDiag = PMatrix::Zero(n->Np,n->Nk);
	
	//3. initialisation of second moments based on prior
	
	E2S = PMatrix::Zero(n->Nk, n->Nk);
	Xprec = PMatrix::Zero(n->Nk,n->Nk);	
	for (int i=0;i<pi.rows();i++)
	{
		//1. contribution from sparistiy prior
		PMatrix pprior = PMatrix::Zero(n->Nk,n->Nk);
		//pprior = diag(1.0*self.C[d,:,1] + self.C[d,:,0]*(tauOff))
		pprior.diagonal() = (this->C.row(i)*tauOn + this->Coff.row(i)*tauOff);	
		PMatrix cov = (pprior).inverse();
		PMatrix E2 = (cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
		E2S += E2;
		Xprec += n->Eps->E1(i,0)*E2;
	}//end for 

}



void cWNodeSparse::update(cBayesNet *net)
{
	cSPARSEFA* n = (cSPARSEFA*)net;
	lndetcovS = 0.;
	E2S = PMatrix::Zero(n->Nk, n->Nk);
	//TODO: don't we need this?
	Xprec = PMatrix::Zero(n->Nk,n->Nk);
	
	// for each phenotype, calculate covariance and mean of weight
	for(int i = 0; i < n->Np; i++){
		//procesion matrix, combining info from prior, noise level and incoming S
		//1. contribution from sparistiy prior
		PMatrix pprior = PMatrix::Zero(n->Nk,n->Nk);
		//pprior = diag(1.0*self.C[d,:,1] + self.C[d,:,0]*(tauOff))
		//store temporary for updateEps:
		CovPriorDiag.row(i) = (this->C.row(i)*tauOn + this->Coff.row(i)*tauOff);
		//std::cout << CovPriorDiag.row(i) << "\n";
		//std::cout << E1.row(i) << "\n";
		
		pprior.diagonal() = CovPriorDiag.row(i);
		//std::cout << CovPriorDiag.row(i) << "\n\n";
		//2. incoming message from X and data:
		PMatrix pin = n->X->E2S*n->Eps->E1(i,0);
		//3. add and invert
		PMatrix cov = (pin+pprior).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
		//std::cout << cov << "\n\n";
		lndetcovS += logdet(cov);
		E1.row(i) = n->Eps->E1(i,0)*cov*n->X->E1.transpose()*n->pheno->E1.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		PMatrix E2 = (cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
		
		//std::cout << E1.row(i) << "\n\n";
		//std::cout << E2 << "\n\n";
		E2S += E2;
		Xprec += n->Eps->E1(i,0)*E2;
		
		if (updateC)
		{
			//4. update sparsity prior
			for (int j=0;j<n->Nk;j++)
			{
				double l0 = lpi_off(i,j);
				double l1 = lpi(i,j);
				l0 += 0.5*log(tauOff/(2*M_PI)) - 0.5*tauOff*E2(j,j);
				l1 += 0.5*log(tauOn/(2*M_PI))  - 0.5*tauOn*E2(j,j);
				double coff = exp(l0) + 1E-6;
				double con  = exp(l1) + 1E-6;
				double Z  = coff+con;
				C(i,j) = con/Z;
				Coff(i,j) = 1-C(i,j);
			}
		} //end updateC
		
	}
	
	// store values needed to calculate covariance for each p
	E_last = n->Eps->E1;
	XE2S_last = n->X->E2S;
}



void cEpsNodeSparse::update(cBayesNet* net)
{
	cSPARSEFA* n = (cSPARSEFA*)net;
	a = (pa + 0.5*n->Nj)*PMatrix::Ones(n->Np,1).array();
	cWNodeSparse* Ws = (cWNodeSparse*) n->W;
	
	PMatrix b1 = (PMatrix::Ones(1, n->Nj)*n->pheno->E2).transpose();
	PMatrix b2 = (PMatrix::Ones(1, n->Nj)*((n->pheno->E1.array()*(n->X->E1*n->W->E1.transpose()).array()).matrix())).transpose();
	PMatrix b3 = PMatrix::Zero(n->Np,1);
	PMatrix CovPrior = PMatrix::Zero(n->Nk,n->Nk);
	for(int i = 0; i < n->Np; i++){
		//get current covariance for phenotype p form W node
		CovPrior.diagonal() = Ws->CovPriorDiag.row(i);
		PMatrix Wcov = (CovPrior + n->W->XE2S_last*n->Eps->E1(i,0)).inverse(); // calculate current covariance matrix of W (cannot use updated values for X,A)
		b3(i,0) = (n->X->E2S.array()*(Wcov + n->W->E1.row(i).transpose()*n->W->E1.row(i)).array()).sum();
	}
	b = pb + 0.5*b1.array() - b2.array() + 0.5*b3.array();
	updateMoments();	
}

//default constructor
cSPARSEFA::cSPARSEFA() { 
	//default settings
	Nk = -1;
	init_params();
};


//constructor from expression data
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior)
: cVBFA(pheno_mean, -1)
{
	init_params();
	this->pi = sparsity_prior;
}
//constructor that takes covariates into account
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior, PMatrix covs)
: cVBFA(pheno_mean, covs, -1)
{
	init_params();
	this->pi = sparsity_prior;
}
//constructor that take variance and covariates into account
cSPARSEFA::cSPARSEFA(PMatrix pheno_mean, PMatrix pheno_var,PMatrix sparsity_prior, PMatrix covs)
: cVBFA(pheno_mean, pheno_var, covs, -1)
{
	init_params();
	this->pi = sparsity_prior;
}


// destructor
/* overwrite as we don't have an Alpha Nde in this net*/
cSPARSEFA::~cSPARSEFA()
{
}

void cSPARSEFA::init_params() { 
	//default settings
	//ovewrite initialization
	initialisation = PRIOR;
	sigmaOff = 1E-2;
	sigmaOn = 1.0;
	add_mean = false;
	Niterations = 0;
};


void cSPARSEFA::init_net()
{
	//1. decide whether to run sparse or non-sparse mode
	if (isnull(pi))
	{
		//hard code initialization type
		initialisation= PCA;
		cVBFA::init_net();
	}
	else 
	{
		//hard code initialization type
		initialisation= PRIOR;

		init_net_sparse();
	}
}


void cSPARSEFA::init_net_sparse()
{
	tolerance = 0;
	
	//call enhirted method first	
	//0. extract dimensions, fill unspecified parameters 
	Nj = pheno_mean.rows();
	Np = pheno_mean.cols();
	
	if (isnull(pheno_var))
	{	
		pheno_var = 0.01*(PMatrix::Ones(pheno_mean.rows(), pheno_mean.cols())); // if uncertainty in expression not provided, assume pretty certain
		printf("XX %d/%d -- %d/%d\n",(int) pheno_mean.rows(),(int) pheno_mean.cols(),(int) pheno_var.rows(),(int) pheno_var.cols());	
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
		covs.conservativeResize(Nj,nc+1);
		covs.block(0,nc,Nj,1) = PMatrix::Ones(Nj, 1); 
	}
	
		
	Nc = covs.cols();
	//total number of factors is determined from dimensionality of Pi
	Nk = pi.cols() + Nc;
	
	//how many "non covariate factors"?
	int Nhiddens = pi.cols();
	
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
	assert (Nj>0);
	assert (Np>0);
	assert (Nk>0);
	//check that we don't have more factors than individuals or genes
			
	//2. create nodes
	pheno = new cPhenoNode(pheno_mean,pheno_var);
	
	//stating initialization of X0/W0
	PMatrix X0 = randn(Nj,Nk);
	PMatrix W0 = randn(Np,Nk);
		
	//3. find ML estimates for known factors and PCA for rest
	PMatrix residuals;
	
	if (Nc>0)
	//initialize W and X parts that are representing known factors
	{
		PMatrix cov_weights = covs.colPivHouseholderQr().solve(pheno->E1);
		residuals = pheno->E1 - covs*cov_weights;
		W0.block(0,0,Np, Nc) = cov_weights.transpose();
		X0.block(0,0,Nj,Nc) = covs;
	}
	else {
		//data to use for initialization
		residuals = pheno->E1;
	}
	
	//create complete prior matrix inlcuding known covariates etc.
	PMatrix piX = PMatrix::Ones(Np,Nk);
	piX.block(0,Nc,Np,Nhiddens) = this->pi;
	//update Pi
	this->pi = piX;
		
	
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
		X0.block(0,Nc,Nj,Nhiddens) = U.block(0,0,U.rows(),Nhiddens);
		W0.block(0,Nc,Np,Nhiddens) = SV.block(0,0,Nhiddens,SV.cols()).transpose();
	}
		
	// Create prior on X that keeps known factors intact during learning
	PMatrix Xprec_prior = PMatrix::Identity(Nk,Nk);
	Xprec_prior.diagonal().block(0,0,Nc,1) = PMatrix::Ones(Nc,1)*covariate_prec;
	PMatrix Xmean_prior = PMatrix::Zero(Nj, Nk);
	Xmean_prior.block(0,0,Nj,Nc) = covs;

	//need Eps for W init
	Eps = new cEpsNodeSparse(Np, Eps_pa,Eps_pb, PMatrix());
	W = new cWNodeSparse(W0,pi,this);
	X = new cXNode(X0, Xmean_prior,Xprec_prior);
	//set Alpha Node to NUll to ensure that delete does not crash
	Alpha = NULL;
	//updates to complete init
	
	if (VERBOSE>=2)
	{
		cout << "\tAfter initi, residual variance " << (pheno->E1 - X->E1*W->E1.transpose()).array().pow(2.).mean() << endl;
	}
	is_initialized = true;
	
} //::init_net_sparse



double cSPARSEFA::calcBound()
{
	//1. decide whether to run sparse or non-sparse mode
	if (isnull(pi))
		return cVBFA::calcBound();
	else {
		return 0;
	}

}

double cSPARSEFA::logprob()
{
	if (isnull(pi))
		return cVBFA::logprob();
	else {
		return 0;
	}

}


// Global model update
void cSPARSEFA::update()
{
	// reserve memory for bound and residual history
	Tresidual_varaince = PVector(Nmax_iterations);
	Tbound = PVector(Nmax_iterations);
		
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
				
		//W
		if (VERBOSE>=4)
			std::cout << W->E1 << "\n\n";
		W->update(this);		
		if (VERBOSE>=4)
			std::cout << W->E1 << "\n\n";
		if((VERBOSE>=3) && (i > 0) )
		{cout << "\tAfter W " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;}
		
		//Alha?
		if (Alpha!=NULL)
		{
			Alpha->update(this);
			if((VERBOSE>=3) && (i > 0) )
			{cout << "\tAfter A " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;}
		}
		
		//X
		if (VERBOSE>=4)
			std::cout << X->E1 << "\n\n";
		X->update(this);
		if (VERBOSE>=4)
			std::cout << X->E1 << "\n\n";
		if (VERBOSE>=3)
			cout << "\tAfter X " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;
		
		//EPS
		if (VERBOSE>=4)
			std::cout << Eps->E1 << "\n\n";
		Eps->update(this);
		if (VERBOSE>=4)
			std::cout << Eps->E1 << "\n\n";
		if (VERBOSE>=3)
			cout << "\tAfter E " << calcBound() << "\tResidual variance " << calc_residuals().array().pow(2.).mean() << endl;
		
		
		//calc bound?
		if ((VERBOSE>=2) || (tolerance>0))
		{
			//deactivate bound calculation as not working at the moment
			last_bound    = current_bound;
			last_residual_var = current_residual_var;
			current_bound = calcBound();
			current_residual_var = calc_residuals().array().pow(2).mean();
			delta_bound = (current_bound - last_bound); // bound should increase
			delta_residual_var = last_residual_var - current_residual_var; // variance should decrease
			Tbound[i] = current_bound;
		}
		
		double res_var = getResiduals().array().array().pow(2.).mean();
		Tresidual_varaince[i] = res_var;
		//debug output?			
		if (VERBOSE>=2)
		{
			ULOG_INFO("Residual variance: %.4f, Delta bound: %.4f, Delta var(residuals): %.4f\n",res_var,delta_bound, delta_residual_var);
		}
			
		//converged?
		if (abs(delta_bound)<tolerance)
			break;
		if (abs(delta_residual_var)<var_tolerance)
			break;
		Niterations+=1;
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
	
} // ::update


//getters and setters
void cSPARSEFA::setSparsityPrior(float64_t* matrix,int32_t rows,int32_t cols)
{this->pi = array2matrix(matrix,rows,cols);is_initialized=false;}
void cSPARSEFA::getSparsityPrior(float64_t** matrix,int32_t* rows,int32_t* cols)
{return matrix2array(pi,matrix,rows,cols);}


