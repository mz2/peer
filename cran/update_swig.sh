#!/bin/sh

#shell script to update the R interface files 
#note: this requires that a current version of PEER is build and ready to run using cmake
cp ./../build/R/peer.R ./peer/R
cp ./../build/R/peerR_wrap.cxx ./peer/src/peerR_wrap.cpp 
#clean 
rm -f ./peer/src/*.o
rm -f ./peer/src/*.so
rm -f ./peer/src-i386/*.o
rm -f ./peer/src-i386/*.so
rm -f ./peer/src-x86_64/*.o
rm -f ./peer/src-x86_64/*.so
