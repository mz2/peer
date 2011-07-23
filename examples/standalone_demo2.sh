# Simple peer application. All default prior values are set explicitly as an example
./peertool -f data/expression.csv -n 20 -i 100 --a_pa 0.001 --a_pb 0.1 --e_pa 0.1 --e_pb 10 -o peer_out_simple
# An equivalent of the above would be
./peertool -f data/expression.csv -n 20 -i 100 -o peer_out_simple

# PEER with covariates
./peertool -f data/expression.csv -c data/covs.csv -n 20 -i 100 -o peer_out_covs

# comparing different numbers of factors
./peertool -f data/expression.csv -n 1 -i 100 -o peer_out_n-1
./peertool -f data/expression.csv -n 2 -i 100 -o peer_out_n-2
./peertool -f data/expression.csv -n 5 -i 100 -o peer_out_n-5
./peertool -f data/expression.csv -n 10 -i 100 -o peer_out_n-10 

# Comparing different noise prior settings
./peertool -f data/expression.csv -n 20 -i 100 --e_pa 0.1 -o peer_out_ea-0.1
./peertool -f data/expression.csv -n 20 -i 100 --e_pa 1 -o peer_out_ea-1
./peertool -f data/expression.csv -n 20 -i 100 --e_pa 10 -o peer_out_ea-10
./peertool -f data/expression.csv -n 20 -i 100 --e_pa 100 -o peer_out_ea-100

# Supervised PEER
./peertool -f data/expression_sparse.csv --prior data/prior.csv -i 100 -o peer_out_supervised
