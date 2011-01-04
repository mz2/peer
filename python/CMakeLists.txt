find_package(PythonLibs REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(NumPy REQUIRED)
include_directories(${PythonLibs_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${NUMPY_INCLUDE_DIR})

include_directories(../External)
include_directories(${PEER_SRC_DIR}/External/alglib)
link_directories(${ALGLIB_BINARY_DIR}/alglib)

set(CMAKE_SWIG_FLAGS "")

set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-classic")
swig_add_module(peer python peer.i peer_wrap.cxx)
swig_link_libraries(peer ${PYTHON_LIBRARIES})
