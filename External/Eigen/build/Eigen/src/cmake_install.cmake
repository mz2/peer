# Install script for directory: /Users/mz2/PearComp/peer/External/Eigen/Eigen/src

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

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Core/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/LU/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/QR/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/SVD/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Cholesky/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Array/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Geometry/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/LeastSquares/cmake_install.cmake")
  INCLUDE("/Users/mz2/PearComp/peer/External/Eigen/build/Eigen/src/Sparse/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

