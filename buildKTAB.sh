#!/bin/bash
#
# You must generate a 'build' directory for each subproject
# before using this script. CMake is the recommended was to do so.
#
#------------------------------------------
FILES="kutils  kmodel  power-dispatch  refpri"

for d in $FILES
do
pushd $d/build ; nice make -k -j3 all ; sleep 1; popd
done
#------------------------------------------
