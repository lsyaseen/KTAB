# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#!/bin/bash
#
# usage: SMPC_RefRuns_Compare
# -------------------------------------------

rm *.out; rm *.db

# SOE-Pol-Comp
echo "SOE-Pol-Comp.csv"
./smpc --logmin --csv ./doc/SOE-Pol-Comp.csv
mv ./smpc*_log.txt ./smpc_PC.out
egrep "Fractional|prob :" smpc_PC.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-SOE-Pol-Comp.txt > ref.out
PCdiff=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)

# dummyData_3Dim
echo "dummyData_3Dim.csv"
./smpc --logmin --csv ./doc/dummyData_3Dim.csv
mv ./smpc*_log.txt ./smpc_D3.out
egrep "Fractional|prob :" smpc_D3.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-dummyData_3Dim.txt > ref.out
D3diff=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)

# dummyData-6Dim
echo "dummyData_6Dim.csv"
./smpc --logmin --csv ./doc/dummyData_6Dim.csv
mv ./smpc*_log.txt ./smpc_D6.out
egrep "Fractional|prob :" smpc_D6.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-dummyData_6Dim.txt > ref.out
D6diff=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)

# dummyData-a040
echo "dummyData-a040.csv"
./smpc --logmin --csv ./doc/dummyData-a040.csv
mv ./smpc*_log.txt ./smpc_40.out
egrep "Fractional|prob :" ./smpc_40.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-dummyData-a040.txt > ref.out
D4diff=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)

# dummyData-a080
#echo "dummyData-a080.csv"
#./smpc --logmin --csv ./doc/dummyData-a080.csv
#mv ./smpc*_log.txt smpc_80.out
#egrep "Fractional|prob :" ./smpc_80.out > run.out
#egrep "Fractional|prob :" ./doc/20170530_ref-dummyData-a080.txt > ref.out
#D8diff=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)

echo "  Number of Differences  "
echo "========================="
echo "SOE-Pol-Comp    ${PCdiff}"
echo "dummyData-3Dim  ${D3diff}"
echo "dummyData-6Dim  ${D6diff}"
echo "dummyData-a040  ${D4diff}"
#echo "dummyData-a080  ${D8diff}"

# this is the "return" value
???

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
