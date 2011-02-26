cmake_minimum_required(VERSION 2.8)
project(PEER)


## Documentation for final package
# manual installation paths

##PYTHON
#SET(PYTHON_EXECUTABLE "")
#python library (so file)
#SET(PYTHON_LIBRARIES "")
#include directory 
#SET(PYTHON_INCLUDE_DIRS "")
#installation path of python (site-packages)
#SET(PYTHON_INSTDIR "")
#numpy includes
#SET(NUMPY_INCLUDE_DIR "")
##/Python END

# R package build configuration
# R header and shared library paths
#set(R_INCLUDE_DIR "/usr/local/Cellar/r/2.12.0/R.framework/Resources/include")
#set(R_SHARED_LIB_PATH "/usr/local/Cellar/r/2.12.0/R.framework/Resources/lib/libR.dylib")
##/R END

#OLIs version
#SET(PYTHON_LIBRARIES "/opt/local/Library/Frameworks/Python.framework/Versions/2.6/Python")
#SET(PYTHON_INCLUDE_DIRS "/opt/local/Library/Frameworks/Python.framework/Versions/2.6/include/python2.6")
#SET(R_SHARED_LIB_PATH  "/opt/local/lib/R/lib/x86_64/libR.dylib")
#The SET(R_INCLUDE_DIR "/opt/local/lib/R/include")

#Matias version
set(PYTHON_INCLUDE_DIRS "/usr/include/python2.6")
set(NUMPY_INCLUDE_DIR "/System/Library/Frameworks/Python.framework/Versions/2.6/Extras/lib/python/numpy/core/include")
set(PYTHON_SHARED_LIB_PATH "/usr/lib/libpython2.6.dylib")

set(R_INCLUDE_DIR "/Library/Frameworks/R.framework/Versions/2.12/Resources/include")
set(R_SHARED_LIB_PATH "/Library/Frameworks/R.framework/Versions/2.12/Resources/lib/i386/libR.dylib")
set(R_SHARED_LIB_PATH_64 "/Library/Frameworks/R.framework/Versions/2.12/Resources/lib/x86_64/libR.dylib")

#set(R_INCLUDE_DIR "/usr/local/Cellar/r/2.12.0/R.framework/Resources/include")
#set(R_SHARED_LIB_PATH "/usr/local/Cellar/r/2.12.0/R.framework/Resources/lib/libR.dylib")

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

include_directories(./External/alglib/src)

add_subdirectory(./External/alglib/src)
add_subdirectory(./src)
add_subdirectory(./python)
add_subdirectory(./R)

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

