# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# The MIT License (MIT)
#
# Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# and associated documentation files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom
# the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# --------------------------------------------
#
# This Python scrip demonstrates how to use the SMP shared library to execute the KTAB SMP
# model in Python. The only difference in running this script in Windows or Linux should
# be the name of the library: libsmpDyn.so for Linux, and smpDyn.dll for Windows. Please
# note that this demo script has *not* been tested as extensively as the rest of KTAB.
#
# --------------------------------------------
import ctypes as c
import os

smpLib = c.cdll.LoadLibrary(os.getcwd()+os.sep+'libsmpDyn.so')#linux
#smpLib = c.cdll.LoadLibrary(os.getcwd()+os.sep+'smpDyn.dll')#windows

''' Prepare the SMP Function Prototypes '''
# logger configuration; the C function declaration is
# void configLogger(const char *cfgFile)
proto_CL = c.CFUNCTYPE(c.c_voidp, c.c_char_p)
configLogger = proto_CL(('configLogger',smpLib))

# database connection string; the C function declaration is
# void dbLoginCredentials(const char *connStr)
proto_LC = c.CFUNCTYPE(c.c_voidp, c.c_char_p)
dbLoginCredentials = proto_LC(('dbLoginCredentials',smpLib))

# SMP model; the C function declaration is
# uint runSmpModel(char * buffer, const unsigned int buffsize,
#   unsigned int sqlLogFlags[5], const char* inputDataFile,
#   unsigned int seed, unsigned int saveHistory, int modelParams[9] = 0)
sqlFlagsType = c.c_bool*5     # array of 5 booleans
modelParamsType = c.c_int*9   # array of 9 integers
proto_SMP = c.CFUNCTYPE(c.c_uint,c.c_char_p,c.c_uint,sqlFlagsType,c.c_char_p,c.c_uint64,c.c_bool,modelParamsType)
runSmpModel = proto_SMP(('runSmpModel',smpLib))

# model desctructor; the C function delaration is
# void destroySMPModel()
proto_DM = c.CFUNCTYPE(c.c_voidp)
destroySMPModel = proto_DM(('destroySMPModel',smpLib))

# get number actors & dims; the C function delclarations are:
# uint getNumAct() and uint getNumDim()
proto_NA = c.CFUNCTYPE(c.c_uint)
getNumAct = proto_NA(('getNumAct',smpLib))
proto_ND = c.CFUNCTYPE(c.c_uint)
getNumDim = proto_ND(('getNumDim',smpLib))

''' Prepare the C-type Variables '''
logFile = bytes(os.getcwd()+os.sep+'smpc-logger.conf',encoding="ascii")
connString = bytes('Driver=QSQLITE;Database=pySMPTest',encoding="ascii")

''' runSmpModel Parameters '''
bsize = 32*16
scenID = c.create_string_buffer(bsize)
# sqlFlags: vector of 5 booleans which enable/disable
# database logging for 5 types of data (see ../../KTAB_SMP_Tables.md):
# 0 = Information Tables, 1 = Position Tables, 2 = Challenge Tables,
# 3 = Bargain Resolution Tables, 4 = VectorPosition table
# the --logmin flag is equivalent to (True,False,False,False,True)
sqlFlags = sqlFlagsType(True,False,False,False,True)
# inputDataFile: self-explanatory
inputDataFile = bytes(os.getcwd()+os.sep+'doc'+os.sep+'SOE-Pol-Comp.csv',encoding="ascii")
# seed: 64-bit unsigned int seed for the random number generator
seed = c.c_uint64(1024)
# saveHist: boolean which enables/disables text output of 
# by-dimension, by-turn position histories (input+'_posLog.csv')
# and by-dimension actor effective powers (input+'_effPower.csv')
saveHist = c.c_bool(False)
# modelParams: vector of 9 integers encoding SMP model parameters:
# Victor Model: Linear=0,Square=1,Quartic=2,Octic=3,Binary=4
# Voting Rule: Binary=0,PropBin=1,Proportional=2,PropCbc=3,Cubic=4,ASymProsp=5
# PCE Type: ConditionalPCM=0,MarkovIPCM=1,MarkovUPCM=2
# State Transition Type: DeterminsticSTM=0,StochasticSTM=1
# Big R Range: Min=0,Mid=1,Max=2
# Big R Adjust: NoRA=0,OneThirdRA=1,HalfRA=2,TwoThirdsRA=3,FullRA=4
# Third Party Commit: NoCommit=0,SemiCommit=1,FullCommit=2
# Bargain Interpolation: S1P1=0,S2P2=1,S2PMax=2
# Bargain Model: InitOnlyInterpSMPBM=0,InitRcvrInterpSMPBM=1,PWCompInterpSMPBM=2
# (see the KTAB documentation & associated publications)
modelParams = modelParamsType(0,0,0,2,1,1,1,1,0) # these are the defaul parameters

''' Run the Model '''
res = configLogger(logFile)
res = dbLoginCredentials(connString)
stateCnt = runSmpModel(scenID,bsize,sqlFlags,inputDataFile,seed,saveHist,modelParams)
# might not need to get the # actors and dimensions if data was dynamically generated
actorCnt = getNumAct()
dimensionCnt = getNumDim()

# debug
actorCnt = 3
dimensionCnt = 2
stateCnt = 5

#''' Setup to Get Some Model Results '''
#class posHist(c.Structure):
#	_fields_ = [("actorCnt",c.c_uint),\
#             ("dimensionCnt",c.c_uint),\
#             ("stateCnt",c.c_uint),\
#             ('positions',((c.c_float * stateCnt) * dimensionCnt)*actorCnt)]
#
#posHists = posHist()
#
#proto_PS = c.CFUNCTYPE(c.c_voidp,c.POINTER(posHist),c.c_uint,c.c_uint,c.c_uint)
#getVPHistory = proto_PS(('getVPHistory',smpLib))
#
#getVPHistory(posHists,actorCnt,dimensionCnt,stateCnt)
#
## print out what we got back
#for a in range(posHists.actorCnt):
#  for d in range(posHists.dimensionCnt):
#    print('Pos Hist for Actor %d, Dimension %d:'%(a,d))
#    print('\t[%s]'%', '.join(['%0.2f'%p for p in posHists.positions[a][d]]))

# try a simple 3-d array
posHistType = ((c.c_float * stateCnt) * dimensionCnt)*actorCnt
proto_PS = c.CFUNCTYPE(c.c_voidp,c.POINTER(posHistType),c.c_uint,c.c_uint,c.c_uint)
getVPHistory = proto_PS(('getVPHistory',smpLib))
posHists = posHistType()
res = getVPHistory(posHists,actorCnt,dimensionCnt,stateCnt)
for a in range(actorCnt):
  for d in range(dimensionCnt):
    print('Pos Hist for Actor %d, Dimension %d:'%(a,d))
    print('\t[%s]'%', '.join(['%0.2f'%p for p in posHists[a][d]]))


# show scenario ID
scenID = scenID.value.decode('utf-8')
print('Scenario ID: %s, %d states'%(scenID,stateCnt))

# release the C model object memory
res = destroySMPModel()




















