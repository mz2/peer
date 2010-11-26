/*
 *  model.h
 *  peer
 *
 *  Created by Leopold Parts on 03/11/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 


double logdet(MatrixXf m);

class cBayesianFA {
	int a;
};


/** Factor activation node /
class cNodeS : public cNode {
public:
    MatrixXf E2S; // sum of second moments
	int N; // number of individuals
	

	cNodeS(cBayesNet *net, MatrixXf *E1_val):cNode();
	double entropy();	
	double calcBound(cBayesNet *net);
	void update(cBayesNet *net);
}; */
