---------------------------------------------
 Copyright KAPSARC. Open Source MIT License.
---------------------------------------------

The 'agenda' example builds a simple implementation
of an agenda-setting actor manipulating the agenda to maximize
the expected utility for itself. This actor is the chairperson.

The other actors have significant votes, and they vote between
alternatives based on the expected utility to themselves of the 
potential outcomes. It does look ahead and estimate the voting
and expected values for each possible follow-on. As there
are only two alternatives at each stage, and they are distinct
from each other by construction, the PCE is quite simple.

Each pre-determined agenda is written as a tree of binary votes.
Suppose there are five options, 0, 1, 2, 3, and 4.
Then [[2:3]:[[0:1]:4]] represents the following agenda:

First, the group votes on {2,3} : {0,1,4}

If {2,3} wins, then the next and final vote is {2}:{3}.

If {0,1,4} wins, then the next vote is on {0,1}:{4}
If {4} wins, then that is the final choice.
If {0,1} wins, then the final vote is {0}:{1}

Six example runs are included. First, the 
chairperson could expect voters to use proportional
or binary voting. Second, the chairman could be free to
set any agenda it wants, limited to somewhat balanced agendas, 
or limited to balanced agendas. In the moderately-balanced case,
an initial vote like {0,1,2,3,4}:{5} would not be allowed.
The balance rule requires that with n<=5 items,
the smaller set has to be at least n/2, and with 6<=n the
smaller set has to have at least n/3

---------------------------------------------

The next obvious choice is to have several actors bargain
over the agenda, each estimating their own expected utility
from each potential agenda. The purpose of this step would
be to address the problem that, if the outcomes of votes
depends mainly on the insitutions (e.g. agendas), then the
serious bargaining is over the choice of institutions.

---------------------------------------------
 Copyright KAPSARC. Open Source MIT License.
---------------------------------------------

