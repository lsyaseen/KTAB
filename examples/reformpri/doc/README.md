# Reform Priorities #

This directory contains some basic documentation:

- XML example data
- XSD schema


## XML Data Format ##

The most precise specification of the format is the `reformpri.xsd` schema. If you are learning the format or developing an XML data file, we suggest using an tool which can check validity with one click, such as  [XML Copy Editor](http://xml-copy-editor.sourceforge.net/). Like many XML editors, it will auto-fill tags as you go, check them for balance, and various other tasks to make writing XML simple and quick.

The basic elements of the data file are the following:

- Scenario name and description
- govBudget: a positive decimal between 0 and 100.
- outOfBudget factor: a positive decimal between 0 and 1.
- orderFactor:  a positive decimal between 0 and 1.
- Items: a list of name and cost pairs.
	- Name: a short, 2-5 character label
	- Cost: a positive decimal between 0 and 100.
- Categories: a list of names
- Actors
	- Name: a short, 2-5 character, e.g. "Pres"
	- Description: a longer description, e.g. "President of the  Federation"
	- Capability: a positive number between 0 and 100.
	- ItemValues: a list of values for each item, in the same order as the items themselves. Each item value is a decimal between 0 and 100 (possibly zero).
	 

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



