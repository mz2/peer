cmake_minimum_required(VERSION 2.8)
project(PEER)

#There are lots of scripts with cmake
#for finding external libraries. 
#see /usr/local/share/cmake-2.6/Modules/Find*.cmake for more examples

find_package(swig REQUIRED)
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

