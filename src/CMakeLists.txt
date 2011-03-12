include_directories(../External)
include_directories(../External/alglib)
include_directories(../External/csv_parser/include)
include_directories(../External/csv_parser/include/csv_parser)

add_library(peerlib STATIC bayesnet.cpp array_helper.cpp sim.cpp vbfa.cpp)

#INSTALL(TARGETS peerlib
#   ARCHIVE DESTINATION lib
#   LIBRARY DESTINATION lib)

add_executable(peerutil sim.cpp vbfa.cpp main.cpp)
target_link_libraries(peerutil peerlib alglib csv_parser)

INSTALL(TARGETS peerutil
   RUNTIME DESTINATION bin)
