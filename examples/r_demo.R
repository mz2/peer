library(peer)

y = read.csv("expression.csv",header=FALSE)

Kinf = 20
Nmax_iterations = 100

vb = PEER()

#set data and parameters
#number of factor for learning
PEER_setNk(vb,Kinf)

#fit mean effect ?
PEER_setPhenoMean(vb,as.matrix(y))

#set prior settings
#(these are the default settings of PEER)
PEER_setPriorAlpha(vb,0.001,0.1);
PEER_setPriorEps(vb,0.1,10.);
PEER_setNmax_iterations(vb,Nmax_iterations)
PEER_update(vb)

#investigate inferance results
#factors:
X = PEER_getX(vb)
#weights:
W = PEER_getW(vb)
#ARD parameters
Alpha = PEER_getAlpha(vb)

#get corrected dataset:
Yc = PEER_getResiduals(vb)

pdf("r_demo.pdf",width=8,height=8)
plot(1.0 / Alpha,xlab="Factors", ylab="Factor relevance", main="")
dev.off()
