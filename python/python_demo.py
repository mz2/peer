"""
PPER pythong example
"""

import sys
import scipy as SP
import peer

def simulate_data(N,K,sigma):
	X = SP.random.randn(N,K)
	W = SP.random.randn(K,G)

	Y = SP.dot(X,W)
	#simulate mean effect
	#mean = SP.rand(N)
	Y += Y
	#add noise
	Y+= sigma*SP.random.randn(N,G)
	return Y

if __name__ == '__main__':
	#number of samples
	N = 200
	#number of facotrs
	K = 5
	#number of genes
	G = 100
	#noise level
	sigma = 0.1


	#1. Simulate dataset
	Y = simulate_data(N,K,sigma)

	#2. run PEER
	#use up to 20 factors
	Kinf = 20
	#maximum number of iterations
	Nmax_iterations = 100

	vb = peer.VBFA()
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
		
