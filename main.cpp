#include <iostream>
#include <Eigen/Eigen>
#include "alglib/src/ap.h"
#include "alglib/src/specialfunctions.h"
#include "vbfa.h"
#include "sim.h"

using namespace Eigen;
using namespace std;
using namespace PEER;


//using Cwise;
using alglib::randomreal;
using alglib::psi;
using alglib::gammafunction;


void playing()
{
	
    // insert code here...
	PMatrix m(2,2);
	PVector v1,v2;
	v1 = PVector::Constant(3, 2.1);
	v2 = PVector::Constant(3, 1.3);
	v1 = v2;
	v2(1) = 3;
	std::cout << PMatrix::Ones(3,4)*1.2 << std::endl;
	std::cout << v1 << std::endl << v2 + PVector::Ones(3)*2.0 << std::endl;
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

    PMatrix alpha = PMatrix::Zero(K,K);//randn(K,1);
	alpha.diagonal() = randn(K,1);
	cout << alpha << endl;
	PMatrix XE2s = randn(K,K);
	PMatrix X = randn(N,K);
	PMatrix eps = randn(P,1);
	PMatrix E1 = randn(P,K);
	PMatrix E2S = randn(K,K);
	PMatrix pheno = randn(N,P);
	PMatrix b = 0.1 + 0.5*E2S.diagonal().array();
	PMatrix a = (10.0 + 0.5*P)*(PMatrix::Ones(K, 1).array());
	
	
	// W update 
	for(int i=0; i<P; ++i){
		PMatrix prec = alpha + XE2s*eps(i);
		PMatrix cov = prec.inverse();
		// cout << alpha << endl << XE2s << endl << XE2s*eps(i) << endl << prec << endl << cov << endl;
		cout << pheno.col(i).rows() << pheno.col(i).cols() << endl << X.rows() << X.cols() << endl;
		cout << eps(i)*cov*X.transpose()*pheno.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		E1.row(i) = eps(i)*cov*X.transpose()*pheno.col(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
//		E1(i) = eps(i)*cov*X*pheno(i); //  self.E1[d,:] = S.dot(dcov[:,:],Eps[d]*S.dot(_S.E1.T,net.dataNode.E1[ :,d]))
		PMatrix outer = E1.row(i).transpose()*E1.row(i);
		E2S += (cov + outer); //  E2 = dcov + outer(self.E1[d], self.E1[d])
		
	}
}


int main (int argc, char * const argv[]) {
	//play_matrix();
	//1. simulate small dataset
	PMatrix Y = simulate_expression(100,20,5,0.01).expr;
	
	PMatrix Yvar = 0.1*PMatrix::Ones(10,100);
	//2. create object
	//test with nonexisting variance:
	Yvar = PMatrix();
	cVBFA vb(Y,Yvar, PMatrix(), 8);
	vb.setAdd_mean(false);
	vb.setNmax_iterations(50);
	vb.update();
	cout << vb.Alpha.E1 << endl << vb.X.E1.col(0);
	
}
