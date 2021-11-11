%module peer

%{
#define SWIG_FILE_WITH_INIT
#define SWIG
#include "vbfa.h"
#include "sparsefa.h"
//use namessace:
using namespace PEER;
%}


/* Get the Numeric typemaps */
%include "swig_typemaps.i"




/*
%apply (short*  IN_ARRAY1, int DIM1) {(short*  series, int size)};
%apply (int*    IN_ARRAY1, int DIM1) {(int*    series, int size)};
%apply (long*   IN_ARRAY1, int DIM1) {(long*   series, int size)};
%apply (float*  IN_ARRAY1, int DIM1) {(float*  series, int size)};
%apply (double* IN_ARRAY1, int DIM1) {(double* series, int size)};

%apply (int*    IN_ARRAY2, int DIM1, int DIM2) {(int*    matrix, int rows, int cols)};
%apply (double* IN_ARRAY2, int DIM1, int DIM2) {(double* matrix, int rows, int cols)};

%apply (int*    INPLACE_ARRAY1, int DIM1) {(int*    array,   int size)};
%apply (double* INPLACE_ARRAY1, int DIM1) {(double* array,   int size)};
*/


//typemap for matrix arguments
%apply (float64_t* IN_ARRAY2, int32_t DIM1, int32_t DIM2) {(float64_t* matrix, int32_t rows, int32_t cols)};

//typemap for return matrix arguments
%apply (float64_t** ARGOUT2, int32_t* DIM1, int32_t* DIM2) {(float64_t** matrix, int32_t* rows, int32_t* cols)};


/* Remove C Prefix */
%rename(PEER) cSPARSEFA;
%rename(VBFA) cVBFA;

/* Include the header file to be wrapped */
//%include "vbfa.h"
%include "sparsefa.i"



 
