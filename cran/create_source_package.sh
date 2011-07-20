#!/bin/sh

#shell script to update the R interface files 
#note: this requires that a current version of PEER is build and ready to run using cmake
rm ./peer/src/*.o
rm ./peer/src/*.so

#create .tar.gz with symlinks dereferenced
tar cfzh ./peer_source.tgz ./peer
