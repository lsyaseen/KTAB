# Compiling Easylogging++ for KTAB #

KTAB Now uses the Easylogging++ framework, hosted on GitHub [here](https://github.com/muflihun/easyloggingpp), to log model progress.  Supported versions are currently 9.94.2 and above.  Compilation instructions under both Windows and Linux are documented here.  In both cases, you will need our [CMakeLists.txt file](.\CMakeLists.tdt)

## Linux: ##

	1. locally copy the easyloggingpp repository: git clone git@github.com:muflihun/easyloggingpp.git
	2. enter the easyloggingpp directory
	3. copy in our CMakeLists.txt, overwriting what is there
	4. execute the following commands to build the library:
- mkdir build
-  cd ./build
- cmake ..
- cmake --build .
- sudo make install; this should copy the header file to /usr/local/include, and the library to /usr/local/lib


## Windows: ##

	1. same as Linux
	2. same as Linux
	3. same as Linux
	4. run cmake - be sure that the build_static_lib option is checked
	5. build with Visual Studio
	6. manually copy the header file from easylogginpp/src to c:\local\easyloggingpp\include
	7. manually copy the library file(s) from easylogging/build/Debug(Release) to c:\local\easyloggingpp\lib\

