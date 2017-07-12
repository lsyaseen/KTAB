# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#!/bin/bash
#
# usage: KTAB_Test_Apps
# -------------------------------------------

rm *LOG.out

# initialize
difOK=0; errOK=0; endOK=0; allOK=0; testcnt=0

# demoutils - DIFFERENT THOUGH SAME SEED;  ONLY ON TRAVIS???
#echo "Running demoutils"
#testcnt=$[testcnt+1]
#cd ./KTAB/kutils
#./demoutils --vimcp 2 --vhc 3
#mv ./kutils*_log.txt ../../demoutilsLOG.out
#egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-demoutils.txt > ../../ref.out
#egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../demoutilsLOG.out > ../../run.out
#DUdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
#DUerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../demoutilsLOG.out)
#DUend=$(grep -c "Finish time" ../../demoutilsLOG.out)

# demomodel
echo "Running demomodel"
testcnt=$[testcnt+1]
cd ./KTAB/kmodel
./demomodel --emod --sql --pce
mv ./kmodel*_log.txt ../../demomodelLOG.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-demomodel.txt > ../../ref.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../demomodelLOG.out > ../../run.out
DMdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
DMerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../demomodelLOG.out)
DMend=$(grep -c "Finish time" ../../demomodelLOG.out)

# leonapp
echo "Running leonApp"
testcnt=$[testcnt+1]
./leonApp --euEcon
mv ./leon*_log.txt ../../leonAppLOG.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-leonApp.txt > ../../ref.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../leonAppLOG.out > ../../run.out
LAdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
LAerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../leonAppLOG.out)
LAend=$(grep -c "Finish time" ../../leonAppLOG.out)

# mtchApp - DIFFERENT THOUGH SAME SEED???
#echo "Running mtchApp"
#testcnt=$[testcnt+1]
#./mtchApp --mtchSUSN --maxSup
#mv ./mtch*_log.txt ../../mtchAppLOG.out
#egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-mtchApp.txt > ../../ref.out
#egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../mtchAppLOG.out > ../../run.out
#MAdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
#MAerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../mtchAppLOG.out)
#MAend=$(grep -c "Finish time" ../../mtchAppLOG.out)

# agdemo
echo "Running agdemo"
testcnt=$[testcnt+1]
cd ../../examples/agenda
./agdemo
mv ./agenda*_log.txt ../../agdemoLOG.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-agdemo.txt > ../../ref.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../agdemoLOG.out > ../../run.out
ADdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
ADerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../agdemoLOG.out)
ADend=$(grep -c "Finish time" ../../agdemoLOG.out)

# rpdemo
echo "Running rpdemo"
testcnt=$[testcnt+1]
cd ../reformpri
./rpdemo --si
mv ./rpdemo*_log.txt ../../rpdemoLOG.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-rpdemo.txt > ../../ref.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../rpdemoLOG.out > ../../run.out
RDdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
RDerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../rpdemoLOG.out)
RDend=$(grep -c "Finish time" ../../rpdemoLOG.out)

# minwater - DIFFERENT THOUGH SAME SEED???
#echo "Running minwater"
#testcnt=$[testcnt+1]
#cd ../minwater
#./mwdemo --waterMin
#mv ./minwater*_log.txt ../../mwdemoLOG.out
#egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-mwdemo.txt > ../../ref.out
#egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../mwdemoLOG.out > ../../run.out
#MWdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
#MWerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../mwdemoLOG.out)
#MWend=$(grep -c "Finish time" ../../mwdemoLOG.out)

# comsel
echo "Running csg"
testcnt=$[testcnt+1]
cd ../comsel
./csg --si
mv ./comsel*_log.txt ../../csgLOG.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-csg.txt > ../../ref.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../csgLOG.out > ../../run.out
CSdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
CSerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../csgLOG.out)
CSend=$(grep -c "Finish time" ../../csgLOG.out)

# pmatrix
echo "Running pmdemo"
testcnt=$[testcnt+1]
cd ../pmatrix
./pmdemo --pmm
mv ./pmatrix*_log.txt ../../pmdemoLOG.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ./20170530_ref-pmdemo.txt > ../../ref.out
egrep "Start time|Finish time|Elapsed time|Scenario" -v ../../pmdemoLOG.out > ../../run.out
PDdif=$(diff -U 0 ../../ref.out ../../run.out | grep -v ^@ | wc -l)
PDerr=$(egrep -c "assert|fail|error|except|abort|dump|segment" ../../pmdemoLOG.out)
PDend=$(grep -c "Finish time" ../../pmdemoLOG.out)

# talk
echo "========================="
echo "demoutils"
echo "------------"
echo "skipped due to stochasticity"
#echo "# Differences  ${DUdif}"
#echo "# Error Words  ${DUerr}"
#echo "# Finish lines ${DUend}"
echo "------------"
echo "demomodel"
echo "------------"
echo "# Differences  ${DMdif}"
echo "# Error Words  ${DMerr}"
echo "# Finish lines ${DMend}"
echo "------------"
echo "leonApp"
echo "------------"
echo "# Differences  ${LAdif}"
echo "# Error Words  ${LAerr}"
echo "# Finish lines ${LAend}"
echo "------------"
echo "mtchApp"
echo "------------"
echo "skipped due to stochasticity"
#echo "# Differences  ${MAdif}"
#echo "# Error Words  ${MAerr}"
#echo "# Finish lines ${MAend}"
echo "------------"
echo "agdemo"
echo "------------"
echo "# Differences  ${ADdif}"
echo "# Error Words  ${ADerr}"
echo "# Finish lines ${ADend}"
echo "------------"
echo "rpdemo"
echo "------------"
echo "# Differences  ${RDdif}"
echo "# Error Words  ${RDerr}"
echo "# Finish lines ${RDend}"
echo "------------"
echo "minwater"
echo "------------"
echo "skipped due to stochasticity"
#echo "# Differences  ${MWdif}"
#echo "# Error Words  ${MWerr}"
#echo "# Finish lines ${MWend}"
echo "------------"
echo "csg"
echo "------------"
echo "# Differences  ${CSdif}"
echo "# Error Words  ${CSerr}"
echo "# Finish lines ${CSend}"
echo "------------"
echo "pmdemo"
echo "------------"
echo "# Differences  ${PDdif}"
echo "# Error Words  ${PDerr}"
echo "# Finish lines ${PDend}"
echo "------------"

# summary
difOK=$[DMdif+LAdif+ADdif+RDdif+CSdif+PDdif==0]
errOK=$[DMerr+LAerr+ADerr+RDerr+CSerr+PDerr==0]
endOK=$[DMend+LAend+ADend+RDend+CSend+PDend==testcnt]
allOK=$[difOK==errOK==endOK==1]
echo "------------"

if [ $allOK -eq 0 ]; then
	echo "At least one test condition failed!"
    exit 42
else
	echo "All test conditions passed!"
fi
echo "========================="

# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# Copyright KAPSARC. MIT Open Source License.
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
