/*
 *  array_helper.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 12/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "array_helper.h"
#include "ossolog.h"


/******************************************************************************/
//	Standard version with trigonometric calls
#define PI 3.14159265358979323846

double randn(double mu, double sigma) {
	static bool deviateAvailable=false;	//	flag
	static float storedDeviate;			//	deviate from previous calculation
	double dist, angle;
	
	//	If no deviate has been stored, the standard Box-Muller transformation is 
	//	performed, producing two independent normally-distributed random
	//	deviates.  One is stored for the next round, and one is returned.
	if (!deviateAvailable) {
		
		//	choose a pair of uniformly distributed deviates, one for the
		//	distance and one for the angle, and perform transformations
		dist=sqrt( -2.0 * log(double(rand()) / double(RAND_MAX)) );
		angle=2.0 * PI * (double(rand()) / double(RAND_MAX));
		
		//	calculate and store first deviate and set flag
		storedDeviate=dist*cos(angle);
		deviateAvailable=true;
		
		//	calcaulate return second deviate
		return dist * sin(angle) * sigma + mu;
	}
	
	//	If a deviate is available from a previous call to this function, it is
	//	returned, and the flag is set to false.
	else {
		deviateAvailable=false;
		return storedDeviate*sigma + mu;
	}
}


double sum(PMatrix& m)
{
	double rv=0;
	for (int i=0;i<m.rows();i++)
		for (int j=0;j<m.cols();j++)
		{
			rv+=m(i,j);
		}
	return rv;
			
}


PMatrix log(PMatrix& m)
{
	PMatrix rv = PMatrix(m.rows(),m.cols());
	for (int i=0;i<m.rows();i++)
		for (int j=0;j<m.cols();j++)
			rv(i,j) = log(m(i,j));
	return rv;
}

PMatrix array2matrix(const float32_t* matrix,int32_t rows,int32_t cols)
{
	//create a matrix from a double array
	PMatrix m = PMatrix(rows,cols);
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
		{
			//m(i,j) = matrix[i*cols+j];
			m(i,j) = matrix[j*rows+i];
		}
	return m;
}

PMatrix array2matrix(const float64_t* matrix,int32_t rows,int32_t cols)
{
	//create a matrix from a double array
	PMatrix m = PMatrix(rows,cols);
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
		{
			//m(i,j) = matrix[i*cols+j];
			m(i,j) = matrix[j*rows+i];
		}
	return m;
}

void matrix2array(const PMatrix m,float32_t** matrix, int32_t* rows, int32_t*cols)
{
	int size = m.rows()*m.cols();
	//allocate memory
	(*matrix) = new float32_t[size];
	//set dimensions
	(*rows) = m.rows();
	(*cols) = m.cols();
	for (int i=0;i<m.rows();i++)
		for(int j=0;j<m.cols();j++)
		{
			//(*matrix)[i*m.cols()+j] = m(i,j);
			(*matrix)[j*m.rows()+i] = m(i,j);
		}
}

void matrix2array(const PMatrix m,float64_t** matrix, int32_t* rows, int32_t*cols)
{
	int size = m.rows()*m.cols();
	//allocate memory
	(*matrix) = new float64_t[size];
	//set dimensions
	(*rows) = m.rows();
	(*cols) = m.cols();
	for (int i=0;i<m.rows();i++)
		for(int j=0;j<m.cols();j++)
		{
			//(*matrix)[i*m.cols()+j] = m(i,j);
			(*matrix)[j*m.rows()+i] = m(i,j);
		}
}



PMatrix randn(int n, int m)
/* create a randn matrix, i.e. matrix of Gaussian distributed random numbers*/
{
	PMatrix rv(n,m);
	for (int i=0; i<n; i++)
		for (int j=0; j<m; j++) {
			rv(i,j) = randn();
		}
	return rv;
}


PMatrix rand(int n,int m)
/* create a rand matrix (uniform 0..1)*/
{
	PMatrix rv(n,m);
	for (int i=0;i<n;i++)
		for(int j=0;j<m;j++)
		{
			rv(i,j) = ((double)rand())/RAND_MAX;
		}
	return rv;
}


bool isnull(const PMatrix& m)
{
	return (m.cols()==0) & (m.rows()==0);
}