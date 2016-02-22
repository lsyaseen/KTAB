# Reform Priorities #

This directory contains the classes for a basic analysis of the priority-order of government reform priorities. 

This treats reform as a collective decision making process (CDMP) over discrete options. Each option is a ranking of items from highest to lowest priority on the government's agenda, where the highest priority is most likely to be achieved and lowest is least likely. Thus, with N reform items, there are N! options.

## Problem Representation ##

This is an example of a CDMP over matching items into categories (aka "buckets"). Every item goes into exactly one category, and categories can hold zero, one, or several items (like a bucket of items). The software uses a library for "matching problems" which is actually more general than the particular problem of prioritizing reform proposals.

A common example of matching problems are the subset selection problems (SSP). If there are just two categories, "selected" and "rejected", then an SSP can be used to analyze problems as diverse as choosing which government projects to implement (or not), which lobbyist provisions to incorporate into a proposed law (or not), which job applicants to hire (or not), and so on.

More general matching problems can have more categories. For example, the problem of three friends dividing a bag of sweets might have three categories: "given to Alice", "given to Bob", and "given to Carol". Similarly, the problem of hiring jobs applicants might be refined to include the decision as to which project they join. With three projects, there would be four categories, such as "Project A", "Project B", "Project C", and "Not Hired".


In the case of reform items, the position of each actor is simply an ordering of the items. Thus, the categories are just the highest to lowest priorities, and the
reform items get assigned a priority. With three items, there would be three categories; with ten items there would be ten categories. It is just a confusing accident that there happen to be exactly as many categories as items in this case, and (also unique to this case), each category happens to get exactly one item.

More detail on this particular analysis and on the underlying theory is available from the KTAB [homepage](https://www.kapsarc.org/openkapsarc/kapsarc-toolkit-for-behavioral-analysis-ktab/) at [KAPSARC](https://www.kapsarc.org).




## Build Instructions ##

To build `rplib` and `rpdemo`, the following must also be built and installed first: 
- tinyxml2
- sqlite3
- kutils
- kmodel 

Under Windows, we recommend using [CMake](https://cmake.org/) to build  `tinyxml2`  and `sqlite3` from source and install them into C:/local. Under Linux, the standard package managers will install them in the standard locations, where CMake will find them.

Build `kutils` and `kmodel`  according to their README files. Each has an INSTALL build target. Under Windows, this will install them to C:/local; under Linux, this will install them to /usr/local.

One can configure and build `rpdemo` using CMake in the same way as for `kutils` and `kmodel`. 


## Software Architecture ##

To be written.

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



