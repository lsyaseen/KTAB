# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# see http://zxq9.com/archives/795 for some date formats
#!/bin/bash

RP="../../../../ktab-priv/refpri/data/"

FILES="reformpri-scen2-0-avrg  reformpri-scen2-2-avrg  reformpri-scen3-0-top4  reformpri-scen3-2-top4 reformpri-scen2-1-avrg  reformpri-scen2-3-avrg  reformpri-scen3-1-top4  reformpri-scen3-3-top4"

rm -f run-rp*txt  run-rp*db 

for f in $FILES
do
  # Zulu (UTC) time, with nanoseconds
  timestamp="-$(date --utc +%Y-%m-%d_%H-%M-%S-%NZ)"
  name="run-rp-$f$timestamp"

 # these two lines are only for creating the reference runs
 # timestamp=""
 # name="ref-run-rp-$f$timestamp"

  echo "$name"
  RFILE=$RP$f
  nice ../rpdemo --xml $RFILE.xml > $name.txt 
done


echo "Problem sizes"
grep "There are" run-rp*.txt | grep "reform items" | grep "actors"


sNum=$(grep "Start time" run-rp*.txt | wc -l)
cNum=$(grep "Elapsed time" run-rp*.txt | wc -l)

echo "Number of runs started   $sNum"
echo "Number of runs completed $cNum"

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

