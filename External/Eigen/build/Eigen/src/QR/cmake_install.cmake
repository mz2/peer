# Install script for directory: /Users/mz2/PearComp/peer/External/Eigen/Eigen/src/QR

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
   "/usr/local/include/eigen2/Eigen/src/QR/EigenSolver.h;/usr/local/include/eigen2/Eigen/src/QR/HessenbergDecomposition.h;/usr/local/include/eigen2/Eigen/src/QR/QR.h;/usr/local/include/eigen2/Eigen/src/QR/SelfAdjointEigenSolver.h;/usr/local/include/eigen2/Eigen/src/QR/Tridiagonalization.h")
FILE(INSTALL DESTINATION "/usr/local/include/eigen2/Eigen/src/QR" TYPE FILE FILES
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/QR/EigenSolver.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/QR/HessenbergDecomposition.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/QR/QR.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/QR/SelfAdjointEigenSolver.h"
    "/Users/mz2/PearComp/peer/External/Eigen/Eigen/src/QR/Tridiagonalization.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

