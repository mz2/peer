/*
 *  bayesnet.cpp
 *  peer
 *
 *  Created by Leopold Parts on 03/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "bayesnet.h"
#include <Eigen/Eigen>
#include "specialfunctions.h"
#include "ossolog.h"

using namespace Eigen;
using alglib::lngamma;
using alglib::psi;

double cNode::entropy() {return 0;}
double cNode::calcBound(cBayesNet &net) {return 0;}
void cNode::update(cBayesNet &net) {}

PMatrix cNode::getE1(){ return E1;}


cDirichletNode::cDirichletNode(int dim, float prior_val){
	prior = prior_val*PVector::Ones(dim);	
	a0 = PVector::Zero(dim);
	a = PVector::Zero(dim);
	E1 = PVector::Zero(dim);
	lnE = PVector::Zero(dim);
}


double cDirichletNode::entropy(){
	double temp = 0;
	double ent = -lngamma(a.sum(), temp);
	for (int i=0; i < a.rows(); i++) {ent += lngamma(a(i), temp);}
	ent += (a.sum() - a.rows())*psi(a.sum());
//	ent -= (a - PVector::Ones(a.rows()))*psi();
	for (int i=0; i < a.rows(); i++){ent -= (a(i) - 1)*psi(a(i));}
	return ent;
}


double cDirichletNode::calcBound(cBayesNet &net){
	double temp = 0;	
	double bound = ((a0 - PVector::Ones(a0.rows()))*lnE).sum() - lngamma(a0.sum(), temp);
	for (int i=0; i < a0.rows(); i++) { bound += lngamma(a0(i), temp); }
	return bound + entropy();
}

				
/** Not used, so not implemented */
void cDirichletNode::update(cBayesNet &net){}





/** Gamma node implementations */
cGammaNode::cGammaNode(){}

cGammaNode::cGammaNode(int dim, float prior_val_a, float prior_val_b, PMatrix E1_val){
	pa = prior_val_a;
	pb = prior_val_b;
	a = pa*PMatrix::Ones(dim,1).array();
	b = pb*PMatrix::Ones(dim,1).array();
	E1 = PMatrix::Zero(dim,1);
	lnE = PMatrix::Zero(dim,1);
	//cout << "Gamma Node init, ncol = " << E1.cols() << endl;
	updateMoments();
	if (!isnull(E1_val)){ 
		E1 = E1_val;
	}
	//cout << "Gamma Node init end, ncol = " << E1.cols() << endl;
}



double cGammaNode::entropy(){
	double temp = 0;
	double ent = 0;
	
	for (int i=0; i < a.rows(); i++) {
	    ent += a(i);
		ent -= log(b(i));
		ent += lngamma(a(i), temp);
		ent += (1. - a(i))*psi(a(i));
	}
	return ent;
}



double cGammaNode::calcBound(cBayesNet &net){

	double bound = 0;
	double temp = 0;	
	for (int i=0; i < lnE.rows(); i++) { 
		bound += pa*log(pb);
		bound += (pa - 1.)*lnE(i);
		bound -= pb*E1(i);
		bound -= lngamma(pa, temp);
	}
	return bound + entropy();
}


void cGammaNode::updateMoments(){
	for (int i=0; i < lnE.rows(); i++){
		this->E1(i) = a(i)/b(i);
		this->lnE(i) = psi(a(i)) - log(b(i));
	}
}

/** Standard update equation */
void cGammaNode::update(cBayesNet &net){
	this->updateMoments();
}



/*

class AVGaussNode(ANode):
''' Vector-Gauss-Node which sits in two plates
dim is supposed to be (N,d) where d is the vectorized gauss-node dimension and N is the data-dimension'''


__slots__ = ['E2','cov'];


# calculate 2nd moment from mean and covariance
# requires: self.cov is not None
def update(self, net=None):
L.debug('AVGaussNode update')
self.E2[:,:,:] = self.cov
for n in range(self.E2.shape[0]):
self.E2[n,:,:] = self.E2[n,:,:] + S.outer(self.E1[n,:],self.E1[n,:])


def entropy(self):
return 0.5*self.E2.shape[0]*(self.E2.shape[1]*(1 + S.log(2*S.pi))) + 0.5*sum(map(logdet, self.cov))


#    def calcBound(self, net):
#        return 0



class AGaussNode(ANode):
''' Completely separated Gauss node
We usually use this representation to model the dataset itself '''

__slots__ = ['E2','cov']


def __init__(self,node=None,dim=[1,1],prior=[0,1],E1=None,E2=None,prec=None,cov=None):
ANode.__init__(self)
#copy constructor
if node is not None:
self.E1 = node.E1.copy()
self.E2 = node.E2.copy()
self.cov= node.cov.copy()
return
self._prior=prior
if E1 is None:  E1 = prior[0]*random.standard_normal(size=dim)
AGaussNode.setMoments(self, E1=E1, E2=E2, prec=prec, cov=cov)


#    def update(self,E1,E2=None,cov=None,prec=None):
def setMoments(self,E1,E2=None,cov=None,prec=None):
"""update(E2,E2,cov,prec)
-updaes the state of the Gaussnode from E1 and a representation of the second moment"""
self.E1 = E1
#make sure this works also without any second moment
if (E2 is None) and (cov is None) and (prec is None):
cov = S.array(1E-6)
if E2 is not None:
self.E2 = E2
self.cov = self.E2 - self.E1**2
elif cov is not None:
self.cov = cov
self.E2 = E1**2 + cov
elif prec is not None:
self.cov = S.ones(self.E1.shape)/prec
self.E2 = E1**2 + self.cov


def getPrec(self):
'''return the precision of this node from current moments'''
return 1.0/self.cov

def getMean(self):
'''return the mean'''
return self.E1

def getVariance(self):
'''return the variance'''
return self.cov

def entropy(self): 
return 0.5*S.log(2.*S.pi*self.cov).sum()


class CGaussMessage(AGaussNode):
'''class for general Gaussian messages
commonly this is used to represent predictions and messages between expressionmodule'''
def __init__(self,E1=None,E2=None,cov=None,prec=None):
if E1 is not None:
#in caes the CGauss is only constructed from mean
self.setMoments(E1=E1,E2=E2,cov=cov,prec=prec)


class ABayesNet(ANode):
''' Abstract Bayes net class. Consists of nodes, schedule of updates, tolerance for convergence, number of iterations'''

__slots__ = ['nIterations', 'schedule', 'forceIterations', 'tolerance', 'nodes', 'nodeParameters', 'iterationCount', 'name', 'isBoundInvalid','isInitialised']


def getDefaultParameters(self):
'''getDefaultParameters()
- return a dictionary with defaultparameters of the BayesNet class'''
dp = {}
dp['nIterations'] = 1
dp['tolerance']   = 1E-3
dp['forceIterations'] = False
dp['schedule'] = []
dp['name'] = 'NoName'
return dp


def __init__(self,parameters=None, nodeParameters=None):
''' Initialise the Bayes net. Requires a map of names to updatable objects (of type AUnit, called nodes for conciseness), and an update schedule of the node names
- this also takes care of default parameter handling etc.
'''
#0. set default parameters
dp = self.getDefaultParameters()
#update with specified parameters
dp.update(parameters)
#set all parameters as member variables
for param in dp.keys():
self.__setattr__(param,dp[param])

#set a default tollerance for convergence
#if len(self.schedule)==0:
#    self.schedule = nodes.keys()
self.nodes = {} # to be filled by implementation, possibly after construction
self.nodeParameters = nodeParameters
self.iterationCount = 0
self._bound = 0
self.isBoundInvalid = True
self.isInitialised  = False
L.debug('%s ABayesNet init'%self.name)


def init(self, net):
if self.isInitialised: return
L.debug('%s ABayesNet init - to be implemented by subclass'%self.name)



def update(self, net):
'''update of this node per'''
L.debug('%s ABayesNet update'%self.name)
if not self.isInitialised: 
L.error("%s Bayes net update tried without initialising inference"%self.name)
self.init(net)
self.iterate(net=net)



def iterate(self, nIterations=None, forceIterations=None,net=None):
'''iterate(nIteations=None,forceIterations=None)
- perform nIterations; per default(None) parameters are tken from local intsance settings
'''

L.debug('%s ABayesNet iterate'%self.name)

if nIterations is None: nIterations = self.nIterations
if forceIterations is None: forceIterations = self.forceIterations
LB = 0

for iter in range(nIterations):
self.isBoundInvalid = True
self.iterationCount+=1
t = time.time();

for node in self.schedule:
if node in self.nodes.keys():
self.nodes[node].update(self)
else:
raise Exception("node %s in schedule but not in nodes" % node)

self.calcBound(net)

if (abs(LB - self._bound) < self.tolerance) and not forceIterations:
L.info('Converged')
break

level = 10
if self.name == 'net': level = LEVEL_NET
L.log(level, "%s Iteration %d: time=%.2f bound=%f" % (self.name, iter+1,time.time() - t, self._bound))
LB = self._bound

return self._bound


def meanLogProb(self):
L.debug('%s ABayesNet meanLogProb - to be filled by implementing class'%self.name)
return 0;



def calcBound(self, net):        
L.debug('%s ABayesNet calcBound'%self.name)
# calculate only if last calculated value is no longer valid
if self.isBoundInvalid:
self._bound = self.meanLogProb()      
for node in self.nodes:
self._bound += self.nodes[node].calcBound(self)
self.isBoundInvalid = False

return self._bound



# convenience function
def logdet(M):
UC = linalg.cholesky(M)
return 2*sum(S.log(S.diag(UC))) */
