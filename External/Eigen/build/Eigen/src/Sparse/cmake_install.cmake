# Install script for directory: /Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/eigen2/Eigen/src/Sparse/AmbiVector.h;/usr/local/include/eigen2/Eigen/src/Sparse/CholmodSupport.h;/usr/local/include/eigen2/Eigen/src/Sparse/CompressedStorage.h;/usr/local/include/eigen2/Eigen/src/Sparse/CoreIterators.h;/usr/local/include/eigen2/Eigen/src/Sparse/DynamicSparseMatrix.h;/usr/local/include/eigen2/Eigen/src/Sparse/MappedSparseMatrix.h;/usr/local/include/eigen2/Eigen/src/Sparse/RandomSetter.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseAssign.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseBlock.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseCwise.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseCwiseBinaryOp.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseCwiseUnaryOp.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseDiagonalProduct.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseDot.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseFlagged.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseFuzzy.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseLDLT.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseLLT.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseLU.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseMatrix.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseMatrixBase.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseProduct.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseRedux.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseTranspose.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseUtil.h;/usr/local/include/eigen2/Eigen/src/Sparse/SparseVector.h;/usr/local/include/eigen2/Eigen/src/Sparse/SuperLUSupport.h;/usr/local/include/eigen2/Eigen/src/Sparse/TaucsSupport.h;/usr/local/include/eigen2/Eigen/src/Sparse/TriangularSolver.h;/usr/local/include/eigen2/Eigen/src/Sparse/UmfPackSupport.h")
FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen/src/Sparse" TYPE FILE FILES
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/AmbiVector.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/CholmodSupport.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/CompressedStorage.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/CoreIterators.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/DynamicSparseMatrix.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/MappedSparseMatrix.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/RandomSetter.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseAssign.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseBlock.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseCwise.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseCwiseBinaryOp.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseCwiseUnaryOp.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseDiagonalProduct.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseDot.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseFlagged.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseFuzzy.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseLDLT.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseLLT.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseLU.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseMatrix.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseMatrixBase.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseProduct.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseRedux.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseTranspose.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseUtil.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SparseVector.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/SuperLUSupport.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/TaucsSupport.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/TriangularSolver.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Sparse/UmfPackSupport.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")

