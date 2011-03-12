#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <Eigen/Eigen>
#include "alglib/src/ap.h"
#include "alglib/src/specialfunctions.h"
#include "vbfa.h"
#include "sim.h"
#include "tclap/CmdLine.h"

using namespace Eigen;
using namespace std;
using namespace PEER;
using namespace TCLAP;

void play(){
	//1. simulate small dataset
	PMatrix Y = simulate_expression(400,100,5,0.01).expr;
	
	PMatrix Yvar = 0.1*PMatrix::Ones(10,100);
	//2. create object
	//test with nonexisting variance:
	Yvar = PMatrix();
	cVBFA vb(Y,Yvar, PMatrix(), 8);
	vb.setPriorEpsA(100);
	vb.setAdd_mean(false);
	vb.setNmax_iterations(1);
	vb.update();
	cout << vb.Alpha.E1 << endl << vb.X.E1.col(0);
	
	
}


PMatrix read_csv(string filename){
    ifstream ifh(filename.c_str());
	if (!ifh){ cerr << "Error opening file " << filename << endl;}
	string line;
	int n_rows = 0, n_cols = 0;
	getline(ifh, line);
	vector< vector<string> > data;
	
	while(ifh){
		vector<string> row_data;
//		row_data = tokenize(line);
		
		cout << line << endl;

		data.push_back(row_data);		
		getline(ifh, line);
		n_rows++;
	}
	
	PMatrix result = PMatrix::Zero(n_rows, n_cols);
//	for(int i = 0; i < n_rows; ++i){
//		for(int j = 0; j < n_cols; ++j){
//			result(i,j) = atof(data[i][j].c_str());
//		}
//	}
	
	return result;
}


int main (int argc, char * const argv[]) {
//	play();
	try {  
		CmdLine cmd("Probabilistic estimation of expression residuals (PEER)", ' ', "1.0");
		SwitchArg no_residuals("","no_res_out","No output of residual values", cmd, false);
		SwitchArg no_X("","no_x_out","No output of estimated factors", cmd, false);
		SwitchArg no_W("","no_w_out","No output of estimated factor weights", cmd, false);
		SwitchArg no_Alpha("","no_a_out","No output of weight precision", cmd, false);
		SwitchArg keep_mean("","keep_mean","Do not include a covariate for the mean", cmd, false);

		ValueArg<std::string> out_dir("o","out_dir","Output directory",false,"peer_out","string", cmd);
		ValueArg<std::string> expr_file("f","file","Expression data file",true,"","string", cmd);
		ValueArg<std::string> cov_file("c","cov_file","Covariate data file",false,"","string", cmd);
		ValueArg<int> n_factors("n","n_factors","Number of hidden factors",false,5,"int", cmd);
		ValueArg<int> n_iter("i","n_iter","Number of iterations",false,50,"int", cmd);
		ValueArg<float> alpha_pa("","a_pa","Alpha node prior parameter a",false,1.,"float", cmd);
		ValueArg<float> alpha_pb("","a_pb","Alpha node prior parameter b",false,1.,"float", cmd);
		ValueArg<float> eps_pa("","e_pa","Eps node prior parameter a",false,1.,"float", cmd);
		ValueArg<float> eps_pb("","e_pb","Eps node prior parameter b",false,1.,"float", cmd);
		ValueArg<float> bound_tolerance("","bound_tol","Bound tolerance",false,0.001,"float", cmd);
		ValueArg<float> var_tolerance("","var_tol","Variation tolerance",false,0.000001,"float", cmd);

		cmd.parse( argc, argv );
		std::list<Arg*> argL = cmd.getArgList();
		list<Arg*>::iterator i;
		cout << "n factors: " << n_factors.getValue() << endl << "n iterations: " << n_iter.getValue() << endl << "Output dir: " << out_dir.getValue() << endl;
	} catch (ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }

	//PMatrix test = read_csv("test.txt");
	//cout << test << endl;
	
	//PMatrix Y = read_csv(expr_file.getValue());
	//PMatrix covs = read_csv(cov_file.getValue());
	//
	return 0;
}
