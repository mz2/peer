/*
 *  model.cpp
 *  peer
 *
 *  Created by Leopold Parts on 03/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "model.h"
#include "bayesnet.h"
#include "alglib/src/ap.h"
#include "alglib/src/alglibmisc.h"

using alglib::hqrndnormal;
using Eigen::LUDecomposition;

double logdet(MatrixXf m){
	Eigen::LUDecomposition<MatrixXf> lu(m);	
	return log(lu.determinant());
}

cNodeS::cNodeS(cBayesNet *net, MatrixXf *E1_val) : cNode() {
	cNode::prior = Vector2f(0,1);
	N = net->N;
	int K = net->components;

	// init mean
	if (E1_val == NULL){ 
		E1 = MatrixXf(N, K);
		for (int i=0; i < E1.rows(); i++){
			for (int j=0; j<E1.cols(); j++){
			    E1(i,j) = prior(0) + hqrndnormal()*prior(1);
			}
		}
	}
	else { E1 = *E1_val; }

	// init variance
	cov = prior(1)*MatrixXf::Identity(net->components, net->components);
	E2S = MatrixXf::Zero(net->components, net->components);
	for (int i=0; i < net->N; i++){
		E2S += MatrixXf::Ones(net->components, net->components);
		E2S += E1.col(i)*E1.col(i).transpose();
    }
}



double cNodeS::entropy() {
    int K = E1.cols();
	double ent = 0.5*N*K*(1 + log(2.*pi()));
	MatrixXf ld = logdet(cov);
	ent += 0.5*N*ld.sum();

	return ent;
}


def calcBound(self, net):
N = self.E1.shape[0]
K = self.E1.shape[1]        
return -0.5*N*K*S.log(2.*S.pi) - 0.5*(N*self.cov.diagonal().sum() + (self.E1**2).sum()) + self.entropy()




double cNodeS::calcBound(cBayesNet *net) {} 
void cNodeS::update(cBayesNet *net) {}
	
	
/**
 class CNodeS(AVGaussNode): 
 
 __slots__ = ['E2S', '_N']
 
 
 
 def update(self, net):        
 t = time.time()
 
 W = net.W
 Eps = net.dataNode.getPrec()[0] # precisions are shared across individuals
 lastEps = net.W.lastEps
 
 # prediction needs to be recomputed if net initialised
 net.isPredictionValid = False 
 
 
 # calculate <WEW> based on the current state of the W node. This requires recalculating second moments
 # of W. For that, we need to use the old Alpha and Eps values to replicate the state of the W node.
 WEW = S.zeros([net.components,net.components])
 for d in range(net._D):
 # calculate second moment of W[d]
 WE2 = outer(W.E1[d], W.E1[d]) + linalg.inv(diag(net.Alpha.lastE1) + lastEps[d]*self.E2S)
 # update <WEW>
 WEW += WE2*Eps[d]
 
 # calculate covariance matrix of S
 p = S.eye(net.components) + WEW
 self.cov = linalg.inv(p)
 
 # calculate mean
 matrix   = S.dot(S.dot(self.cov, W.E1.T), S.diag(Eps))
 self.E1  = S.dot(net.dataNode.E1, matrix.T)
 
 # calculate sum of second moments
 self.E2S[:,:] = 0
 for n in range(self._N):
 self.E2S += self.cov + S.outer(self.E1[n,:], self.E1[n,:])
 
 # Note that the WSSW product precalculated in W node is invalid until
 # W is updated.
 
 L.log(LEVEL_NODE, '\tFA S node update, time=%.2f sec'%(time.time()-t))
 
*/ 
