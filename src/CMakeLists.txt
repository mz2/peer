include_directories(../External)
include_directories(../External/alglib)
include_directories(../External/csv_parser/include) #remove this once deemed safe
include_directories(../External/csv_parser/include/csv_parser)
include_directories(../External/tclap)

#OPTION(BUILD_PEERTOOL "Build the peertool command-line utility" ON)

if(DEBUG_PEER)
  ADD_DEFINITIONS(-DOSSOLOG_COMPILE -DOSSOLOG_STDERR -DOSSOLOG_DEBUG)
else(DEBUG_PEER)
  if(SILENT_PEER)
    #do nothing: don't include OSSOLOG_COMPILE
  else(SILENT_PEER)
    ADD_DEFINITIONS(-DOSSOLOG_COMPILE -DOSSOLOG_STDERR) 
  endif(SILENT_PEER)
endif(DEBUG_PEER)

add_library(peerlib STATIC bayesnet.cpp array_helper.cpp sim.cpp
vbfa.cpp sparsefa.cpp)

INSTALL(TARGETS peerlib
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib)

if (BUILD_PEERTOOL)
  include_directories(../External/yaml-cpp/include)
  add_executable(peertool sim.cpp peerutil_main.cpp peerutil.cpp)
  target_link_libraries(peertool peerlib alglib csv_parser yaml-cpp)

  INSTALL(TARGETS peertool
     RUNTIME DESTINATION bin)
endif (BUILD_PEERTOOL)
