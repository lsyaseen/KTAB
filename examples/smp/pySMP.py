# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# The MIT License (MIT)
#
# Copyright (c) 2017 King Abdullah Petroleum Studies and Research Center
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
# This Python script demonstrates how to use the SMP shared library to execute the KTAB SMP
# model in Python. The only difference in running this script in Windows or Linux should
# be the name of the library: libsmpDyn.so for Linux, and smpDyn.dll for Windows. Please
# note that this demo script has *not* been tested as extensively as the rest of KTAB. Usage
# is demonstrated below after the SMP class declaration.  If an .xml file is use as data
# input, any model parameters stored in the .xml file will *not* be used.
#
# After a model run, the SMP process will "remember" results and statuses; multiple runs
# will require restarting python (technically, any SMP will access and use an existing
# smpDyn.dll process, and the OS will only kill the process when python exits).
#
# See the KTAB documentation and relevant publications for details on the model inputs.
#
# --------------------------------------------

import ctypes as c
import os
import sys


class SMP(object):
	'''
	Input Attributes
	connString - string; database connection string
	inputDataFile - string; either .csv data file with input data
	logFile - string; full path and filename for the easylogger++ configuration file
	modelParams - integer list; vector of 9 integers encoding SMP model parameters:
		Victor Model: Linear=0,Square=1,Quartic=2,Octic=3,Binary=4
		Voting Rule: Binary=0,PropBin=1,Proportional=2,PropCbc=3,Cubic=4,ASymProsp=5
		PCE Type: ConditionalPCM=0,MarkovIPCM=1,MarkovUPCM=2
		State Transition Type: DeterminsticSTM=0,StochasticSTM=1
		Big R Range: Min=0,Mid=1,Max=2
		Big R Adjust: NoRA=0,OneThirdRA=1,HalfRA=2,TwoThirdsRA=3,FullRA=4
		Third Party Commit: NoCommit=0,SemiCommit=1,FullCommit=2
		Bargain Interpolation: S1P1=0,S2P2=1,S2PMax=2
		Bargain Model: InitOnlyInterpSMPBM=0,InitRcvrInterpSMPBM=1,PWCompInterpSMPBM=2
		(see the KTAB documentation & associated publications)
	saveHist - boolean; flag which enables/disables text output of 
		by-dimension, by-turn position histories (input+'_posLog.csv')
		and by-dimension actor effective powers (input+'_effPower.csv')
	seed - integer; 64-bit unsigned int seed for the random number generator
	sqlFlags - boolean list; vector of 5 flags which enable/disable
		database logging for 5 types of data (see ../../KTAB_SMP_Tables.md):
		0 = Information Tables, 1 = Position Tables, 2 = Challenge Tables,
		3 = Bargain Resolution Tables, 4 = VectorPosition table
		the --logmin flag is equivalent to (True,False,False,False,True)

	Output Attributes
	numActors - integer; self-explanatory
	numDimensions - integer; self-explanatory
	numStates - integer; number of states through which the actors' positions evolved
	posHists - triply-nested list; complete history of actors' positions
		from the model; states are nested in dimensions are nested in actors
	scenID - string; scenario ID of the model run, returned by runModel()
	lastError - string; last SMP error (if any)

	Methods
	setDatabase() - set the database connection
	setLogger() - configure the logger
	runModel() - run the model
	getNumActors() - self-explanatory
	getNumDimensions() - self-explanatory
	getNumStates() - self-explanatory
	getPositionHistory() - self-explanatory
	delModel() - delete the model object through the SMP library and release the memory
	getLastError() - self-explanatory
	'''
	# define special types for the SMP Model Runner
	_sqlFlagsTypeC = c.c_bool*5     # array of 5 booleans
	_modelParamsTypeC = c.c_int*9   # array of 9 integers	
	# define string sizes
	_bsize = 32*16		# scenario ID 
	_bsize2 = 256*16	# last error message

	def __init__(self):
		# get the library
		if sys.platform == 'linux':
			self._smpLib = c.cdll.LoadLibrary(os.getcwd()+os.sep+'libsmpDyn.so')
		else:
			self._smpLib = c.cdll.LoadLibrary(os.getcwd()+os.sep+'smpDyn.dll')
		self._logged = False
		self._dbased = False
		self._runned = False
		self._deleted = False

	#def __repr__(self):

	#def __str__(self):

	def runModel(self,sqlFlags,inputDataFile,seed,saveHist,modelParams):
		'''
		Run the SMP model; must be executed after setDatabase and setLogger
		scenID = runModel(sqlFlags,inputDataFile,seed,saveHist,modelParams)
		'''
		if not(self._logged and self._dbased):
			print('Please execute setDatabase and setLogger first!')
			return None
		self._deleted = False
		# save user inputs
		self.sqlFlags = sqlFlags
		self.inputDataFile = inputDataFile
		self.seed = seed
		self.saveHist = saveHist
		self.modelParams = modelParams
		# define the linking function, if not already defined
		if not(hasattr(self,'_runSmpModel')):
			# SMP model; the C function declaration is
			# uint runSmpModel(char * buffer, const unsigned int buffsize,
			#   bool sqlLogFlags[5], const char* inputDataFile,
			#   unsigned int seed, unsigned int saveHistory, int modelParams[9] = 0)
			self._scenIDC = c.create_string_buffer(self._bsize)
			proto_SMP = c.CFUNCTYPE(c.c_uint,c.c_char_p,c.c_uint,self._sqlFlagsTypeC,\
				c.c_char_p,c.c_uint64,c.c_bool,self._modelParamsTypeC)
			self._runSmpModel = proto_SMP(('runSmpModel',self._smpLib))
		# prepare c-style parameters from inputs
		self._sqlFlagsC = self._sqlFlagsTypeC(*sqlFlags)
		self._inputDataFileC = bytes(inputDataFile,encoding="ascii")
		self._seedC = c.c_uint64(seed)
		self._saveHistC = c.c_bool(saveHist)
		self._modelParamsC = self._modelParamsTypeC(*modelParams)
		# run the model
		self.numStates = self._runSmpModel(self._scenIDC,self._bsize,self._sqlFlagsC,
			self._inputDataFileC,self._seedC,self._saveHistC,self._modelParamsC)
		self.scenID = self._scenIDC.value.decode('utf-8')
		# check for errors
		err = self.getLastError()
		if err != '':
			print('Error occurred: %s'%err)
		self._runned = True
		return self.scenID

	def setDatabase(self,connString):
		'''
		Set the database connection string before running the model
		setDatabase(connString)
		'''
		# define the linking function if not already defined
		if not(hasattr(self,'_dbLoginCredentials')):
			# database connection string; the C function declaration is
			# void dbLoginCredentials(const char *connStr)
			proto_LC = c.CFUNCTYPE(c.c_voidp, c.c_char_p)
			self._dbLoginCredentials = proto_LC(('dbLoginCredentials',self._smpLib))
		# setup the database connection
		self.connString = connString
		self._connStringC = bytes(connString,encoding="ascii")
		self._dbLoginCredentials(self._connStringC)
		# check for errors
		err = self.getLastError()
		if err != '':
			print('Error occurred: %s'%err)
		self._dbased = True

	def setLogger(self,logFile):
		'''
		Set the logger configuration file before running the model
		setDatabase(logFile)
		'''
		# define the linking function if not already defined
		if not(hasattr(self,'_configLogger')):
			# logger configuration; the C function declaration is
			# void configLogger(const char *cfgFile)
			proto_CL = c.CFUNCTYPE(c.c_voidp, c.c_char_p)
			self._configLogger = proto_CL(('configLogger',self._smpLib))
		# setup the logger
		self.logFile = logFile
		self._logFileC = bytes(logFile,encoding="ascii")
		self._configLogger(self._logFileC)
		# check for errors
		err = self.getLastError()
		if err != '':
			print('Error occurred: %s'%err)
		self._logged = True

	def delModel(self):
		'''
		Delete the library SMP model object and release all it's memory
		delModel()
		'''
		if not(self._deleted) and (self._runned):
			# first get the data & remove the getter functions
			self.getNumActors()
			delattr(self,'_getActorCount')
			self.getNumDimensions()
			delattr(self,'_getDimensionCount')
			self.getPositionHistory()
			delattr(self,'_getVPHistory')
			# define the linking function if not already defined
		if not(hasattr(self,'_destroySMPModel')):
			# model desctructor; the C function delaration is
			# void destroySMPModel()
			proto_DM = c.CFUNCTYPE(c.c_voidp)
			self._destroySMPModel = proto_DM(('destroySMPModel',self._smpLib))
		self._deleted = True
		self._destroySMPModel()

	def getNumStates(self):
		'''
		After running the SMP model, get the number of states through which
		the actors' positions evolved
		numStates = getNumStates()
		'''
		if not(self._runned):
			# be sure model has run
			print('Please execute runModel first!')
			return 0
		return self.numStates

	def getNumActors(self):
		'''
		After running the SMP model, get the number of actors; should be
		the same as in the data file
		numActors = getNumActors()
		'''
		if not(self._runned):
			# be sure model has run
			print('Please execute runModel first!')
			return 0
		elif self._deleted:
			# if model object deleted, just return the stored value
			return self.numActors
		# define the linking function if not already defined
		if not(hasattr(self,'_getActorCount')):
			# get number actors; the C function declaration is:
			# uint getActorCount()
			proto_NA = c.CFUNCTYPE(c.c_uint)
			self._getActorCount = proto_NA(('getActorCount',self._smpLib))
		# execute the getter
		self.numActors = self._getActorCount()
		# check for errors
		err = self.getLastError()
		if err != '':
			print('Error occurred: %s'%err)
		return self.numActors

	def getNumDimensions(self):
		'''
		After running the SMP model, get the number of dimensions; should be
		the same as in the data file
		numDimensions = getNumDimensions()
		'''
		if not(self._runned):
			# be sure model has run
			print('Please execute runModel first!')
			return 0
		elif self._deleted:
			# if model object deleted, just return the stored value
			return self.numDimensions
		# define the linking function if not already defined
		if not(hasattr(self,'_getDimensionCount')):
			# get number dimensions; the C function declaration is:
			# uint getDimensionCount()
			proto_ND = c.CFUNCTYPE(c.c_uint)
			self._getDimensionCount = proto_ND(('getDimensionCount',self._smpLib))
		# exectue the getter
		self.numDimensions = self._getDimensionCount()
		# check for errors
		err = self.getLastError()
		if err != '':
			print('Error occurred: %s'%err)
		return self.numDimensions

	def getPositionHistory(self):
		'''
		After running the SMP model, get the complete history of positions
		for all actors; histories are returned in a triply-nested list of
		states within dimensions within actors: posHists[actors][dimensions][states]
		posHists = getPositionHistory()
		'''
		if not(self._runned):
			# be sure model has run
			print('Please execute runModel first!')
			return []
		elif self._deleted:
			# if model object deleted, just return the stored value
			return self.posHists
		# define the linking function if not already defined
		if not(hasattr(self,'_getVPHistory')):
			# have to do this after running the model, to know the size of posHist
			# get the complete history of states; the C function declaration is:
			# void getVPHistory(float positions[])
			self._posHistTypeC = c.c_float * (self.numStates*self.numDimensions*self.numActors)
			proto_PS = c.CFUNCTYPE(c.c_voidp,c.POINTER(self._posHistTypeC))
			self._getVPHistory = proto_PS(('getVPHistory',self._smpLib))
		# get the position histories
		self.posHists = self._posHistTypeC()
		self._getVPHistory(self.posHists)
		# check for errors
		err = self.getLastError()
		if err != '':
			print('Error occurred: %s'%err)
		else:
			# reshape into a more useful shape; posHist is a long array in blocks of length
			# numStates, within blocks of length numDimensions, within blocks of length numActors
			tmp = [self.posHists[i*self.numStates:(i*self.numStates+self.numStates)] for i in range(self.numActors*self.numDimensions)]
			self.posHists = [tmp[i*self.numDimensions:(i*self.numDimensions+self.numDimensions)] for i in range(self.numActors)]
			# could just use posHist = np.reshape(posHists,(numActors,numDimensions,numStates)),
			# but that would kind of hide what's actually happening in the parsing
		return self.posHists
  
	def getLastError(self):
		'''
		get the last error, if any:
		lastError = getLastError()
		'''
		if self._deleted:
			# if model object deleted, just return the stored value
			return self.lastError
		# define the linking function if not already defined
		if not(hasattr(self,'_getLastError')):
			# get number actors; the C function declaration is:
			# void getLastError(char * errBuffer, const unsigned int buffsize)
			self._lastErrorC = c.create_string_buffer(self._bsize2)
			proto_LE = c.CFUNCTYPE(c.c_voidp,c.c_char_p,c.c_uint)
			self._getLastError = proto_LE(('getLastError',self._smpLib))
		# execute the getter
		self._getLastError(self._lastErrorC,self._bsize2)
		self.lastError = self._lastErrorC.value.decode('utf-8')
		return self.lastError


''' Use of the SMP object '''
if __name__ == "__main__":
	# full path to the easylogger++ configuration file	
	logFile = os.getcwd()+os.sep+'smpc-logger.conf'
	# connection string for the database
	connString = 'Driver=QSQLITE;Database=pySMPTest'
	# SMP model parameters
	# sqlFlags: vector of 5 booleans which enable/disable
	# database logging for 5 types of data (see ../../KTAB_SMP_Tables.md):
	# 0 = Information Tables, 1 = Position Tables, 2 = Challenge Tables,
	# 3 = Bargain Resolution Tables, 4 = VectorPosition table
	# the --logmin flag is equivalent to (True,False,False,False,True)
	sqlFlags = (True,False,False,False,True)
	# inputDataFile: self-explanatory
	inputDataFile = os.getcwd()+os.sep+'doc'+os.sep+'SOE-Pol-Comp.csv'
	# seed: 64-bit unsigned int seed for the random number generator
	seed = 1024
	# saveHist: boolean which enables/disables text output of 
	# by-dimension, by-turn position histories (input+'_posLog.csv')
	# and by-dimension actor effective powers (input+'_effPower.csv')
	saveHist = False
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
	modelParams = (0,0,0,2,1,1,1,1,0) # these are the defaul parameters

	# create, setup, and run the model object
	thisSMP = SMP()
	thisSMP.setLogger(logFile)
	thisSMP.setDatabase(connString)
	scenID = thisSMP.runModel(sqlFlags,inputDataFile,seed,saveHist,modelParams)
	# check for any error
	err = thisSMP.getLastError()
	# get data
	actorCnt = thisSMP.getNumActors()
	dimensionCnt = thisSMP.getNumDimensions()
	stateCnt = thisSMP.getNumStates()
	posHists = thisSMP.getPositionHistory()
	# clean up and delete the C model object
	thisSMP.delModel()
	# talk a little about the results
	print('SMP Model Scenario ID: %s:\n%d actors, %d dimensions, %d states\n'%(scenID,actorCnt,dimensionCnt,stateCnt))
	for a in range(actorCnt):
		for d in range(dimensionCnt):
		  print('Pos Hist (every 2nd) for Actor %d, Dimension %d:'%(a,d))
		  # print the alternating positions of all actors
		  print('\t[%s]'%', '.join(['%0.2f'%p for p in posHists[a][d][::2]]))
		  # print the final position
		  print('\tFinal Position %0.2f'%posHists[a][d][-1])

# ---------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# ---------------------------------------------
