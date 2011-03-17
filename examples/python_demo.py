"""
PEER python API usage example
Fosir a complete documentatio please see
https://github.com/PMBio/peer/wiki/
"""

import sys
import scipy as SP
import peer


if __name__ == '__main__':
	#1. load expression dataset
	Y = SP.loadtxt('expression.csv',delimiter=',')

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
		
