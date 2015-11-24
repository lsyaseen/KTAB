#!/bin/bash
#
# This wipes out all the "build" directories, so you
# will have to run CMake individually on each to
# rebuild them
#
#------------------------------------------
FILES="minwater  reformpri  smp  agenda"

for d in $FILES
do
pushd $d/build; make clean; popd
pushd $d; rm -rf build ; mkdir build; sleep 1; popd
done
#------------------------------------------
