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
		//private prior on regulatory link being on
		PMatrix pi;
		//log prior of link being on
		PMatrix lpi;
		//posteior probability of link being on
		PMatrix C;
	public:
		//public stuff is inherited from cWnode...
		cWNodeSparse(); // 
		cWNodeSparse(PMatrix pi); // 
		cWNodeSparse(PMatrix E1,PMatrix CE1,PMatrix pi); //E1 init on W1 and on indicator Z
		
		void update(cBayesNet &net);
		

		
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
		
		void init_params();
		
	public:
		//updated, sparse node for W
		cWNodeSparse W;
		
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
		
		void init_net();	
		void update();
		
		
	};// :: sparseFA
	
	
	
}


#endif