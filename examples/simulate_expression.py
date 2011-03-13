"""
Simulated expression dataset script
For a complete documentatio please see
https://github.com/PMBio/peer/wiki/
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
    

    #use simulated dataset
    Y = simulate_data(N,K,sigma)
    SP.savetxt('expression.csv',Y)
        
        
