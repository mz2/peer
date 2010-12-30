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

#include <Eigen/Eigen>
#include "alglib/src/ap.h"
using namespace Eigen;
using alglib::randomreal;


//some definitions for the python interface
#define float64_t double
#define float32_t float
#define int32_t int

//standard Matrix type to use in this project
typedef Matrix<float32_t, Dynamic, Dynamic> PMatrix;
typedef Matrix<float32_t, Dynamic, 1> PVector;


//array 2 matrix and vice versa
PMatrix array2matrix(const float32_t* matrix,int32_t rows,int32_t cols);
PMatrix array2matrix(const float64_t* matrix,int32_t rows,int32_t cols);
void matrix2array(const PMatrix m,float32_t** matrix, int32_t* rows, int32_t*cols);
void matrix2array(const PMatrix m,float64_t** matrix, int32_t* rows, int32_t*cols);


//check for null Matrix (empty)
bool isnull(const PMatrix m);

//create random matrix:
PMatrix randn(int n, int m);

#endif