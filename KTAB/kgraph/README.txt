=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright KAPSARC. Open Source MIT License.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
This is the start of a very simple graphical library
to insulate the programmer from low-level details of
FLTK.

---------------------------------------------
The demo program is very simple version of the classic Tetris game.
Because the game was invented in 1984 at the Soviet Academy of Sciences
in Moscow, it is worth knowing how to write "Tetris" with proper
Cyrillic letters: Тетрис.

Each game lasts 5 minutes (not counting pauses), until the box fills,
or the user terminates early and starts a new game. Of course, the point
is to score as many points as possible in one game.

---------------------------------------------
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

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright KAPSARC. Open Source MIT License.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

