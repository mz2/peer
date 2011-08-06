Usage examples for PEER in all supported interfaces.
For a complete documentation please see

https://github.com/PMBio/peer/wiki/



##Data files for tutorial examples##

*data/brem_expr.csv 	 
	Expression data for 109 BY x RM yeast segregants grown in
	glucose. Expreiments described in Brem et al, 
	Smith and Kruglyak. Data preprocessing described in
	Parts*,Stegle*,Winn,Durbin (2011). 
*data/brem_genotype.csv
	Brem genotype data
*data/brem_yeastract_expr.csv	
	Brem expression dataset, filtered to only genes with factors targeting
	them as described in Parts*,Stegle*,Winn,Durbin (2011). 
*data/brem_yeastract_prior.csv	
	Yeastract prior for Brem data 
*data/simulate_expression.py	
	script to simulate the expression dataset 
*data/expression.csv
	simulated expression dataset
*data/genotypes.csv
	simulated genotypes
*data/covs.csv
	simulated covariates
*data/groundtruth_sparse.csv
	simulation ground truth
*data/expression_sparse.csv
	simulated expression using the sparse prior
*data/prior_sparse.csv
	simulated sparse prior

##Example script, illustrating PEER usage##

*python_demo.py
	example usage of pyton API
*standalone_demo.sh
	usage example for the standalone API
*r_demo.R
	 example usage of R API
*brem_exploration.R
	a case study in exploring expression variation in PEER using data from
	Brem and Kruglyak yeast segregants 
*paper_plots.R
	scripts used to create plots in the protocols paper
*helpers.R
	misc. R scripts used by paper_plots.R


##Acknowledgements## 
We thank Rachel Brem and Kruglyak Leonid for permission 
to include their primary data for illustration in this package.  
