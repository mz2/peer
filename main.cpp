#include <iostream>
#include <Eigen/Eigen>
#include "alglib/src/ap.h"
#include "alglib/src/specialfunctions.h"
#include "vbfa.h"
#include "sim.h"

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



/** Trying out update equations etc */
void play_matrix(){
	int N = 3;
	int P = 4;
	int K = 2;

    MatrixXf alpha = MatrixXf::Zero(K,K);//randn(K,1);
	alpha.diagonal() = randn(K,1);
	cout << alpha << endl;
	MatrixXf XE2s = randn(K,K);
	MatrixXf X = randn(N,K);
	MatrixXf eps = randn(P,1);
	MatrixXf E1 = randn(P,K);
	MatrixXf E2S = randn(K,K);
	MatrixXf pheno = randn(N,P);
	MatrixXf b = 0.1 + 0.5*E2S.diagonal().array();
	MatrixXf a = (10.0 + 0.5*P)*(MatrixXf::Ones(K, 1).array());
	
	
	// W update 
	for(int i=0; i<P; ++i){
		MatrixXf prec = alpha + XE2s*eps(i);
		MatrixXf cov = prec.inverse();
		// cout << alpha << endl << XE2s << endl << XE2s*eps(i) << endl << prec << endl << cov << endl;
		cout << pheno.col(i).rows() << pheno.col(i).cols() << endl << X.rows() << X.cols() << endl;
		cout << eps(i)*cov*X.transpose()*pheno.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		E1.row(i) = eps(i)*cov*X.transpose()*pheno.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
//		E1(i) = eps(i)*cov*X*pheno(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		MatrixXf outer = E1.row(i).transpose()*E1.row(i);
		E2S += (cov + outer); //  E2 = dcov + outer(self.E1[d], self.E1[d])
		
	}
}


int main (int argc, char * const argv[]) {
	//play_matrix();
	//1. simulate small dataset
	MatrixXf Y = simulate_expression(10,100,5,0.01).expr;
	MatrixXf Yvar = 0.1*MatrixXf::Ones(10,100);
	//2. create object
	//test with nonexisting variance:
	Yvar = MatrixXf();
	cVBFA vb(Y,Yvar, MatrixXf(), 8);
	vb.init_net();
	vb.update();
	cout << vb.Alpha.E1 << endl << vb.X.E1.col(0);
	
	double** m;
	int* rows;
	int* cols;
	vb.X.getE1(m, rows, cols);
	
}
