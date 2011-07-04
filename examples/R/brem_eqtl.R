library(peer)
library(qtl)

# read information on cross (Brem 2005, Smith 2008)
cross = read.cross(format="csv", dir="examples/R/", file="brem_cross.csv", genotypes=c("0","1"), alleles=c("0","1"))

# create factor analysis object and set its properties
vb = VBFA()
VBFA_setNk(vb,8)
VBFA_setNmax_iterations(vb, 5)
VBFA_setPhenoMean(vb,as.matrix(cross$pheno[,0:100])) # full dataset does not do useful things
# infer hidden factors and residuals
VBFA_update(vb) 

# create other cross object where the phenotypes are residuals or hidden factors
residuals_cross = cross
residuals_cross$pheno <- VBFA_getResiduals(vb)
factors_cross = cross
factors_cross$pheno <- VBFA_getX(vb)

# calculate LOD scores for chromosome 1 loci against all probes using marker regression
raw_lods = scanone(cross, 1, 1:500, method="mr")
residual_lods = scanone(residuals_cross, 1, 1:500, method="mr")
# count raw number of associations for both sets of LOD scores - we expect to find more eQTLs in cis
sum(lods[,2:5495] > 10)
sum(residual_lods[,2:5495] > 10)

# calculate genetic association to the 20 hidden factors
factor_lods = scanone(factors_cross, 1:20, method="mr")
sum(factor_lods[,2:22] > 10)
# see if any of the first 3 factors has a strong LOD peak
plot(factor_lods, lodcolumn=1:3)
