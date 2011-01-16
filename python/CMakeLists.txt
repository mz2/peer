find_package(PythonLibs REQUIRED)
find_package(NumPy REQUIRED)
include_directories(${PythonLibs_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${NUMPY_INCLUDE_DIR})

include_directories(../External)
include_directories(${PEER_SRC_DIR}/External/alglib)
include_directories(${PEER_SRC_DIR}/include)
link_directories(${ALGLIB_BINARY_DIR}/alglib)
link_directories(${PEERLIB_BINARY_DIR}/peerlib)

set(CMAKE_SWIG_FLAGS "")

set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
#set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-classic")
add_definitions(-DSWIG)
swig_add_module(peer python peer.i)
swig_link_libraries(peer ${PYTHON_LIBRARIES})
