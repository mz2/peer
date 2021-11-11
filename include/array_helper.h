/*
 *  array_helper.h
 *  peer
 *
 *  Created by Oliver Stegle on 12/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __ARRAY_HELPER_H__
#define __ARRAY_HELPER_H__

#include "Eigen/Eigen"
#include "ap.h"

using namespace Eigen;
using alglib::randomreal;

double randn(double mu=0.0, double sigma=1.0);

//some definitions for the python interface
#define float64_t double
#define float32_t float
#define int32_t int


//standard Matrix type to use in this project
typedef Matrix<float32_t, Dynamic, Dynamic> PMatrix;
typedef Matrix<float32_t, Dynamic, 1> PVector;


//array 2 matrix and vice versa
//these functions are used for the swig interface
PMatrix array2matrix(const float32_t* matrix,int32_t rows,int32_t cols);
PMatrix array2matrix(const float64_t* matrix,int32_t rows,int32_t cols);
void matrix2array(const PMatrix m,float32_t** matrix, int32_t* rows, int32_t*cols);
void matrix2array(const PMatrix m,float64_t** matrix, int32_t* rows, int32_t*cols);


//check for null Matrix (empty)
bool isnull(const PMatrix& m);
double sum(PMatrix& m);
PMatrix log(PMatrix& m);


//create random matrix:
PMatrix randn(int n, int m);
PMatrix rand(int n,int m);

#endif
