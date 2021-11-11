import peer
import scipy as SP
import pylab as PL
import pdb

def simple_unsupervised_demo():
    print "Simple PEER application. All default prior values are set explicitly as demonstration."
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
    plot_Alpha(Alpha)
    PL.savefig("demo_simple.pdf")
    print "Plotted factor relevance"
    PL.show()


def get_simple_model_object(K=10, Nmax_iterations=100, expr_file="data/expression.csv"):
    y = SP.loadtxt(expr_file,delimiter=",")
    model = peer.PEER()
    # set data and parameters
    model.setNk(K) #number of factor for learning
    model.setPhenoMean(y) # data for inference
    model.setNmax_iterations(Nmax_iterations)
    return model


def plot_Alpha(Alpha, color="blue"):
    PL.plot(1.0 / Alpha,lw=4, color=color)
    min_a,max_a = (1.0/Alpha).min(), (1.0/Alpha).max()
    PL.ylim(min_a - 0.1*(max_a - min_a), max_a + 0.1*(max_a - min_a))
    PL.xlabel("Factors")
    PL.ylabel("Factor relevance")


def unsupervised_with_covariates_demo():
    print "PEER with two known covariates included in inference."
    model = get_simple_model_object() # see simple_unsupervised_demo for how it is constructed
    covs = SP.loadtxt("data/covs.csv", delimiter=",")
    model.setCovariates(covs) # covariates (e.g batch, RNA quality, ...)
    model.update()

    # plot variance of factors - in this case, we expect the first two factors to have no uncertainty (described by inverse of precision Alpha), and
    # the other 3 of the 5 simulated factors to be captured by the next factors. Note that although 20 factors were fit, 22 values are plotted
    # - the covariates are treated on equal footing with the factors, and included in the factor matrix.
    plot_Alpha(model.getAlpha())
    PL.savefig("demo_covs.pdf")
    print "Plotting factor relevance"
    PL.show()


def n_factors_demo():
    print "Comparing different numbers of factors in inference (n=2,4,6,10)."
    colors = ("yellow","red","green","blue","black")

    # plot factor weight variances for a large set of maximum number K of factors, and see if K has an effect
    alpha_10 = None
    n_factors = [10,6,4,2]
    for i,k in enumerate(n_factors):
        model = get_simple_model_object(K=k) # see simple_unsupervised_demo for how it is constructed
        model.update()
        plot_Alpha(model.getAlpha(),colors[i])
        if k == 10: alpha_10 = model.getAlpha()
    min_a,max_a = (1./alpha_10).min(), (1./alpha_10).max()
    PL.ylim(min_a - 0.1*(max_a - min_a), max_a + 0.1*(max_a - min_a))
    PL.legend(['K=%d'%f for f in n_factors])
    # expected result - as soon as there are at least 5 factors, the factor inference will not change any more
    PL.savefig("demo_factors.pdf")
    print "Plotting factor relevances"
    PL.show()



def eps_prior_demo():
    print "Comparing different noise priors to see the effect on how aggressively PEER explains variability."

    # plot factor weight variances for a large set of maximum number K of factors, and see if K has an effect
    for pa in (0.0001, 0.1, 1000):
        for pb in (0.1,10,1000):
            model = get_simple_model_object() # simple object using default simulated dataset; see simple_unsupervised_demo for how it is constructed
            model.setPriorEps(pa,pb);
            model.update()
            print "Eps pa=%.4f pb=%.4f mean(residuals^2)=%.4f"%(pa, pb, SP.mean(model.getResiduals()**2))


def simple_supervised_demo():
    print "Simple demo of supervised factor inference"
    model = get_simple_model_object(expr_file='data/expression_sparse.csv') # simple object using default simulated dataset; see simple_unsupervised_demo for how it is constructed
    prior = SP.loadtxt("data/prior_sparse.csv",delimiter=",") # and prior for which factor regulates which gene. This matrix has entries between 0 and 1. The (g,k) entry represents the probability that gene g is affected by factor k
    model.setSparsityPrior(prior) # prior on which factors affect which genes
    model.update()
    for i in range(prior.shape[1]):
        print "Correlation between factor",i, "prior and weight",SP.corrcoef(model.getW()[:,i], prior[:,i])[0,1], "sum prior", sum(prior[:,i])


def supervised_prior_comparison_demo():
    print "Supervised factor inference demo, comparing different error rates in prior specification"
    prior = SP.loadtxt("data/prior_sparse.csv",delimiter=",")

    # compare outcomes of inference depending on uncertainty in prior
    for error in (0,0.01,0.1,0.2):
        print "Prior error=",error
        model = get_simple_model_object(expr_file='data/expression_sparse.csv') # simple object using default simulated dataset; see simple_unsupervised_demo for how it is constructed
        p = prior
        p[p > 0.5] = (1-error)
        p[p < 0.5] = error
        model.setSparsityPrior(p) # prior on which factors affect which genes
        model.update()

        for i in range(prior.shape[1]):
            if SP.isnan(model.getW()).any(): pdb.set_trace()
            elif SP.isnan(SP.corrcoef(model.getW()[:,i], prior[:,i])[0,1]): pdb.set_trace()
            print "Correlation between factor",i, "prior and weight",SP.corrcoef(model.getW()[:,i], prior[:,i])[0,1], "sum prior", sum(prior[:,i])



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
