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


double sum(PMatrix m)
{
	double rv=0;
	for (int i=0;i<m.rows();i++)
		for (int j=0;j<m.cols();j++)
		{
			rv+=m(i,j);
		}
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
/* create a randn matrix */
{
	PMatrix rv(n,m);
	for (int i=0; i<n; i++)
		for (int j=0; j<m; j++) {
			rv(i,j) = randomreal();
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
			rv(i,j) = rand();
		}
	return rv;
}

bool isnull(const PMatrix m)
{
	return (m.cols()==0) & (m.rows()==0);
}