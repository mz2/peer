/*
 *  vbfa.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 11/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "bayesnet.h"
#include "Eigen/Eigen"
#include "alglib/src/specialfunctions.h"
#include "vbfa.h"




//constructor from expression data
cVBFA::cVBFA(MatrixXf pheno_mean)
{
	MatrixXf pheno_var(pheno_mean.rows(),pheno_mean.cols());
	cVBFA::cVBFA(pheno_mean,pheno_var);
}
//constructor that take variance into account
cVBFA::cVBFA(MatrixXf pheno_mean,MatrixXf pheno_var)
{
	

}

