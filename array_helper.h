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

#define float64_t double
#define int32_t int

using namespace Eigen;
using Eigen::MatrixXf;

//array 2 matrix and vice versa
MatrixXf array2matrix(const float64_t* matrix,int32_t rows,int32_t cols);
void matrix2array(const MatrixXf m,float64_t** matrix, int32_t* rows, int32_t*cols);

//check for null Matrix (empty)
bool isnull(const MatrixXf m);

#endif