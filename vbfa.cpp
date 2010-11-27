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

/** Default constructors */ 
cWNode::cWNode(){}
cWNode::cWNode(MatrixXf E1){
	this->E1 = E1;
	//TODO: 2nd moment
}
cXNode::cXNode(){}
cXNode::cXNode(MatrixXf E1){
	this->E1 = E1;
	//TODO: 2nd moment
}


double cWNode::calcBound(cBayesNet &net){
	return 0.;
}


double cWNode::entropy(){
	return 0;
}


void cWNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;

	//    // for each phenotype, calculate covariance and mean of weight
	for(int i = 0; i < n.Np; i++){
//		MatrixXf diagAE1 = MatrixXf::Zero(n.
//		cov = (diagAE1 + n.X.E2S*n.Eps.E1(i)).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
//		this->E1(d) = cov*(net->Eps.E1(i)&net->S.E1&net->pheno.E1(i)); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
//		this->E2S += (cov + this->E1(d)*this->E1(d)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
	}
}



double cXNode::calcBound(cBayesNet &net){
	return 0.;
}


double cXNode::entropy(){
	return 0;
}


void cXNode::update(cBayesNet &net){
}


void cAlphaNode::update(cBayesNet &net){
}


void cEpsNode::update(cBayesNet &net){
}


cPhenoNode::cPhenoNode(){}


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


// Global update
void cVBFA::update(){
	for(int i=0; i < this->Niterations; ++i){
//		this.W.update();
//		this.Alpha.update();
//		this.S.update();
//		this.Eps.update();
	}
}
