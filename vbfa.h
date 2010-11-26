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


class cVBFA : public cBayesNet {

protected:
	//TODO
	/*
	 
	pheno = CGaussian()
	 */
	int Nj; //individuals 
	int Np; //phenotypes
	int Nk; //factors
	
	void updateW();
	void updateX();
	void updateAlpha();
	
    	
public:
	//default constructor
	//cVBFA();
	
	//constructor from expression data
	cVBFA(MatrixXf pheno_mean);
	//constructor that take variance into account
	cVBFA(MatrixXf pheno_mean,MatrixXf pheno_var);
	

	void update();
	
	
};

#endif