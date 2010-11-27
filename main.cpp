#include <iostream>
#include <Eigen/Eigen>
#include "alglib/src/ap.h"
#include "alglib/src/specialfunctions.h"
#include "vbfa.h"


using namespace Eigen;
using namespace std;


using Eigen::MatrixXf;
using Eigen::VectorXf;
//using Cwise;
using alglib::randomreal;
using alglib::psi;
using alglib::gammafunction;

void playing()
{
	
    // insert code here...
	MatrixXf m(2,2);
	VectorXf v1,v2;
	v1 = VectorXf::Constant(3, 2.1);
	v2 = VectorXf::Constant(3, 1.3);
	v1 = v2;
	v2(1) = 3;
	std::cout << MatrixXf::Ones(3,4)*1.2 << std::endl;
	std::cout << v1 << std::endl << v2 + VectorXf::Ones(3)*2.0 << std::endl;
	m(0,0) = 3;
	m(1,0) = 2.5;
	m(0,1) = -1;
	m(1,1) = m(1,0) + m(0,1);
	std::cout << m << std::endl << m.rowwise().sum() << std::endl << m.colwise().sum() << std::endl;
	std::cout << randomreal() << std::endl << psi(10) << std::endl << gammafunction(10) << std::endl;
	
}



MatrixXf randn(int n, int m)
/* create a randn matrix */
{
	MatrixXf rv(n,m);
	for (int i=0; i<n; i++)
		for (int j=0; j<m; j++) {
			rv(i,j) = randomreal();
		}
	return rv;
}

/** Trying out update equations etc */
void play_matrix(){
	int N = 3;
	int P = 4;
	int K = 2;
	
    MatrixXf alpha = MatrixXf::Zero(K,K);//randn(K,1);
	alpha.diagonal() = randn(K,1);
	MatrixXf XE2s = randn(K,K);
	MatrixXf X = randn(N,K);
	MatrixXf eps = randn(P,1);
	MatrixXf E1 = randn(P,K);
	MatrixXf E2S = randn(K,K);
	MatrixXf pheno = randn(N,P);

	for(int i=0; i<P; ++i){
		MatrixXf prec = alpha + XE2s*eps(i);
		MatrixXf cov = prec.inverse();
	}
}




MatrixXf simulate_expression(int N, int D, int K,double sigma=0.1)
/*
 Simulate an expresison matrix with N entries, D dimensions and K factors
*/
{	
	//1. create factors
	MatrixXf X = randn(N,K);
	MatrixXf W = randn(K,D);
	
	MatrixXf eps = sigma*randn(N,D);
	
	MatrixXf Y = X*W;
	Y = Y + eps;
	//2. output array
	
	return Y;
}


int main (int argc, char * const argv[]) {
	//play_matrix();
	//1. simulate small dataset
	MatrixXf Y=simulate_expression(10,100,5);
	//2. create object
	cVBFA vb(Y,10);
	
	
}
