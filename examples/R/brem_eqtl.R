library(peer)
library(qtl)

unsupervised_demo <- function(cross, n_factors=10, n_iterations=10, n_genes=200){
    # First, let's infer the unsupervised factors and residuals
    # create factor analysis object and set its properties
    vb = VBFA()
    VBFA_setNk(vb, n_factors)
    VBFA_setNmax_iterations(vb, n_iterations)
    VBFA_setPhenoMean(vb,as.matrix(cross$pheno))
    # infer hidden factors and residuals
    VBFA_update(vb) 

    # Now, let us do some mapping of genetic basis of the inferred factors
    # Create a new cross object, with phenotypes switched for the factor activations
    factors_cross = cross
    factors_cross$pheno <- VBFA_getX(vb)
    colnames(factors_cross$pheno) <- 1:(n_factors + 1) 
    # calculate genetic association to the 20 hidden factors
    factor_lods = scanone(factors_cross, pheno.col=2:(n_factors + 1), method="mr")
    # see if any of the first 3 factors has a strong LOD peak
    pdf("test1.pdf")
    plot(factor_lods, lodcolumn=1:3, lwd=2, ylab="LOD", xlab="Yeast chromosomes")
    dev.off()

    # Finally, let us compare mapping the genetic basis of expression traits on the original data and residuals
    # We only use the first 100 expression levels
    # create other cross object where the phenotypes are residuals or hidden factors
    residuals_cross = cross
    residuals_cross$pheno <- VBFA_getResiduals(vb)
    colnames(residuals_cross$pheno) <- colnames(cross$pheno)
    raw_lods = scanone(cross, pheno.col=1:n_genes, method="mr")
    residual_lods = scanone(residuals_cross, pheno.col=1:n_genes, method="mr")
    
    # count raw number of cis associations for both sets of LOD scores - we expect to find more eQTLs in cis
    probe_locations = get_probe_locs(cross)
    lod_cutoffs = 1:50
    raw_eqtls = count_eqtls(raw_lods, probe_locations, lod_cutoffs)
    residual_eqtls = count_eqtls(residual_lods, probe_locations, lod_cutoffs)
    # plot the results - do we find more eQTLs robustly for different cutoffs?
    pdf("test2.pdf")
    plot(lod_cutoffs, raw_eqtls, "l", lwd=6, col="blue", xlab="LOD cutoff", ylab="# eQTLs", cex.lab=1.5, cex.axis=1.2)
    lines(lod_cutoffs, residual_eqtls, "l", lwd=6, col="red")
    dev.off()
}


# Helper function - probe locations from cross 
get_probe_locs <- function(cross){
    locfun <- function(x) {d=lapply(strsplit(substr(x,2,99),"_")[[1]], as.integer);1e9*d[[1]] + 0.5*(d[[2]] + d[[3]])}
    lapply(colnames(cross$pheno), locfun) # probe names are chr_start_end  - transform that into an int
}

# Helper function - number of eQTLs for a list of thresholds
count_eqtls <- function(lods, probe_locs, cutoffs, cis_only=TRUE, cis_distance=10000){
    snplocs = 1e9*as.numeric(lods[,1]) + lods[,2]
    lods = lods[,3:dim(lods)[2]]
    I = (lods >= 0) # which SNP-probe pairs to consider - default all
    
    if(cis_only){
        for (i in 1:length(probe_locs)){ 
            I[,i] = (abs(snplocs - probe_locs[[i]]) < cis_distance) # cis pairs are ones where SNP and probe location are within distance
        } 
    }
    
    max_lods = apply(lods*I, 2, max) # take best LOD for each probe
    count = rep(0,100) # count probes with eQTLs at 100 cutoffs
    for (cutoff in cutoffs) {
        count[cutoff] = sum(max_lods > cutoff)
    }
    count
}

# read information on cross (Brem 2005, Smith 2008)
cross = read.cross(format="csv", dir="./", file="brem_cross.csv", genotypes=c("0","1"), alleles=c("0","1"))
unsupervised_demo(cross)