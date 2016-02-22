# KTAB Examples


KTAB is an open-source toolkit for assembling agent-based models of negotiation and bargaining using the Probabilistic Condorcet Election paradigm. Important URLs for KTAB include the following: 

- Home page for [KTAB](https://www.kapsarc.org/openkapsarc/kapsarc-toolkit-for-behavioral-analysis-ktab/).
- Main [GitHub](https://github.com/) access at [KAPSARC](https://github.com/KAPSARC)/[KTAB](https://github.com/KAPSARC/KTAB)
- Main development site is the King Abdullah Center for Petroleum Studies and Research, [KAPSARC](http://www.kapsarc.org) 
 

The "examples" directory holds examples of how to build models outside the main library structure, as one would do for a custom model.
Note that the entire "examples" directory is parallel to the KTAB sub-directory.

This mirrors the recommended structure for building your own custom models: setup a directory parallel to one of the example directories, and build your models there. The CMakeLists.txt file in 'reformpri' uses relative paths that work in the recommended structure, so it can be used as a template for developing your own CMakeLlists.txt files.

Each example contains its own README file which describes that example in more detail. Of course, none of the examples are either calibrated or verified. For rigorous work, one could make whatever local modifications were needed to produce a model which met one's own standards for validation.  
 
-
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
