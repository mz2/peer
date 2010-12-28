/*
 *  array_helper.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 12/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "array_helper.h"



MatrixXf array2matrix(const float64_t* matrix,int32_t rows,int32_t cols)
{
	//create a matrix from a double array
	MatrixXf m = MatrixXf(rows,cols);
	printf("starting copying");
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++)
		{
			m(i,j) = matrix[i*cols+j];
		}
	return m;
}

void matrix2array(const MatrixXf m,float64_t** matrix, int32_t* rows, int32_t*cols)
{
	int size = m.rows()*m.cols();
	(*matrix) = new double[size];
	for (int i=0;i<m.rows();i++)
		for(int j=0;j<m.cols();j++)
		{
			(*matrix)[i*m.cols()+j] = m(i,j);
		}
}



bool isnull(const MatrixXf m)
{
	return (m.cols()==0) & (m.rows()==0);
}