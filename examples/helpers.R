# Helper functions that are used in tutorials


# probe locations from cross phenotype names - names are in form X[chr]_[start]_[end] (without []) - transform this to 10^9 * chr + probe midpoint
get_probe_locs <- function(cross){
    locfun <- function(x) {d=lapply(strsplit(substr(x,2,99),"_")[[1]], as.integer);1e9*d[[1]] + 0.5*(d[[2]] + d[[3]])}
    I = colnames(cross$pheno) != "ID"
    lapply(colnames(cross$pheno)[I], locfun) # probe names are chr_start_end  - transform that into an int
}

# Helper function - number of eQTLs for a list of thresholds
count_eqtls <- function(lods, probe_locs, cutoffs, cis_only=TRUE, cis_distance=10000){
    snplocs = 1e9*as.numeric(lods[,1]) + lods[,2]
    lods = lods[,3:dim(lods)[2]] # first two columns are chromosome and position
    I = (lods >= 0) # which SNP-probe pairs to consider - default all
    
    if(cis_only){
        for (i in 1:length(probe_locs)){ 
            I[,i] = (abs(snplocs - probe_locs[[i]]) < cis_distance) # cis pairs are ones where SNP and probe location are within distance
        } 
    }
    
    max_lods = apply(lods*I, 2, max) # take best LOD for each probe
    count = rep(0,length(cutoffs)) # count probes with eQTLs at 100 cutoffs
    for (cutoff in cutoffs) {
        count[cutoff] = sum(max_lods > cutoff)
    }
    count
}



get_simple_PEER_object <- function(K=20, Nmax_iterations=100, expr_file="data/expression.csv"){
    y = read.csv(expr_file,header=FALSE)
    model = PEER()
    # set data and parameters
    PEER_setNk(model, K) #number of factor for learning
    PEER_setPhenoMean(model, as.matrix(y)) # data for inference - note the as.matrix() !
    PEER_setNmax_iterations(model,Nmax_iterations)
    model
}


