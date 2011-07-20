"""
PEER python API usage example
Fosir a complete documentatio please see
https://github.com/PMBio/peer/wiki/
"""

import sys
import scipy as SP
import sys
#import peer module
sys.path.append('./../build/python')
import peer
import pdb
import pylab as PL

#set PEER verbosity level 
peer.setVerbose(2)

def demo_supervised(Nmax_iterations=100):
	#1. load expression 
	Y = SP.loadtxt('expression_sparse.csv',delimiter=',')
	pi = SP.loadtxt('prior_sparse.csv',delimiter=',')
	Ztrue = SP.loadtxt('groundtruth_sparse.csv',delimiter=',')	
	Nmax_iterations = 200
	#initialize peer object
	vb = peer.PEER()
	#set phenotype data
	vb.setPhenoMean(Y)
	vb.setSparsityPrior(pi)
	#not needed actually, K is determined internally from pi
	vb.setNk(pi.shape[1])
	#set prior on Eps
	vb.setPriorEps(1E-3,1E-3);
	vb.setNmax_iterations(Nmax_iterations)
	vb.update()

	Eps = vb.getEps()
	W = vb.getW()
	X = vb.getX()
	Z = vb.getZ()

	PL.figure()
	PL.imshow(W,aspect='auto')
	PL.figure()
	PL.imshow(Z,aspect='auto')
	pass

def demo_unsupervised(Nmax_iterations=100,Kinf=20):
	Y = SP.loadtxt('expression.csv',delimiter=',')

	#2. run PEER
	#use up to 20 factors
	Kinf = 20
	#maximum number of iterations

	vb = peer.PEER()
	#set data and parameters
	#number of factor for learning
	vb.setNk(Kinf)
	
	#fit mean effect ?
	vb.setAdd_mean(False)
	vb.setPhenoMean(Y)

	#set prior settings
	#(these are the default settings of PEER)
	vb.setPriorAlpha(0.001,0.1);
	vb.setPriorEps(0.1,10);
	vb.setNmax_iterations(Nmax_iterations)
	vb.update()

	#investigate inferance results
	#factors:
	X = vb.getX()
	#weights:
	W = vb.getW()
	#ARD parameters
	Alpha = vb.getAlpha()

	#get corrected dataset:
	Yc = vb.getResiduals()
	#3. plotting (requires matplotlib)
	if 1:
		import pylab as PL
		#plot relevance of factors:
		PL.figure(1)
		PL.plot(1/Alpha)
		PL.xlabel('Factors')
		PL.ylabel('Facotr Relevance')



if __name__ == '__main__':
#	demo_unsupervised(Nmax_iterations=20)
	demo_supervised(Nmax_iterations=20)
