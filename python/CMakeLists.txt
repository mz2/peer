find_package(SWIG REQUIRED)

INCLUDE(${SWIG_USE_FILE})

include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${NUMPY_INCLUDE_DIR})

include_directories(./../External)
include_directories(./../External/Eigen)
include_directories(${PEER_SRC_DIR}/External/alglib)
include_directories(${PEER_SRC_DIR}/include)

set(CMAKE_SWIG_FLAGS "")
SET(SWIGLINKLIBS ${PYTHON_SHARED_LIB_PATH})

set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-includeall")
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-cpperraswarn")
add_definitions(-DSWIG)
swig_add_module(peer python peer.i)
swig_link_libraries(peer ${SWIGLINKLIBS} alglib peerlib)
