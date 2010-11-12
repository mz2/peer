/*
 *  vbfa.h
 *  peer
 *
 *  Created by Oliver Stegle on 11/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

class cVBFA : public cBayesNet {

protected:
	
    VectorXf a0;
	VectorXf a;
	VectorXf lnE;
	
public:
	//default constructor
	cVBFA();
	
	//constructor from expression data
	cVBFA(MatrixXf& pheno_mean);
	//constructor that take variance into account
	cVBFA(MatrixXf& pheno_mean,MatrixXf& pheno_var);
	
	void update(cBayesNet *net);

};