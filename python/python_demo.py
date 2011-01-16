"""Demo script testing the python interface of peer"""

import sys
#TODO: remove, this is only needed if not propperly installed:
sys.path.append('build/lib.macosx-10.6-x86_64-2.6')
import scipy as SP
import peer
import pdb

#1. simulate sth.
N = 100

N = 30
K = 5
G = 20
sigma = 0.1

X = SP.random.randn(N,K)
W = SP.random.randn(K,G)

Y = SP.dot(X,W)
Y+= sigma*SP.random.randn(N,G)

#run peer
#use up to 10 factors
Kinf = 20
Nmax_iterations = 10
vb = peer.VBFA()
#set data and parameters
vb.setNk(Kinf)
vb.setAdd_mean(False)
vb.setPhenoMean(Y)
vb.setNmax_iterations(Nmax_iterations)
vb.update()

#investigate inferance results
#factors:
X = vb.getX()
#weights:
W = vb.getW()
#ARD parameters
Alpha = vb.getAlpha()

if 0:
	import pylab as PL	
	PL.figure()
	PL.plot(Alpha)
