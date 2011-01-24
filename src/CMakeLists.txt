include_directories(../External)
include_directories(${PEER_SRC_DIR}/External/alglib)
#link_directories(${ALGLIB_BINARY_DIR}/alglib)
add_library(peerlib STATIC bayesnet.cpp array_helper.cpp sim.cpp vbfa.cpp)
