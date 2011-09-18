library(peer)
library(qtl)
source("helpers.R")

# A 'real-life' application of PEER, exploring the gene expression data in the set
# of yeast segregants established by Brem and Kruglyak.

unsupervised_exploration <- function(cross, n_factors=10, n_iterations=10, max_n_genes=200){
    # First, let's infer the unsupervised factors and residuals
    # create factor analysis object and set its properties
    model = PEER()
    PEER_setNk(model, n_factors)
    PEER_setNmax_iterations(model, n_iterations)
    PEER_setPhenoMean(model,as.matrix(cross$pheno))
    # infer hidden factors and residuals
    PEER_update(model) 

    # Now, let us do some mapping of genetic basis of the inferred factors
    # Create a new cross object, with phenotypes switched for the factor activations
    factors_cross = cross
    factors_cross$pheno <- PEER_getX(model)
    colnames(factors_cross$pheno) <- 1:n_factors 
    # calculate genetic association to the 20 hidden factors
    factor_lods = scanone(factors_cross, pheno.col=1:n_factors, method="mr")
    # see if any of the first 3 factors has a strong LOD peak
    pdf("test1.pdf")
    plot(factor_lods, lodcolumn=1:3, lwd=2, ylab="LOD", xlab="Yeast chromosomes")
    dev.off()

    # Finally, let us compare mapping the genetic basis of expression traits on the original data and residuals
    # We only use the first [max_n_genes] expression levels
    # create other cross object where the phenotypes are residuals or hidden factors
    residuals_cross = cross
    residuals_cross$pheno <- PEER_getResiduals(model)
    colnames(residuals_cross$pheno) <- colnames(cross$pheno)
    raw_lods = scanone(cross, pheno.col=1:max_n_genes, method="mr")
    residual_lods = scanone(residuals_cross, pheno.col=1:max_n_genes, method="mr")
    np_lods = scanone(cross, pheno.col=1:max_n_genes, model="np")
    covariate_lods = scanone(cross, pheno.col=1:max_n_genes, method="mr", addcovar=PEER_getX(model))
    
    # look at raw number of associations for all sets of LOD scores - we expect to find more eQTLs in cis, but not necessarily in trans
    probe_locations = get_probe_locs(cross)[1:max_n_genes]
    lod_cutoffs = 1:30

    # plot the results - do we find more cis eQTLs robustly for different cutoffs and alternative calling methods?
    pdf("test2.pdf")
    par(lwd=6, cex.lab=1.5, cex.axis=1.2)    
    plot(lod_cutoffs, count_eqtls(raw_lods, probe_locations, lod_cutoffs), "l", col="blue", xlab="LOD cutoff", ylab="# cis eQTLs")
    lines(lod_cutoffs, count_eqtls(residual_lods, probe_locations, lod_cutoffs), col="red")
    lines(lod_cutoffs, count_eqtls(covariate_lods, probe_locations, lod_cutoffs), col="green")
    lines(lod_cutoffs, count_eqtls(np_lods, probe_locations, lod_cutoffs), col="yellow")
    dev.off()
    # plot the results - do we find more total eQTLs robustly for different cutoffs and alternative calling methods?
    pdf("test3.pdf")
    par(lwd=6, cex.lab=1.5, cex.axis=1.2)
    plot(lod_cutoffs, count_eqtls(raw_lods, probe_locations, lod_cutoffs, cis_only=FALSE), "l", col="blue", xlab="LOD cutoff", ylab="# eQTLs")
    lines(lod_cutoffs, count_eqtls(residual_lods, probe_locations, lod_cutoffs, cis_only=FALSE), col="red")
    lines(lod_cutoffs, count_eqtls(covariate_lods, probe_locations, lod_cutoffs, cis_only=FALSE), col="green")
    lines(lod_cutoffs, count_eqtls(np_lods, probe_locations, lod_cutoffs, cis_only=FALSE), col="yellow")
    dev.off()
}


supervised_exploration <- function(cross, n_iterations=10, max_n_genes=200){
    # First, let's infer the unsupervised factors and residuals
    # create factor analysis object and set its properties
    model = PEER()
    PEER_setPhenoMean(model, as.matrix(cross$pheno[,1:max_n_genes]))
    prior = as.matrix(read.csv('data/brem_yeastract_prior.csv', header=FALSE, row.names=1))
    I = (apply(prior, 1, sum) > 150) # filter for factors with a lot of targets to speed up inference
    prior = prior[I,]
    print(dim(prior))
    print(dim(cross$pheno))
    PEER_setSparsityPrior(model, t(prior))
    n_factors = dim(prior)[1]
    PEER_setNmax_iterations(model, n_iterations)
    # infer hidden factors and residuals
    PEER_update(model)
    factors = PEER_getX(model)

    # Now, let us do some mapping of genetic basis of the inferred factors
    # Create a new cross object, with phenotypes switched for the factor activations
    factors_cross = cross
    factors_cross$pheno <- factors
    colnames(factors_cross$pheno) <- 1:n_factors
    # calculate genetic association to the 20 hidden factors
    factor_lods = scanone(factors_cross, pheno.col=1:n_factors, method="mr")
    # see if any of the first 3 factors has a strong LOD peak
    pdf("test_sparse.pdf")
    plot(factor_lods, lodcolumn=1:3, lwd=2, ylab="LOD", xlab="Yeast chromosomes")
    dev.off()
}

#cross <- read.cross(format="csvs", dir="./data", genfile="brem_genotype.csv", phefile="brem_expr.csv",genotypes=c("0","1"), alleles=c("0","1"))
#unsupervised_exploration(cross, n_iterations=40, n_factors=30, max_n_genes=1000)
cross <- read.cross(format="csvs", dir="./data", genfile="brem_genotype.csv", phefile="brem_yeastract_expr.csv",genotypes=c("0","1"), alleles=c("0","1"))
supervised_exploration(cross, max_n_genes=2941, n_iterations=40)
