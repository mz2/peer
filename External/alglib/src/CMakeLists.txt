#include_directories(${EIGEN2_INCLUDE_DIR})
add_library(alglib STATIC ap.cpp fasttransforms.cpp solvers.cpp dataanalysis.cpp linalg.cpp alglibmisc.cpp integration.cpp specialfunctions.cpp diffequations.cpp optimization.cpp interpolation.cpp statistics.cpp)
