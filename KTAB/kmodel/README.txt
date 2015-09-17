--------------------------------------------
Copyright KAPSARC. Open source MIT License.
--------------------------------------------
The MIT License (MIT)

Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------

This directory contains the abstract base classes for most KTAB models. Some critical methods
are implemented here, such as Probabilistic Condorcet Elections.

Several examples are included:
- smpApp: this is a simple command-line implementation of the Spatial Model of Politics
- leonApp: agents negotiate over tax/subsidy policies in a simple Leontief economy
- mtchApp: abstract problem where agents negotiate over which sweets get matched with which agents
- minApp: optimize actor-weights to approximate estimated likelihood of outcomes

-------------------------------------------------
To build the kmodel library and examples, you must
obtain and install CMake (www.cmake.org).

You must have a working C++11 development environment.
On Windows, the KTAB build has been verified with Visual Studio 12 2013 Win64
(Community edition) using the default native compilers.
On Linux, the build has been verified with KDevelop 3 and plain Unix makefiles,
using both g++ 4.8 and clang++ 3.4.

We will assume you have checked out or unpacked the KTAB
sources into /home/ktab/, so that the top of the directory
tree appears like this:
  /home/ktab/cmakemodules
  /home/ktab/kmodel
  /home/ktab/kutils
  ...

Before attempting to build kmodel, you must configure and build kutils,
as kmodel relies on that library. It is strongly advised that you run
the "demoutils" program to test the functionality.

You should start cmake-gui.

In the CMake GUI, you should answer "Where is the source code"
with the location of the CMake source file CMakeLists.txt,
not the C++ source files.

Hence, if your KTAB source is checked out into /home/ktab,
the CMakeLists.txt file would be in /home/ktab/kmodel/CMakeLists.txt

In the CMake GUI, you  should answer "Where to build the binaries"
with /home/ktab/kutils/build.

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
in the build directory, like /home/ktab/kmodel/build/kmodel.sln for Windows and Visual Studio
or /home/ktab/kmodel/build/Makefile for Linux and make.

Open the kmodel project with your IDE; it should display several targets.

"kmodel" is the library of modeling functions.

"leonApp" accepts a "--help" argument to display the help menu:

Usage: specify one or more of these options
--help            print this message
--euEcon          exp. util. of IO econ model
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
                  
"smpApp" accepts a "--help" argument to display the help menu: 

smpApp version 0.1
Usage: specify one or more of these options
--help            print this message
--euSMP           exp. util. of spatial model of politics
--csv <f>         read a scenario from CSV
--seed <n>        set a 64bit seed
                  0 means truly random
                  default: 00000000000829961074
The --euSMP option generates a random multi-dimensional dataset and simulates
bargaining until the actor positions stabilize. They are considered stable
when the RMS change is 1/100-th of the RMS change on the first turn.
As the simulation generates a great deal of text output, it is recommended that
one pipe the output to a text file with a command such as the following:
   smpApp --seed 31416 --euSMP  > tmp.txt

The "--csv" option reads in the named file and simulates bargaining until the actor positions stabilize.
An example of the expected format is in kmode/doc/dummyData_3Dim.csv
Again, it is recommended that the output be piped into a text file.

The output consists of a great deal of detailed information as the bargaining progresses.
At the end, after the phrase "Completed model run", one can find the positions of the actors over the course of the simulation.
In the following excerpt,there are two actors (number 00 and 01) with positions in a three dimensional policy space
(dimensions 00, 01, and 02). Each coordinate is on a [0,100 scale]. The first column is the initial position of the
actors (randomly generated, in this case). Thus, the position of SActor-00 at time 0 is [5.330, 78.656, 77.921].
After one step of the simulation, SActor-00 moves to [87.131, 34.184, 42.949] at time 1.


History of actor positions over time
SActor-00 , Dim-00  ,   5.330  ,  87.131  ,  72.061  ,  72.061  ,  79.614  ,  79.614  ,  72.053  ,  72.053  ,  72.053  , 
SActor-00 , Dim-01  ,  78.656  ,  34.184  ,  34.904  ,  34.904  ,  36.190  ,  36.190  ,  56.741  ,  56.741  ,  56.741  , 
SActor-00 , Dim-02  ,  77.921  ,  42.949  ,  70.253  ,  70.253  ,  71.085  ,  71.085  ,  74.612  ,  74.612  ,  74.612  , 
SActor-01 , Dim-00  ,  74.961  ,  67.822  ,  71.127  ,  71.127  ,  71.127  ,  71.129  ,  72.052  ,  72.052  ,  72.052  , 
SActor-01 , Dim-01  ,  60.367  ,  60.340  ,  57.958  ,  57.958  ,  57.958  ,  57.958  ,  56.741  ,  56.741  ,  56.741  , 
SActor-01 , Dim-02  ,  81.793  ,  80.704  ,  74.989  ,  74.989  ,  74.989  ,  74.984  ,  74.612  ,  74.612  ,  74.612  , 

Below the history of positions comes an estimate of the likelihood each actor would be the favored option,
at each step of the simulation. In the excerpt below, SActor-01 has 32% probability in the initial state,
but only 18% in the final state. By contrast, SActor-06 rises from 8.6% to 19% probability.

SActor-00 , prob  , 0.0488  , 0.1808  , 0.1948  , 0.1770  , 0.1560  , 0.1355  , 0.1696  , 0.1834  , 0.1745  , 
SActor-01 , prob  , 0.3189  , 0.1814  , 0.1731  , 0.1671  , 0.1987  , 0.1845  , 0.1804  , 0.1936  , 0.1810  , 
SActor-02 , prob  , 0.1560  , 0.1364  , 0.1348  , 0.1013  , 0.0837  , 0.1319  , 0.1365  , 0.1269  , 0.1240  , 
SActor-03 , prob  , 0.1469  , 0.1149  , 0.1099  , 0.1057  , 0.1273  , 0.1189  , 0.1235  , 0.1101  , 0.1172  , 
SActor-04 , prob  , 0.1618  , 0.1232  , 0.1106  , 0.1105  , 0.1088  , 0.1158  , 0.0932  , 0.0886  , 0.0938  , 
SActor-05 , prob  , 0.0821  , 0.0899  , 0.0721  , 0.1357  , 0.1560  , 0.1329  , 0.1269  , 0.1178  , 0.1192  , 
SActor-06 , prob  , 0.0855  , 0.1734  , 0.2047  , 0.2027  , 0.1696  , 0.1803  , 0.1700  , 0.1796  , 0.1902  , 

-------------------------------------------------

If you are interested in contributing code, ideas, or
data to KTAB, please contact ktab@kapsarc.org

--------------------------------------------
Copyright KAPSARC. Open source MIT License.
--------------------------------------------
