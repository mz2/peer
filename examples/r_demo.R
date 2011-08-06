library(peer)
library(qtl)
source("helpers.R")


simple_unsupervised_demo <- function(){
    y = read.csv("data/expression.csv",header=FALSE)
    K = 20
    Nmax_iterations = 100
    model = PEER()
    
    # set data and parameters
    PEER_setNk(model, K) #number of factor for learning
    PEER_setPhenoMean(model, as.matrix(y)) # data for inference - note the as.matrix() !
    # set priors (these are the default settings of PEER)
    PEER_setPriorAlpha(model,0.001,0.1);
    PEER_setPriorEps(model,0.1,10.);
    PEER_setNmax_iterations(model,Nmax_iterations)
    # perform inference
    PEER_update(model)

    #investigate results
    #factors:
    X = PEER_getX(model)
    #weights:
    W = PEER_getW(model)
    #ARD parameters
    Alpha = PEER_getAlpha(model)
    #get corrected dataset:
    Yc = PEER_getResiduals(model)

    # plot variance of factors - in this case, we expect a natural elbow where there are 5 active factors, as 5 were simulated
    pdf("r_demo.pdf",width=8,height=8)
    plot(1.0 / Alpha,xlab="Factors", ylab="Factor relevance", main="")
    dev.off()
}


unsupervised_with_covariates_demo <- function(){
    model = get_simple_PEER_object() # see simple_unsupervised_demo for how it is constructed
    covs = read.csv("data/covs.csv", header=FALSE)
    PEER_setCovariates(model, as.matrix(covs)) # covariates (e.g batch, RNA quality, ...) - not the as.matrix()!
    PEER_update(model)

    # plot variance of factors - in this case, we expect the first two factors to have no uncertainty (described by inverse of precision Alpha), and
    # the other 3 of the 5 simulated factors to be captured by the next factors. Note that although 20 factors were fit, 22 values are plotted
    # - the covariates are treated on equal footing with the factors, and included in the factor matrix.
    Alpha = PEER_getAlpha(model)
    pdf("r_demo_covs.pdf",width=8,height=8)
    plot(1.0 / Alpha,xlab="Factors", ylab="Factor relevance", main="")
    dev.off()
}


n_factors_demo <- function(){
    pdf("r_demo_nk.pdf",width=8,height=8)
    colors = c("yellow","red","green","blue","black")

    # plot factor weight variances for a large set of maximum number K of factors, and see if K has an effect
    for (k in (1:5)*2){
        model = get_simple_PEER_object(K=k) # see simple_unsupervised_demo for how it is constructed
        PEER_update(model)
        if (k==2) {  plot(1.0 / PEER_getAlpha(model),xlab="Factors", ylab="Factor relevance", main="", type="l",xlim=c(0,10), ylim=c(0,1000))}
        else {  lines(1.0 / PEER_getAlpha(model), type="l", col=colors[k/2])}
    }
    # expected result - as soon as there are at least 5 factors, the factor inference will not change any more
    dev.off()
}



eps_prior_demo <- function(){
    # plot factor weight variances for a large set of maximum number K of factors, and see if K has an effect
    for (pb in c(10,100,1000,10000)){
        model = get_simple_PEER_object() # see simple_unsupervised_demo for how it is constructed
        PEER_setPriorEps(model,0.1, pb);
        PEER_update(model)
        print(paste("Eps pb=", pb, "mean(|residuals|)=",mean(abs(PEER_getResiduals(model)))))
    }
    # expected result - as the Eps prior b parameter gets larger, soon as there are at least 5 factors, the factor inference will not change any more
}



simple_supervised_demo <- function(){
    y = read.csv("data/expression.csv",header=FALSE) # read expression data
    prior = as.matrix(read.csv("data/prior_sparse.csv",header=FALSE)) # and prior for which factor regulates which gene. This matrix has entries between 0 and 1. The (g,k) entry represents the probability that gene g is affected by factor k
    Nmax_iterations = 100

    # create model, set parameters and observed values
    model = PEER()
    PEER_setPhenoMean(model, as.matrix(y)) # expression levels - note as.matrix()!
    PEER_setSparsityPrior(model, as.matrix(prior)) # prior on which factors affect which genes
    PEER_setPriorEps(model,0.1,10.);
    PEER_setNmax_iterations(model, Nmax_iterations)
    PEER_update(model)
}


supervised_prior_comparison_demo <- function(){
    y = read.csv("data/expression.csv",header=FALSE) # read expression data - all as in simple_supervised_demo
    prior = as.matrix(read.csv("data/prior_sparse.csv",header=FALSE)) 
    Nmax_iterations = 100

    # compare outcomes of inference depending on uncertainty in prior
    for(error in c(0,0.01,0.1,0.5)){
        print(paste("Prior error=",error))
        model = PEER()
        PEER_setPhenoMean(model, as.matrix(y)) # expression levels - note as.matrix()!
        p = prior
        p[p > 0.5] = (1-error)
        p[p < 0.5] = error
        PEER_setSparsityPrior(model, as.matrix(p)) # prior on which factors affect which genes
        PEER_setPriorEps(model,0.1,10.);
        PEER_setNmax_iterations(model, Nmax_iterations)
        PEER_update(model)
        for(i in 1:dim(prior)[2]){
            print(paste("Correlation between factor",i, "prior and weight",cor(PEER_getW(model)[,i], prior[,i]), "sum prior", sum(prior[,i])))
        }
    }
}




# simple demo of how to use PEER to look for broad variance components.
# 5 global factors were simulated - PEER is expected to capture this
simple_unsupervised_demo()

# If some known covariates are given, they can be included in inference, like
# shown in this demo. Two of the five simulated global expression modifiers
# are included in the model as covariates, and PEER finds the other three:
unsupervised_with_covariates_demo()

# Number of factors does not have an effect on PEER, as the unused
# factors are switched off. 
n_factors_demo()

# Prior on the noise level or factor weight precisions affects factor inference
# In general the prior parameters for Alpha and Eps can be tweaked to explain some
# amount of variability in the data. In many settings, PEER is robust to changes in priors
# for several orders of magnitude
eps_prior_demo()


# Demo on supervised factor learning. A prior information matrix has to be given that determines
# how likely each factor is to affect every gene.
simple_supervised_demo()


# Prior information can be uncertain - this demo explores the effect of having potential for error in the prior
supervised_prior_comparison_demo()
