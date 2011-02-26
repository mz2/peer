OPTION(BUILD_R_PACKAGE "Build the peer R package" ON)

if (BUILD_R_PACKAGE)
  find_package(SWIG REQUIRED)

  INCLUDE(${SWIG_USE_FILE})

  include_directories(./../External)
  include_directories(./../External/Eigen)
  include_directories(${PEER_SRC_DIR}/External/alglib)
  include_directories(${PEER_SRC_DIR}/include)

  set(CMAKE_SWIG_FLAGS "")
  include_directories(${R_INCLUDE_DIR})

  SET(SWIGLINKLIBS ${R_SHARED_LIB_PATH_64} ${R_SHARED_LIB_PATH} alglib peerlib)

  set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
  set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-includeall")
  set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-cpperraswarn")
  add_definitions(-DSWIG)
  swig_add_module(peer r peer.i)
  swig_link_libraries(peer ${SWIGLINKLIBS})
endif()
