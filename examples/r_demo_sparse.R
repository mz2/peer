library(peer)

y = read.csv("expression.csv",header=FALSE)
pi = read.csv("prior_sparse.csv",header=FALSE)

Kinf = 20
Nmax_iterations = 100

vb = PEER()

#fit mean effect ?
#strangely still need to call VB here
VBFA_setPhenoMean(vb,as.matrix(y))

#set prior
PEER_setSparsityPrior(vb,as.matrix(pi))

#set prior settings
#(these are the default settings of PEER)
VBFA_setPriorEps(vb,0.1,10.);
VBFA_setNmax_iterations(vb,Nmax_iterations)
VBFA_update(vb)

#investigate inferance results
#factors:
X = VBFA_getX(vb)
#weights:
W = VBFA_getW(vb)

