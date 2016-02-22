# KGraph #


This is the start of a very simple graphical library
to insulate the programmer from low-level details of
 the open source, cross-platform, C++ toolkit [FLTK](http://www.fltk.org).

## Build Instructions ##

Because kgraph is a wrapper around FLTK, the first step is to install FLTK.

Under Linux, this is simple: use your package manager to download and install it. This will put the headers and libraries where CMake expects to find them. If you want to build from source, then install it to /usr/local (CMake will also look there).

Under Windows, the most reliable approach we've found is to download the source, build using CMake, and copy it to C:/local.

## Tetris Demo ##

The demo program is very simple version of the classic Tetris game.
Because the game was invented in 1984 at the Soviet Academy of Sciences in Moscow, it is worth knowing how to write "Tetris" with proper Cyrillic letters: Тетрис.

Each game lasts 5 minutes (not counting pauses), until the box fills,
or the user terminates early and starts a new game. Of course, the goals is to score as many points as possible in one game.


The commands are pretty standard:

	left:      4, left arrow
	right:     6, right arrow
	hard drop: 8, up arrow, space bar
	soft drop: 2, down arrow
	
	rotate R (CW):  3, 9, x
	rotate L (CCW): 1, 7, z
	
	pause/resume: 5
	pause: p
	resume with delay: r
	new game: n
	quit: q

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

