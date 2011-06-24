/*
 *  sparsefa.h
 *  peer
 *  Sparse Factor analysis variant of VBFA
 *
 *  Created by Oliver Stegle on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SPARSEFA_H__
#define __SPARSEFA_H__


#include "vbfa.h"
#include "bayesnet.h"


namespace PEER
{
	
	
	/*Sparse Factor analysis node for W*/
	class cWNodeSparse : public cWNode {
	protected:
		//log prior of link being on
		PMatrix pi;
		PMatrix lpi;
		//posteior probability of link being on
		PMatrix C;
		//posterior probability of link being off (for convenience)
		PMatrix Coff;
		double tauOn;
		double tauOff;
	public:
		//public stuff is inherited from cWnode...
		cWNodeSparse(); // 
		cWNodeSparse(PMatrix E1,PMatrix pi,cBayesNet &net);
		virtual void update(cBayesNet &net);
		

		
	};
	
	
	class cSPARSEFA : public cVBFA { 
		//declare friends:
		friend class cNode;
		friend class cWNode;
		friend class cXNode;
		friend class cWNodeSparse;
		friend class cPhenoNode;
		friend class cEpsNode;
		friend class cAlphaNode;
		
	protected:
		//test: matrix with pror information about relations?
		PMatrix pi;
		double sigmaOff;
		double sigmaOn;
		virtual void init_params();

		
	public:
		
		//constructor
		cSPARSEFA();
		
#ifndef SWIG
		//constructors that take matrices in 
		//constructor from expression data
		cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior,int Nfactors);
		//constructor that takes covariates into account
		cSPARSEFA(PMatrix pheno_mean,PMatrix sparsity_prior, PMatrix covs,int Nfactors);
		//constructor that take variance and covariates into account
		cSPARSEFA(PMatrix pheno_mean, PMatrix pheno_var,PMatrix sparsity_prior, PMatrix covs, int Nfactors);

		
#endif
		//setters
		void setSigmaOff(double sigma_off) {this->sigmaOff = sigma_off;};
		//getters
		double getSigmaOff() { return this->sigmaOff;};
		
#ifdef SWIG
		//interface-specific methods
		void setSparsityPrior(float64_t* matrix,int32_t rows,int32_t cols)
		{this->pi = array2matrix(matrix,rows,cols);is_initialized=false;}
		void getSparsityPrior(float64_t** matrix,int32_t* rows,int32_t* cols)
		{return matrix2array(pi,matrix,rows,cols);}
#else
		void setSparsityPrior(const PMatrix sparsity_prior) {this->pi = sparsity_prior;is_initialized=false;}
		PMatrix getSparsityPrior(){return this->pi;}

#endif
		
		 virtual double calcBound();
		 virtual double logprob();
		 virtual void init_net();	
		 virtual void update();
		
		
	};// :: sparseFA
	
	
	
}


#endif