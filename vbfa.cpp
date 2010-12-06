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
}


double cWNode::calcBound(cBayesNet &net){
	return 0.;
}


double cWNode::entropy(){
	return 0;
}


void cWNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;

	// for each phenotype, calculate covariance and mean of weight
	for(int i = 0; i < n.Np; i++){
		MatrixXf diagAE1 = MatrixXf::Zero(n.Np,1);
		diagAE1.diagonal() = n.Alpha.E1;
		MatrixXf cov = (diagAE1 + n.X.E2S*n.Eps.E1(i)).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
		E1.row(i) = n.Eps.E1(i)*cov*n.X.E1.transpose()*n.pheno.E1.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		E2S += (cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
	}
}



double cXNode::calcBound(cBayesNet &net){
	return 0.;
}


double cXNode::entropy(){
	return 0;
}


void cXNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	
	// big work - calculate precision matrix
	MatrixXf prec = MatrixXf::Identity(n.Nk, n.Nk);
	for(int i = 0; i < n.Np; i++){ // as we don't keep W.E2 in memory, have to recalculate it to compute the sum WE2[i]*Eps[i]
		MatrixXf diagAE1 = MatrixXf::Zero(n.Np,1);
		diagAE1.diagonal() = n.Alpha.E1;
		MatrixXf cov = (diagAE1 + n.X.E2S*n.Eps.E1(i)).inverse(); // linalg.inv(diag(Alpha.E1) + Eps[d]*M)
		E1.row(i) = n.Eps.E1(i)*cov*n.X.E1.transpose()*n.pheno.E1.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		prec += n.Eps.E1(i)*(cov + E1.row(i).transpose()*E1.row(i)); //  E2 = dcov + outer(self.E1[d], self.E1[d])
	}
	
	// Invert precision to get covariance, update moments
	MatrixXf cov = prec.inverse(); 
	E1 = n.pheno.E1*cov*n.W.E1.transpose()*n.Eps.E1;
	E2S = cov + E1.transpose()*E1;
}



void cAlphaNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	cout << "Alpha Node update start, ncol=" << E1.cols() << endl; 
	b = pb + 0.5*n.W.E2S.diagonal().array();
	a = (pa + 0.5*n.Np)*(MatrixXf::Ones(n.Nk, 1).array());
	updateMoments();
	cout << "Alpha Node update end, ncol=" << E1.cols() << endl; 
}


void cEpsNode::update(cBayesNet &net){
	cVBFA n = (cVBFA&)net;
	a = (pa + 0.5*n.Nj)*MatrixXf::Ones(n.Np,1).array();
	
	MatrixXf b1 = (MatrixXf::Ones(1, n.Nj)*n.pheno.E2).transpose();
	MatrixXf b2 = (MatrixXf::Ones(1, n.Nj)*(n.pheno.E1.cwiseProduct(n.X.E1*n.W.E1.transpose()))).transpose();
	MatrixXf b3 = MatrixXf::Zero(n.Np,1);
	// for each phenotype, recalculate second moment of W
	MatrixXf diagAE1 = MatrixXf::Zero(n.Nk,n.Nk);
	diagAE1.diagonal() = n.Alpha.E1;
	for(int i = 0; i < n.Np; i++){
		MatrixXf cov = (diagAE1 + n.X.E2S*n.Eps.E1(i)).inverse();
		b3(i,0) = (n.X.E2S*(cov + n.W.E1.row(i).transpose()*n.W.E1.row(i))).sum();
	}
	
	b = pb + 0.5*(b1.array() - 2.*b2.array() + b3.array());
	updateMoments();
}


cPhenoNode::cPhenoNode(MatrixXf pheno_mean,MatrixXf pheno_var)
{
	E1 = pheno_mean;
	E2 = pheno_var;
}


//constructor from expression data
cVBFA::cVBFA(MatrixXf pheno_mean,int Nfactors)
{
	MatrixXf pheno_var(pheno_mean.rows(),pheno_mean.cols());
	// cannot 'chain' constructors in C++ so cannot call another constructor here..
}

//constructor that take variance into account
cVBFA::cVBFA(MatrixXf pheno_mean,MatrixXf pheno_var,int Nfactors)
{
	//1. checkups of parameters passed
	assert (pheno_mean.rows()==pheno_var.rows());
	assert (pheno_mean.cols()==pheno_var.cols());
	Nj = pheno_mean.rows();
	Np = pheno_mean.cols();
	Nk = Nfactors;
	Niterations = 1;
	//2. create nodes
	pheno = cPhenoNode(pheno_mean,pheno_var);
	//3. initialize with PCA
	//JacobiSVD test;
	JacobiSVD<MatrixXf> svd(pheno.E1,ComputeThinU | ComputeThinV);
	//cout << "Its singular values are:" << endl << svd.singularValues() << endl;
	//cout << "Its left singular vectors are the columns of the thin U matrix:" << endl << svd.matrixU() << endl;
	//cout << "Its right singular vectors are the columns of the thin V matrix:" << endl << svd.matrixV() << endl;
	//create a diagonal matrix
	MatrixXf Sdiag = svd.singularValues().asDiagonal();
	MatrixXf U = svd.matrixU();
	MatrixXf V = svd.matrixV();
	//dot product Sdiag V
	MatrixXf SV = Sdiag*V.transpose();
	//get the factors up to the Kth component
	MatrixXf X0 = U.block(0,0,U.rows(),Nk);
	MatrixXf W0 = SV.block(0,0,Nk,SV.cols()).transpose();
	
	// create nodes and initialise to the PCA solution
	W = cWNode(W0);
	X = cXNode(X0);
	Alpha = cAlphaNode((int)Nk, (float)0.1, (float)10, (MatrixXf *)NULL);
	cout << Alpha.E1.cols() << endl;
	Eps = cEpsNode(Np, 0.1, 10, (MatrixXf *)NULL);
    cout << Alpha.E1.cols() << endl;
	// update precision nodes to initialise them
	Alpha.update(*this);
	Eps.update(*this);
}




// Global update
void cVBFA::update(){
	for(int i=0; i < this->Niterations; ++i){
		W.update(*this);
		Alpha.update(*this);
		X.update(*this);
		Eps.update(*this);
	}
	cout << "Update of cVBFA done, " << this->Niterations << " iterations elapsed" << endl;
}
