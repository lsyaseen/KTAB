#KModel

This directory contains the abstract base classes for most KTAB models. Some critical methods
are implemented here, such as Probabilistic Condorcet Elections.

Several examples are included:
- smpApp: this is a simple command-line implementation of the Spatial Model of Politics
- leonApp: agents negotiate over tax/subsidy policies in a simple Leontief economy
- mtchApp: abstract problem where agents negotiate over which sweets get matched with which agents
- minApp: optimize actor-weights to approximate estimated likelihood of outcomes

## Build Instructions ##

To build the kmodel library and examples, you must
obtain and install [CMake](www.cmake.org).

You must have a working C++11 development environment.
On Windows, the KTAB build has been verified with Visual Studio 12 2013 Win64
(Community edition) using the default native compilers.
On Linux, the build has been verified with KDevelop 3 and plain Unix makefiles,
using both g++ 4.8 and clang++ 3.4.

The kmodel library and examples use [SQLite3](https://www.sqlite.org/). Under Linux, simply install the sqlite3 package,
and everything will be where CMake expects it.

Under Windows, the simplest and most reliable way is to compile it and put the library and header into C:/local/sqlite. In fact, the CMake files of KTAB expect all the libraries to be in C:/local for Windows: fltk, kmodel, sqlite, tinyxml, kutil. Under Linux, the kutils, kmodel files will be installed to /usr/local. There is no need to modify your PATH under either Linux or Windows.
 
First, download the source code "amalgamation" from the SQLite3
 [download](https://www.sqlite.org/download.html) page. The amalgamation contains the complete source code as a single
"sqlite3.c" file, as well as a simple shell demo (shell.c). Use Visual Studio to make a solution which contains two projects. The first project should build a static library, sqlite3.lib, 
using only sqlite3.c. The second project should build a console program using the library and shell.c in order to test/demo functionality.

Then, copy sqlite3.h, sqlite3ext.h and sqlite3.lib into 
C:/local/sqlite. This is where CMake expects to find them.

We will assume you have checked out or unpacked the KTAB
sources into /home/KTAB/, so that the top of the directory
tree appears like this:

	/home/KTAB/examples
	/home/KTAB/KTAB/cmakemodules
	/home/KTAB/KTAB/kmodel
	/home/KTAB/KTAB/kutils
	...
  


Before attempting to build kmodel, you must configure and build kutils, as kmodel relies on that library. Build instructions are in the kutils [README](../kutils/README.md). It is strongly advised that you run the "demoutils" program to test the functionality. When you build the INSTALL target, it will copy the kutils library and header to where CMake expects to find them. Under Windows, this will be C:/local/; under Linux, it will be /usr/local.

After building and installing kutils, the first step in building kmodel is to start  CMake. We recommend using the GUI version, not the command-line version.

In the CMake GUI, you should answer "Where is the source code"
with the location of the CMake source file CMakeLists.txt,
not the C++ source files.

Hence, if your KTAB source is checked out into /home/KTAB,
the CMakeLists.txt file would be in /home/KTAB/KTAB/kmodel/CMakeLists.txt

In the CMake GUI, you  should answer "Where to build the binaries"
with /home/KTAB/KTAB/kmodel/build.

Hit the "Configure" button. The first time you configure the system,
CMake should immediately ask if it should create the build directory;
answer "Yes".

The first time you configure the system, CMake should ask what generator 
to use for this project and offer a drop-down list of the IDE's for your platform.
These are IDE's which CMake can support, which are not necessarily those installed 
on your system. Select one of the installed options (you will probably need to 
use the same generator for kutils)

Hit "Finish".

CMake should display some notifications in the lower window, but there should be no errors.
There should be a message saying that it located the kutils header and library which you
created earlier. There should be a corresponding entry KUTILS in the upper window that shows
the include, library, and prefix. The upper window may have some items marked in red;
hitting "Configure" again should resolve them.

If errors occur, you will probably need to consult the CMake documentation to diagnose the problem.

After all problems are resolved, there are no error messages, and no items in the upper window
are red, hit "Generate" to generate the build files for your chosen IDE. They will appear
in the build directory, like /home/KTAB/kmodel/build/kmodel.sln for Windows and Visual Studio
or /home/KTAB/KTAB/kmodel/build/Makefile for Linux and make.

## Library and Examples ##

Open the kmodel project with your IDE; it should display several targets.

"kmodel" is the library of modeling functions.

"leonApp" accepts a "--help" argument to display the help menu:

Usage: specify one or more of these options
--help            print this message
--euEcon          exp. util. of IO econ model
--sql             demo SQLite
--seed <n>        set a 64bit seed
                  0 means truly random
                  default: 00000000003924059859
                  
The "--euEcon" option generates a simple random I/O model and demonstrates
the optimization of a policy position from one actor's point of view.

"mtchApp" accepts a "--help" argument to display the help menu:

Usage: specify one or more of these options
--help            print this message
--dos             division of sweets
--maxSup          max support in division of sweets
--mtchSUSN        SUSN bargaining over division of sweets
--seed <n>        set a 64bit seed
                  0 means truly random
                  default: 00000000003868993640

The --maxSup option finds the option with maximum support.
The --mtchSUSN option simulates simple dynamic bargaining, where each actor
crafts their position (at each turn) to maximize the expected utility to themselves,
given the level of support or opposition their position will receive.
The bargaining continues until no actor changes their position, i.e. until
they reach Nash Equilibrium.



## Contributing and License Information ##



If you are interested in contributing code, ideas, or
data to KTAB, please contact ktab@kapsarc.org


KTAB is released under The MIT License (Expat).
For details, see the following URLs:

- [http://opensource.org/](http://opensource.org/)
- [http://opensource.org/licenses/MIT](http://opensource.org/licenses/MIT)
 

----------

Copyright KAPSARC. Open source MIT License.

----------

