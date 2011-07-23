import peer
import scipy as SP
import pylab as PL

def simple_unsupervised_demo():
    y = SP.loadtxt("data/expression.csv",delimiter=",")
    K = 20
    Nmax_iterations = 100
    model = peer.PEER()
    
    # set data and parameters
    model.setNk(K) #number of factor for learning
    model.setPhenoMean(y) # data for inference
    # set priors (these are the default settings of PEER)
    model.setPriorAlpha(0.001,0.1);
    model.setPriorEps(0.1,10.);
    model.setNmax_iterations(Nmax_iterations)
    # perform inference
    model.update()

    #investigate results
    #factors:
    X = model.getX()
    #weights:
    W = model.getW()
    #ARD parameters
    Alpha = model.getAlpha()
    #get corrected dataset:
    Yc = model.getResiduals()

    # plot variance of factors - in this case, we expect a natural elbow where there are 5 active factors, as 5 were simulated
    PL.plot(1.0 / Alpha)
    PL.xlabel("Factors")
    PL.ylabel("Factor relevance")
    PL.savefig("demo_simple.pdf")
    PL.show()


def get_simple_model_object(K=20, Nmax_iterations=100, expr_file="data/expression.csv"):
    y = SP.loadtxt(expr_file,delimiter=",")
    model = peer.PEER()
    # set data and parameters
    model.setNk(K) #number of factor for learning
    model.setPhenoMean(y) # data for inference
    model.setNmax_iterations(Nmax_iterations)
    return model


def unsupervised_with_covariates_demo():
    model = get_simple_model_object() # see simple_unsupervised_demo for how it is constructed
    covs = SP.loadtxt("data/covs.csv", delimiter=",")
    model.setCovariates(covs) # covariates (e.g batch, RNA quality, ...)
    model.update()

    # plot variance of factors - in this case, we expect the first two factors to have no uncertainty (described by inverse of precision Alpha), and
    # the other 3 of the 5 simulated factors to be captured by the next factors. Note that although 20 factors were fit, 22 values are plotted
    # - the covariates are treated on equal footing with the factors, and included in the factor matrix.
    Alpha = model.getAlpha()
    PL.plot(1.0 / Alpha)
    PL.xlabel("Factors")
    PL.ylabel("Factor relevance")
    PL.savefig("demo_covs.pdf")
    PL.show()


def n_factors_demo():
    PL.figure()
    colors = ("yellow","red","green","blue","black")

    # plot factor weight variances for a large set of maximum number K of factors, and see if K has an effect
    for k in range(2,12,2):
        model = get_simple_model_object(K=k) # see simple_unsupervised_demo for how it is constructed
        model.update()
        PL.plot(1.0 / model.getAlpha())
    # expected result - as soon as there are at least 5 factors, the factor inference will not change any more
    PL.savefig("demo_factors.pdf")
    PL.show()



def eps_prior_demo():
    # plot factor weight variances for a large set of maximum number K of factors, and see if K has an effect
    for pb in (10,100,1000,10000):
        model = get_simple_model_object() # see simple_unsupervised_demo for how it is constructed
        model.setPriorEps(0.1, pb);
        model.update()
        print "Eps pb=", pb, "mean(|residuals|)=",SP.mean(abs(model.getResiduals()))
    # expected result - as the Eps prior b parameter gets larger, soon as there are at least 5 factors, the factor inference will not change any more



def simple_supervised_demo():
    y = SP.loadtxt("data/expression.csv",delimiter=",") # read expression data
    prior = SP.loadtxt("data/prior_sparse.csv",delimiter=",") # and prior for which factor regulates which gene. This matrix has entries between 0 and 1. The (g,k) entry represents the probability that gene g is affected by factor k
    Nmax_iterations = 100

    # create model, set parameters and observed values
    model = PEER()
    model.setPhenoMean(y) # expression levels 
    model.setSparsityPrior(prior) # prior on which factors affect which genes
    model.setPriorEps(0.1,10.);
    model.setNmax_iterations(Nmax_iterations)
    model.update()


def supervised_prior_comparison_demo():
    y = SP.loadtxt("data/expression.csv",delimiter=",") # read expression data - all as in simple_supervised_demo
    prior = SP.loadtxt("data/prior_sparse.csv",delimiter=",")
    Nmax_iterations = 100

    # compare outcomes of inference depending on uncertainty in prior
    for error in (0,0.01,0.1,0.5):
        print "Prior error=",error
        model = PEER()
        model.setPhenoMean(y) # expression levels - note )!
        p = prior
        p[p > 0.5] = (1-error)
        p[p < 0.5] = error
        model.setSparsityPrior(p) # prior on which factors affect which genes
        model.setPriorEps(0.1,10.);
        model.setNmax_iterations(Nmax_iterations)
        model.update()
        for i in range(prior.shape[1]):
            print "Correlation between factor",i, "prior and weight",cor(model.getW()[:,i], prior[:,i]), "sum prior", sum(prior[:,i])



# simple demo of how to use PEER to look for broad variance components.
# 5 global factors were simulated - PEER is expected to capture this
#simple_unsupervised_demo()

# If some known covariates are given, they can be included in inference, like
# shown in this demo. Two of the five simulated global expression modifiers
# are included in the model as covariates, and PEER finds the other three:
#unsupervised_with_covariates_demo()

# Number of factors does not have an effect on PEER, as the unused
# factors are switched off. 
#n_factors_demo()

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
