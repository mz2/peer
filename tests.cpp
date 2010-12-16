/*
 *  tests.cpp
 *  peer
 *
 *  Created by Matias Piipari on 18/11/2010.
 *  Copyright 2010 Matias Piipari. All rights reserved.
 *
 */


#include <limits.h>
#include "gtest/gtest.h"
#include <stdlib.h>
#include "sim.h"
#include "vbfa.h"
#include <iostream>

using namespace std;

namespace {
	
	// The fixture for testing class Foo.
	class FooTest : public ::testing::Test {
	protected:
		// You can remove any or all of the following functions if its body
		// is empty.
		
		FooTest() {
			// You can do set-up work for each test here.
		}
		
		virtual ~FooTest() {
			// You can do clean-up work that doesn't throw exceptions here.
		}
		
		// If the constructor and destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:
		
		virtual void SetUp() {
			// Code here will be called immediately after the constructor (right
			// before each test).
		}
		
		virtual void TearDown() {
			// Code here will be called immediately after each test (right
			// before the destructor).
		}
		
		// Objects declared here can be used by all tests in the test case for Foo.
	};
	
	// Tests that Foo does Xyz.
	TEST_F(FooTest, DoesXyz) {
		// Exercises the Xyz feature of Foo.
	}

	

	class cInferTest : public ::testing::Test {
		sSimulation sim;
	protected:
		cInferTest() {	sim = simulate_expression(30, 100, 8, 0.1); }				

		void TestUpdate(){
			cout << "Testing update" << endl;
			MatrixXf Y= sim.expr.array() - sim.expr.array().mean();
			MatrixXf YE2 = MatrixXf::Ones(Y.rows(), Y.cols()) + Y.cwiseProduct(Y);
			cout << "Expr mean " << sim.expr.array().abs().mean() << "\n";
			//2. create object
			
			cVBFA vb(Y,YE2, 8);
//			cout << vb.W.E1.cols() << endl << vb.Alpha.E1 << endl;
			for(int i=0; i < 10; i++){ vb.update();	}
			assert(vb.pheno.E1.array().abs().mean() > 0.4);
			assert((vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() > 0.2);
			assert((vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() < 0.21);
			cout << "Expr mean " << vb.pheno.E1.array().abs().mean() << "\nError mean " << (vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() << endl;

			vb = cVBFA(Y,YE2, 12);
//			cout << vb.W.E1.cols() << endl << vb.Alpha.E1 << endl;
			for(int i=0; i < 10; i++){ vb.update();	}
			assert(vb.pheno.E1.array().abs().mean() > 0.4);
			assert((vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() > 0.2);
			assert((vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() < 0.21);
			cout << "Expr mean " << vb.pheno.E1.array().abs().mean() << "\nError mean " << (vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() << endl;

			vb = cVBFA(Y,YE2, 1);
//			cout << vb.W.E1.cols() << endl << vb.Alpha.E1 << endl;
			for(int i=0; i < 10; i++){ vb.update();	}
			assert(vb.pheno.E1.array().abs().mean() > 0.4);
			assert((vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() > 0.33);
			assert((vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() < 0.35);
			cout << "Expr mean " << vb.pheno.E1.array().abs().mean() << "\nError mean " << (vb.pheno.E1 - vb.X.E1*(vb.W.E1.transpose())).array().abs().mean() << endl;
		}
	};
	
	// Tests that inference works on simulations
	TEST_F(cInferTest, TestUpdate) {
		TestUpdate();
	}
}  // namespace


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}