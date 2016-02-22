# Reform Priorities #

This directory contains the classes for a basic analysis of the priority-order of government reform priorities. 

This treats reform as a collective decision making process (CDMP) over discrete options. Each option is a ranking of items from highest to lowest priority on the government's agenda, where the highest priority is most likely to be achieved and lowest is least likely. Thus, with N reform items, there are N! options.

## Problem Representation ##

This is an example of a CDMP over matching items into categories (aka "buckets"). Every item goes into exactly one category, and categories can hold zero, one, or several items (like a bucket of items). 

A common example of matching problems are the subset selection problems (SSP). If there are just two categories, "selected" and "rejected", then an SSP can be used to analyze problems as diverse as choosing which government projects to implement (or not), which lobbyist provisions to incorporate into a proposed law (or not), which job applicants to hire (or not), and so on.


In the case of reform items, the position of each actor is simply an ordering of the items. Thus, the categories are just the highest to lowest priorities, and the
reform items get assigned a priority. With three items, there would be three categories; with ten items there would be ten categories. It is just a confusing accident that there happen to be exactly as many categories as items in this case, and (also unique to this case), each category happens to get exactly one item.

More detail on this particular analysis and on the underlying theory is available from the KTAB [homepage](https://www.kapsarc.org/openkapsarc/kapsarc-toolkit-for-behavioral-analysis-ktab/) at [KAPSARC](https://www.kapsarc.org).

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



