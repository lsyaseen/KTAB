# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# see http://zxq9.com/archives/795 for some date formats
#!/bin/bash
#
# usage: monte-carlo-runs.sh <n>
# -------------------------------------------

START=1
END=20

rm -f smpc*_GMT_log.txt  smpc*_GMT.db  test.db
for i in $(eval echo "{$START..$END}")
do
  # Zulu (UTC) time, with nanoseconds
  timestamp="$(date --utc +%Y-%m-%d_%H-%M-%S-%NZ)"
  name="smpc-${timestamp}_GMT"
  echo "${i}: ${name}" 
  ../smpc --seed 0 --logmin --connstr "Driver=QSQLITE;Database=${name}" --euSMP
done


echo "Problem sizes"
grep "Number of actors" smpc*_GMT_log.txt
grep "Number of SMP dimensions" smpc*_GMT_log.txt


sNum=$(grep "Start time" smpc*_GMT_log.txt | wc -l)
cNum=$(grep "Elapsed time" smpc*_GMT_log.txt | wc -l)

echo "Number of runs started   ${sNum}"
echo "Number of runs completed ${cNum}"

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

