include_directories(../External)
include_directories(${PEER_SRC_DIR}/External/alglib)


add_library(peerlib STATIC bayesnet.cpp array_helper.cpp sim.cpp vbfa.cpp)

INSTALL(TARGETS peerlib
   ARCHIVE DESTINATION lib
   LIBRARY DESTINATION lib)
