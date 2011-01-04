# Install script for directory: /Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/eigen2/Eigen/src/Core/Assign.h;/usr/local/include/eigen2/Eigen/src/Core/Block.h;/usr/local/include/eigen2/Eigen/src/Core/CacheFriendlyProduct.h;/usr/local/include/eigen2/Eigen/src/Core/Coeffs.h;/usr/local/include/eigen2/Eigen/src/Core/CommaInitializer.h;/usr/local/include/eigen2/Eigen/src/Core/Cwise.h;/usr/local/include/eigen2/Eigen/src/Core/CwiseBinaryOp.h;/usr/local/include/eigen2/Eigen/src/Core/CwiseNullaryOp.h;/usr/local/include/eigen2/Eigen/src/Core/CwiseUnaryOp.h;/usr/local/include/eigen2/Eigen/src/Core/DiagonalCoeffs.h;/usr/local/include/eigen2/Eigen/src/Core/DiagonalMatrix.h;/usr/local/include/eigen2/Eigen/src/Core/DiagonalProduct.h;/usr/local/include/eigen2/Eigen/src/Core/Dot.h;/usr/local/include/eigen2/Eigen/src/Core/Flagged.h;/usr/local/include/eigen2/Eigen/src/Core/Functors.h;/usr/local/include/eigen2/Eigen/src/Core/Fuzzy.h;/usr/local/include/eigen2/Eigen/src/Core/GenericPacketMath.h;/usr/local/include/eigen2/Eigen/src/Core/IO.h;/usr/local/include/eigen2/Eigen/src/Core/Map.h;/usr/local/include/eigen2/Eigen/src/Core/MapBase.h;/usr/local/include/eigen2/Eigen/src/Core/MathFunctions.h;/usr/local/include/eigen2/Eigen/src/Core/Matrix.h;/usr/local/include/eigen2/Eigen/src/Core/MatrixBase.h;/usr/local/include/eigen2/Eigen/src/Core/MatrixStorage.h;/usr/local/include/eigen2/Eigen/src/Core/Minor.h;/usr/local/include/eigen2/Eigen/src/Core/NestByValue.h;/usr/local/include/eigen2/Eigen/src/Core/NumTraits.h;/usr/local/include/eigen2/Eigen/src/Core/Part.h;/usr/local/include/eigen2/Eigen/src/Core/Product.h;/usr/local/include/eigen2/Eigen/src/Core/Redux.h;/usr/local/include/eigen2/Eigen/src/Core/SolveTriangular.h;/usr/local/include/eigen2/Eigen/src/Core/Sum.h;/usr/local/include/eigen2/Eigen/src/Core/Swap.h;/usr/local/include/eigen2/Eigen/src/Core/Transpose.h;/usr/local/include/eigen2/Eigen/src/Core/Visitor.h")
FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen/src/Core" TYPE FILE FILES
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Assign.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Block.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/CacheFriendlyProduct.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Coeffs.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/CommaInitializer.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Cwise.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/CwiseBinaryOp.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/CwiseNullaryOp.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/CwiseUnaryOp.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/DiagonalCoeffs.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/DiagonalMatrix.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/DiagonalProduct.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Dot.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Flagged.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Functors.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Fuzzy.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/GenericPacketMath.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/IO.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Map.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/MapBase.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/MathFunctions.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Matrix.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/MatrixBase.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/MatrixStorage.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Minor.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/NestByValue.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/NumTraits.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Part.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Product.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Redux.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/SolveTriangular.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Sum.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Swap.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Transpose.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/Core/Visitor.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Core/util/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Core/arch/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

