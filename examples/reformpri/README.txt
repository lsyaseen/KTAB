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

This directory contains the classes for a basic analysis of the priority-order of government
reform priorities. This treats reform as a collective decision making process (CDMP) over
discrete options. Each option is a ranking of items from highest to lowest priority on the government's
agenda, where the highest priority is most likely to be achieved and lowest is least likely.
Thus, with N reform items, there are N! options.

This is an example of a CDMP over matching items into categories (aka "buckets").
Every item goes into exactly one category, and categories can hold zero, one, or several items
(like a bucket of items). In this case, the categories are high, medium, low priorities, and the
reform items get assigned a priority. It is just a confusing accident that there happen to be exactly
as many categories as items in this case, and (also unique to this case), each category gets
exactly one item.

-------------------------------------------------

If you are interested in contributing code, ideas, or
data to KTAB, please contact ktab@kapsarc.org

--------------------------------------------
Copyright KAPSARC. Open source MIT License.
--------------------------------------------
