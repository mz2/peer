library(peer)

y = read.csv("expression.csv",header=FALSE)

Kinf = 20
Nmax_iterations = 100

vb = VBFA()

#set data and parameters
#number of factor for learning
VBFA_setNk(vb,Kinf)

#fit mean effect ?
VBFA_setAdd_mean(vb,FALSE)
VBFA_setPhenoMean(vb,y)

#set prior settings
#(these are the default settings of PEER)
VBFA_setPriorAlpha(vb,0.001,0.1);
VBFA_setPriorEps(vb,0.1,10);
VBFA_setNmax_iterations(vb,Nmax_iterations)
VBFA_update(vb)

#investigate inferance results
#factors:
X = VBFA_getX(vb)
#weights:
W = VBFA_getW(vb)
#ARD parameters
Alpha = VBFA_getAlpha(vb)

#get corrected dataset:
Yc = VBFA_getResiduals()

plot(1.0 / Alpha,xlab="Factors", ylab="Factor relevance", main="")