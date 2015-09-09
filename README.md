# KTAB


This is the top-level README file for KTAB,
KAPSARC's open-source toolkit for assembling agent-based models of negotiation and bargaining.

KTAB is written in portable, cross-platform C++11.
It was developed and tested on 64bit Windows and Linux
platforms. Because no platform-specific code is allowed,
porting to Mac or other platforms should be feasible.

KTAB uses CMake for configuration, then your favorite IDE to build.
CMake and instructions can be obtained from www.cmake.org. The basic
procedure is to configure kutils with CMake, then build the library
and examples with your favorite IDE. Then configure kmodel with CMake
and build with your favorite IDE. Detailed build instructions can be
found in the subdirectories, starting with kutils.

Three examples are provided to illustrate the range of modelling
which can be done in the framework and to provide templates
for further work. None of them are either calibrated or verified.
For rigorous work, one could make whatever local modifications were
needed to produce a model which met one's own standards for validation. 

KTAB is not a general-purpose agent-based modelling system.
All KTAB models represent stochastic decision-making among
comparatively small numbers of stakeholder groups (roughly 5 to 50),
within the paradigm of "Probabilistic Condorcet Elections". 
PCEs are used to estimate the likelihood of different outcomes from a
collective decision making process, depending on what kinds of options
each agent has or can generate, how they value those options,
and what kinds of coalitions they can form to support or oppose each option.
Examples and details can be found in the documentation.

KTAB is released under The MIT License (Expat).
For details, see the following URLs:

- [http://opensource.org/](http://opensource.org/)
- [http://opensource.org/licenses/MIT](http://opensource.org/licenses/MIT)


----------


If you are interested in contributing code, ideas, or
data to KTAB, please contact ktab@kapsarc.org

----------

Copyright KAPSARC. Open source MIT License.

----------