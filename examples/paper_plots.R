library(peer)
library(qtl)
source("helpers.R")

unsupervised_plots <- function(n_factors=10, n_iterations=10, n_genes=200, load=FALSE){
    cross = read.cross(format="csvs", dir="./data/", genotypes=c("0","1"), alleles=c("0","1"), genfile="brem_genotype.csv", phefile="brem_expr.csv", estimate.map=FALSE)
    # First, let's infer the unsupervised factors and residuals
    # create factor analysis object and set its properties
    X = 0
    if(load){
        X = read.table("store/unsupervised_X.tab")
        raw_lods = read.table("store/raw_lods.tab")
        residual_lods = read.table("store/residual_lods.tab")
    }
    else{
        model = PEER()
        PEER_setNk(model, n_factors)
        PEER_setNmax_iterations(model, n_iterations)
        PEER_setPhenoMean(model,as.matrix(cross$pheno))
        # infer hidden factors and residuals
        PEER_update(model) 
        X = PEER_getX(model)
        write.table(X, file="store/unsupervised_X.tab")
        residuals_cross = cross
        residuals_cross$pheno <- PEER_getResiduals(model)
        colnames(residuals_cross$pheno) <- colnames(cross$pheno)
        raw_lods = scanone(cross, pheno.col=2:(n_genes+1), method="mr")
        residual_lods = scanone(residuals_cross, pheno.col=2:(n_genes+1), method="mr")
        write.table(raw_lods, file="store/raw_lods.tab")
        write.table(residual_lods, file="store/residual_lods.tab")
    }
    
    # count raw number of cis associations for both sets of LOD scores - we expect to find more eQTLs in cis
    probe_locations = get_probe_locs(cross, n_genes)
    lod_cutoffs = 1:80/2.
    raw_eqtls = count_eqtls(raw_lods, probe_locations, lod_cutoffs)
    residual_eqtls = count_eqtls(residual_lods, probe_locations, lod_cutoffs)

    # plot the results - do we find more eQTLs robustly for different cutoffs?
    pdf("store/paper_eqtl_discovery.pdf")
    plot(lod_cutoffs, raw_eqtls, "l", lwd=6, col="blue", xlab="LOD cutoff", ylab="# genes with eQTL", cex.lab=1.5, cex.axis=1.2, yaxp=c(0,5500,5))
    lines(lod_cutoffs, residual_eqtls, "l", lwd=6, col="red")
    legend(25,5300,c("Standard","Using PEER"), fill=c("red","blue"), cex=2)
    dev.off()

    # plot frequency of associations called at 0.05 false discovery rate
    pdf("store/paper_eqtl_frequency.pdf", width=8, height=4)
    qvals = apply(raw_lods[,3:(max_genes+2)],2,function(x){p.adjust(dchisq(2*log(10)*x,1), method="fdr")}) # Apply chi2(1) density calculation to the LOD scores (in base 10) to get p-values, and use FDR adjustment to get q-values
    frequency = apply(qvals < 0.05, 1, sum)
    write.table(frequency, file="store/eqtl_frequency.tab")
    par(cex.axis=2, cex.lab=3, lwd=1, mex=1.3, mai=c(1.1,1.5,0.2,0.2))
    plot(frequency, xlab="", ylab="# eQTLs", type="l", col="blue", lw=6, xaxt="n")
    dev.off()
}


supervised_plot <- function(n_iterations=40, load=FALSE){
    cross = read.cross(format="csvs", dir="./data", genotypes=c("0","1"), alleles=c("0","1"), genfile="brem_genotype.csv", phefile="brem_yeastract_expr.csv", estimate.map=FALSE)
    prior = read.csv("brem_yeastract_prior.csv", header=FALSE, row.names=1)
    I = (apply(prior, 1, sum) > 150)
    prior = prior[I,]

    if(!load){
        model = PEER()
        PEER_setPhenoMean(model, as.matrix(cross$pheno)[,1:dim(cross$pheno)[2]-1]) # last column is ID
        PEER_setSparsityPrior(model, t(prior))
        n_factors = dim(prior)[1]
        PEER_setNmax_iterations(model, n_iterations)
        PEER_update(model)      
        write.csv(PEER_getX(model), "store/supervised_full_X2.csv")
    }
    cross$pheno = read.csv("store/supervised_full_X2.csv", header=TRUE, row.names=1)
    lods = scanone(cross, pheno.col=1:62, method="mr")
    pdf("store/paper_sparse_linkage2.pdf",width=8,height=4)
    par(cex.axis=2, cex.lab=3, lwd=1, mex=1.3, mai=c(1.1,1.5,0.2,0.2))
    to_plot = c(2,19,34)
    plot.scanone(lods, lodcol=to_plot, xlab="Yeast chromosomes", ylab="LOD", lwd=6)    
    dev.off()
    print(rownames(prior)[to_plot])
}

supervised_plot(n_iterations=40)
unsupervised_plots(n_iterations=40, n_factors=20, n_genes=5490)
