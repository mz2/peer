"""
Simulated expression dataset script
For a complete documentatio please see
https://github.com/PMBio/peer/wiki/
"""

import sys
import scipy as SP

def simulate_data(N,K,sigma):
	X = SP.random.randn(N,K)
	W = SP.random.randn(K,G)

	Y = SP.dot(X,W)
	#simulate mean effect
	#mean = SP.rand(N)
	Y += Y
	#add noise
	Y+= sigma*SP.random.randn(N,G)
	return {'Y':Y,'W':W,'X':X}


def simulate_data_sparse(N,K,sparsity=1E-2,fpr=0.05, fnr = 0.01, sigma=0.1):
	"""simulate expression dataset with N individuals, K factors and a given sparsity"""
	#1. simulate prior matrix with probability of connectivity
	Zp = (SP.random.rand(K,G)<sparsity)
	Pi = SP.zeros([K,G])
	Pi[Zp] = (1-fpr)
	Pi[~Zp] = fnr
	#2. sample actual structure
	Z = 1.0*(SP.random.rand(K,G)<Pi)
	W = SP.random.randn(K,G)*Z
	X = SP.random.randn(N,K)
	Y = SP.dot(X,W)
	Y += sigma*SP.random.randn(N,G)
	return {'Y':Y,'W':W,'X':X,'Pi':Pi,'Z':Z}
	


if __name__ == '__main__':
    SP.random.seed(1)
    #number of samples
    N = 200
    #number of facotrs
    K = 5
    #number of genes
    G = 100
    #noise level
    sigma = 0.1
    

    #simulate non-sparse expression dataset
    RV0 = simulate_data(N,K,sigma)
    SP.savetxt('expression.csv',RV0['Y'],delimiter=',')
    SP.savetxt('covs.csv', RV0['X'][:,0:2], delimiter=",")

    #simulate sparse expression dataset
    RV1 = simulate_data_sparse(N,K,sigma)
    SP.savetxt('expression_sparse.csv',RV1['Y'],delimiter=',')
    SP.savetxt('prior_sparse.csv',RV1['Pi'].T,delimiter=',')
    SP.savetxt('groundtruth_sparse.csv',RV1['Z'],delimiter=',')
    
    
        
        
