#!/bin/sh

#shell script to update the R interface files 
#note: this requires that a current version of PEER is build and ready to run using cmake
cp ./../build/R/peer.R ./peer/R
cp ./../build/R/peerR_wrap.cxx ./peer/src/peerR_wrap.cpp 
#clean 
rm ./peer/src/*.o
rm ./peer/src/*.so
