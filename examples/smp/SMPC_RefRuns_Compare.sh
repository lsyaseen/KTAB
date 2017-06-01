# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#!/bin/bash
#
# usage: SMPC_RefRuns_Compare 
# -------------------------------------------

rm *.out; rm *.db

# initialize
difOK=0; errOK=0; endOK=0; allOK=0

# SOE-Pol-Comp
echo "Running SOE-Pol-Comp.csv"
./smpc --logmin --csv ./doc/SOE-Pol-Comp.csv
mv ./smpc*_log.txt ./smpc_PC.out
egrep "Fractional|prob :" smpc_PC.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-SOE-Pol-Comp.txt > ref.out
PCdif=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)
PCerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ./smpc_PC.out)
PCend=$(grep -c "Finish time" ./smpc_PC.out)

# dummyData_3Dim
echo "Running dummyData_3Dim.csv"
./smpc --logmin --csv ./doc/dummyData_3Dim.csv
mv ./smpc*_log.txt ./smpc_D3.out
egrep "Fractional|prob :" smpc_D3.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-dummyData_3Dim.txt > ref.out
D3dif=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)
D3err=$(egrep -c "assert|fail|error|except|abort|dump|segment" ./smpc_D3.out)
D3end=$(grep -c "Finish time" ./smpc_D3.out)

# dummyData-6Dim
echo "Running dummyData_6Dim.csv"
./smpc --logmin --csv ./doc/dummyData_6Dim.csv
mv ./smpc*_log.txt ./smpc_D6.out
egrep "Fractional|prob :" smpc_D6.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-dummyData_6Dim.txt > ref.out
D6dif=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)
D6err=$(egrep -c "assert|fail|error|except|abort|dump|segment" ./smpc_D6.out)
D6end=$(grep -c "Finish time" ./smpc_D6.out)

# dummyData-a040
echo "Running dummyData-a040.csv"
./smpc --logmin --csv ./doc/dummyData-a040.csv
mv ./smpc*_log.txt ./smpc_40.out
egrep "Fractional|prob :" ./smpc_40.out > run.out
egrep "Fractional|prob :" ./doc/20170530_ref-dummyData-a040.txt > ref.out
D4dif=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)
D4err=$(egrep -c "assert|fail|error|except|abort|dump|segment" ./smpc_40.out)
D4end=$(grep -c "Finish time" ./smpc_40.out)

# dummyData-a080
#echo "Running dummyData-a080.csv"
#./smpc --logmin --csv ./doc/dummyData-a080.csv
#mv ./smpc*_log.txt smpc_80.out
#egrep "Fractional|prob :" ./smpc_80.out > run.out
#egrep "Fractional|prob :" ./doc/20170530_ref-dummyData-a080.txt > ref.out
#D8dif=$(diff -U 0 ./ref.out ./run.out | grep -v ^@ | wc -l)
#D8err=$(egrep -c "assert|fail|error|except|abort|dump|segment" ./smpc_80.out)
#D8end=$(grep -c "Finish time" ./smpc_80.out)

# talk
echo "========================="
echo "SOE-Pol-Comp"
echo "------------"
echo "# Differences  ${PCdif}"
echo "# Error Words  ${PCerr}"
echo "# Finish lines ${PCend}"
echo "dummyData-3Dim"
echo "------------"
echo "# Differences  ${D3dif}"
echo "# Error Words  ${D3err}"
echo "# Finish lines ${D3end}"
echo "dummyData-6Dim"
echo "------------"
echo "# Differences  ${D6dif}"
echo "# Error Words  ${D6err}"
echo "# Finish lines ${D6end}"
echo "dummyData-a040"
echo "------------"
echo "# Differences  ${D4dif}"
echo "# Error Words  ${D4err}"
echo "# Finish lines ${D4end}"
#echo "dummyData-a080"
#echo "------------"
#echo "# Differences  ${D8dif}"
#echo "# Error Words  ${D8err}"
#echo "# Finish lines ${D8end}"

# summary
difOK=$[PCdif+D3dif+D6dif+D4dif==0]
errOK=$[PCerr+D3err+D6err+D4err==0]
endOK=$[PCend+D3end+D6end+D4end==4]
allOK=$[difOK==errOK==endOK==1]
echo "------------"
echo "All OK: ${allOK}"

if [ $allOK -eq 0 ]; then
	echo "At least one test condition failed!"
    exit 42
fi
echo "========================="

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
