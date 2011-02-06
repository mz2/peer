find_package(SWIG REQUIRED)


#default: enable python bindings
option(ENABLE_python "Enable Python bindings" ON)

#1. find swig
INCLUDE(${SWIG_USE_FILE})

#find python
if(NOT PTHON_EXECUTABLE)
       FIND_PACKAGE(PythonInterp)
endif()

#we got everything?
if ( (NOT EXISTS ${PYTHON_EXECUTABLE} ) )
   message("Did not find python")
   set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif()

#find python libs and includes
if (NOT PYTHON_LIBRARIES OR NOT PYTHON_INCLUDE_DIRS)
     FIND_PACKAGE(PythonLibsDist)
endif()
#find dist-packages
if (NOT PYTHON_DIST_PACKAGES)
   FIND_PACKAGE(PythonInst)
endif()

#find numpy
if (NOT NUMPY_INCLUDE_DIR)
   FIND_PACKAGE(Numpy)
endif()


if ( (NOT EXISTS ${PYTHON_LIBRARIES}))
   message("Did not find python libs")
   set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif()

if ( (NOT EXISTS ${PYTHON_INCLUDE_DIRS}))
   message("Did not find python include dirs")
   set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif()


if ( (NOT EXISTS ${NUMPY_INCLUDE_DIR}) )
   message("Did not find numpy, disabling python interface")
   set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif()


if ( NOT EXISTS ${PYTHON_INSTDIR})
   message("Did not find python install directory")
   set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif()


if(ENABLE_python)



#include_directories(${PYTHON_INCLUDE_DIR})
#include_directories(${NUMPY_INCLUDE_DIR})

include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${NUMPY_INCLUDE_DIR})


include_directories(./../External)
include_directories(./../External/Eigen)
include_directories(${PEER_SRC_DIR}/External/alglib)
include_directories(${PEER_SRC_DIR}/include)

set(CMAKE_SWIG_FLAGS "")
SET(SWIGLINKLIBS ${PYTHON_LIBRARIES})

set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-includeall")
set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-cpperraswarn")
add_definitions(-DSWIG)
swig_add_module(peer python peer.i)
swig_link_libraries(peer ${SWIGLINKLIBS} alglib peerlib)

#TODO:
#install _peer.so and peer.py into PYTHON_INSTDIR

endif(ENABLE_python)
