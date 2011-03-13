cmake_minimum_required(VERSION 2.8)
project(PEER)


## Documentation for final package
# manual installation paths

##PYTHON##
#
# Path to python executable. If specified peer is built against this
# python version, otherwise "which python" is used for auto-detection
#SET(PYTHON_EXECUTABLE "")
#Python include directory 
#normally there is no need to specify them;instead these are
#auto-determined from the location of the binary.
#SET(PYTHON_INCLUDE_DIRS "")
#Python install directory
#If specified, the peer module will be installed here. Otherwise the
#default site-packages location of the respective python installation
#is used. 
#SET(PYTHON_INSTDIR "")
#Numpy header fiels.
#Usually there is no need to specify them as they are automatically
#determined from the python binary.
#Note: it is vital that peer is built against the exact same python
#installation that you are aiming to use.
#SET(NUMPY_INCLUDE_DIR "")
#
##/PYTHON END##


# R package build configuration
# R header and shared library paths
#set(R_INCLUDE_DIR "/usr/local/Cellar/r/2.12.0/R.framework/Resources/include")
#set(R_SHARED_LIB_PATH "/usr/local/Cellar/r/2.12.0/R.framework/Resources/lib/libR.dylib")
##/R END

#Matias version
#set(PYTHON_EXECUTABLE "/usr/bin/python")
#set(PYTHON_INCLUDE_DIRS "/usr/include/python2.6")
#set(NUMPY_INCLUDE_DIR "/System/Library/Frameworks/Python.framework/Versions/2.6/Extras/lib/python/numpy/core/include")
#set(PYTHON_SHARED_LIB_PATH "/usr/lib/libpython2.6.dylib")

set(R_INCLUDE_DIR "/Library/Frameworks/R.framework/Versions/2.12/Resources/include")
set(R_SHARED_LIB_PATH "/Library/Frameworks/R.framework/Versions/2.12/Resources/lib/i386/libR.dylib")
set(R_SHARED_LIB_PATH_64 "/Library/Frameworks/R.framework/Versions/2.12/Resources/lib/x86_64/libR.dylib")

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#There are lots of scripts with cmake
#for finding external libraries. 
#see /usr/local/share/cmake-2.6/Modules/Find*.cmake for more examples

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_SWIG_FLAGS "")

set(CMAKE_MODULE_PATH "${PEER_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include_directories(./include)

add_definitions(-g -Wextra -D_BSD_SOURCE)

#fPic while compiling on:
set(CMAKE_CXX_FLAGS "-fPIC")

include_directories(./External/alglib/src)

add_subdirectory(./External/alglib/src)
add_subdirectory(./External/csv_parser)
add_subdirectory(./src)
add_subdirectory(./python)
add_subdirectory(./External/yaml-cpp)

#add_subdirectory(./R)

#include_directories(
#    .
#    ..
#    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMAKE_SOURCE_DIR}
#    ${CMAKE_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/bin/
#    ./src
#   )

