# Install script for directory: /Users/mz2/PearComp/peer/External/Eigen/Eigen

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
   "/usr/local/include/eigen2/Eigen/Core;/usr/local/include/eigen2/Eigen/LU;/usr/local/include/eigen2/Eigen/Cholesky;/usr/local/include/eigen2/Eigen/QR;/usr/local/include/eigen2/Eigen/Geometry;/usr/local/include/eigen2/Eigen/Sparse;/usr/local/include/eigen2/Eigen/Array;/usr/local/include/eigen2/Eigen/SVD;/usr/local/include/eigen2/Eigen/LeastSquares;/usr/local/include/eigen2/Eigen/QtAlignedMalloc;/usr/local/include/eigen2/Eigen/StdVector;/usr/local/include/eigen2/Eigen/NewStdVector;/usr/local/include/eigen2/Eigen/Eigen;/usr/local/include/eigen2/Eigen/Dense")
FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen" TYPE FILE FILES
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Core"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/LU"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Cholesky"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/QR"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Geometry"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Sparse"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Array"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/SVD"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/LeastSquares"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/QtAlignedMalloc"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/StdVector"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/NewStdVector"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Eigen"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/Dense"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

