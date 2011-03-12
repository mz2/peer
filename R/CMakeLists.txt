#oli: I changed this to FindR which seems quite stable here
find_package(R REQUIRED)

OPTION(BUILD_R_PACKAGE "Build the peer R package" ON)

if (BUILD_R_PACKAGE)
  find_package(SWIG REQUIRED)

  INCLUDE(${SWIG_USE_FILE})

  include_directories(./../External)
  include_directories(./../External/Eigen)
  include_directories(${PEER_SRC_DIR}/External/alglib)
  include_directories(${PEER_SRC_DIR}/include)

  set(CMAKE_SWIG_FLAGS "")
  #Oli: changed to R_INCLUDEDIR
  include_directories(${R_INCLUDEDIR})

  #OLI: changed to LIBR_SO
  SET(SWIGLINKLIBS ${LIBR_SO} alglib peerlib)

  set_source_files_properties(peer.i PROPERTIES CPLUSPLUS ON)
  set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-includeall")
  set_source_files_properties(peer.i PROPERTIES SWIG_FLAGS "-cpperraswarn")
  add_definitions(-DSWIG)
  swig_add_module(peer r peer.i)
  swig_link_libraries(peer ${SWIGLINKLIBS})
endif()
