#!/bin/bash
#
# This wipes out all the "build" directories, so you
# will have to run CMake individually on each to
# rebuild them
#
#------------------------------------------
FILES="minwater  reformpri  smp  agenda  comsel"

for d in $FILES
do
pushd $d/build; make clean; popd
pushd $d; rm -rf build ; sleep 1; popd
done
#------------------------------------------
