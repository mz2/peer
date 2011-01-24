find_package(NumPy REQUIRED)
find_package(SWIG REQUIRED)
find_package(PythonLibs REQUIRED)

INCLUDE(${SWIG_USE_FILE})

include_directories(${PYTHON_INCLUDE_DIRS})
#include_directories(${NUMPY_INCLUDE_DIR})
#include_directories("/usr/local/Cellar/python/2.7.1/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages/numpy/core/include")
#include_directories("/usr/local/Cellar/python/2.7.1/Frameworks/Python.framework/Versions/2.7/include/python2.7")
include_directories("/Library/Python/2.6/site-packages/numpy/core/include")
include_directories(./../External)
include_directories(./../External/Eigen)
include_directories(${PEER_SRC_DIR}/External/alglib)
include_directories(${PEER_SRC_DIR}/include)
#link_directories(${ALGLIB_BINARY_DIR})
#link_directories(${PEERLIB_BINARY_DIR})


set(CMAKE_SWIG_FLAGS "")
SET(SWIGLINKLIBS ${PYTHON_LIBRARIES})

set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-includeall")
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-cpperraswarn")
add_definitions(-DSWIG)
swig_add_module(peerpython python peer.i)
swig_link_libraries(peerpython ${SWIGLINKLIBS} alglib peerlib)
