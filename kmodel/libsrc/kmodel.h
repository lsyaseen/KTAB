// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
// The MIT License (MIT)
// 
// Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom 
// the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// --------------------------------------------
// Define the abstract top-level classes of KTAB models.
// -------------------------------------------------
// We try to handle matrices and vectors in such a way
// as to minimize transpositions.
// The KMatrix u_ij is the utility to actor i of the position held by actor j.
// If p_j is a column-vector of the probabilities of each position being
// the actual outcome of this state, then  e = u*p is the column-vector
// of expected value to each actor, e_i.
// With that same u matrix, suppose w_j is the row-vector of
// the effective weights of each actor in this state, then
// z = w * u is the row-vector of weighted sums of utilities.
// Hence, we always treat u_ij, p_i, e_i, w_j, and z_j as
// matrix, c-vec, c-vec, r-vec, and r-vec respectively.
// -------------------------------------------------
#ifndef KTAB_MODEL_H
#define KTAB_MODEL_H

#include "kutils.h"
#include "kmatrix.h"
#include "prng.h"

namespace KBase {
  using std::shared_ptr;
  using std::tuple;
  using KBase::KMatrix;
  using KBase::ReportingLevel;

  class KMatrix;
  class PRNG;
  class Model;
  class Actor;
  class Position;
  class State;

  // How much influence to exert (vote) given a difference in [0,1] utility
  enum class VotingRule : char { Binary, PropBin, Proportional, PropCbc, Cubic };
  // No more than 256 distinct voting rules

  string vrName(VotingRule vr);

  enum class ThirdPartyCommit { None, Semi, Full };
  // third parties have the same range of voting rules as in VotingRule enum.
  string tpcName(ThirdPartyCommit tpc);

  class Model {
  public:
    Model(PRNG * r);
    virtual ~Model();

    // In the abstract, you run a model by stepping it until it is time to stop.
    // In detail, each step is likely to record copious information to
    // a database for analysis, and the stopping criterion is likely to
    // lambda-bind a lot of parameters to determine whether anything
    // significant is likely to happen if the run were to continue.
    void run();

    static double nProd(double x, double y);

    // simple voting based on the differrence in utility.
    static double vote(VotingRule vr, double wi, double uij, double uik);

    // The voting function can index into only occupied states, or a mix of occupied and hypothetical states,
    // or into a set of potential two- or three-actor bargains, etc. etc.
    // The vfn can estimate from any actor's perspective.
    // The actors can do simple or complex voting.

    // calculate strength of coalitions for general actors and options.
    static KMatrix coalitions(function<double(unsigned int ak, unsigned int pi, unsigned int pj)> vfn,
			      unsigned int numAct, unsigned int numOpt);

    // whether you consider the probability of a coalition winning to go up linearly
    // or quadratically in strength-compared-to-opponent:
    // Linear law says 2:1 advantage gives pv = 2/3, and 3:1 gives 3/4
    // Square law says 2:1 advantage gives pv = 4/5, and 3:1 gives 9/10
    //
    enum class VPModel { Linear, Square };
    static string VPMName(VPModel vpm);

    // calculate pv[i>j] from coalitions
    static KMatrix vProb(VPModel vpm, const KMatrix & c);

    // assumes simple voting over those options with that utility matrix,
    // builds coalitions, and return pv[i>j]
    static KMatrix vProb(VotingRule vr, VPModel vpm, const KMatrix & w, const KMatrix & u);

    // calculate column vector P[i] from square matrix pv[i>j]
    static KMatrix probCE(const KMatrix & pv);
    static KMatrix markovPCE(const KMatrix & pv);
    static KMatrix condPCE(const KMatrix & pv);

    static KMatrix scalarPCE(unsigned int numAct, unsigned int numOpt, const KMatrix & w,
			     const KMatrix & u, VotingRule vr, VPModel vpm, ReportingLevel rl);

    virtual unsigned int addActor(Actor* a);
    int actrNdx(const Actor* a) const;

    int addState(State* s);

    function <bool(unsigned int iter, const State* s)> stop; // you have to provide this λ-fn

    // these should probably be less public and more protected
    vector<Actor*> actrs;
    unsigned int numAct;
    PRNG * rng;
    vector<State*> history;
  };


  class State {
  public:
    State(Model* mod);
    virtual ~State();


    void randomizeUtils(double minU, double maxU, double uNoise); //testing only

    void clear();
    virtual void addPstn(Position* p);

    // use the parameters of your state to compute the relative probability of each actor's position.
    // persp = -1 means use everyone's separate perspectives (i.e. get actual probabilities, not one actor's beliefs)
    virtual KMatrix pDist(int persp) const = 0;

    Model * model;
    function <State* () > step; // you have to provide this λ-fn
    vector<KMatrix> aUtil; // aUtil[h](i,j) is h's estimate of the utility to A_i of Pos_j
    vector<Position*> pstns;

  };


  // In the abstract, we can not say much about actors.
  // Different utility models might assume different parameters
  // in the utility function - and the associated actor
  // would have to have their own value for those parameters.
  // For example, their capability may change over time,
  // so an actor like that would have to have a history of capabilities
  // into  which the State could look.
  // An actor in multi-dimensional issues might have different
  // capabilities on different issues.
  // They do not have fixed positions, but change over time.
  // And so on.
  class Actor {
  public:
    Actor(string n, string d);
    virtual ~Actor();

    static double thirdPartyVoteSU(double wk, VotingRule vr, ThirdPartyCommit comm,
				   double pik, double pjk, double uki, double ukj, double ukk);

    static double vProbLittle(VotingRule vr, double wn, double uni, double unj, double contrib_i_ij, double contrib_j_ij);

    string name; // a short name, usually 2-5 characters
    string desc; // short description, like a line or two.


    // the most common kinds of votes for actors are the following:

    // Vote between positions occupied by two different actors in the same
    // state, just looking up stored information. 
    // Note well: it cannot be assumed that the vote between two
    // options can be determined simply by looking at the difference
    // in stored utilities.
    virtual double vote(unsigned int p1, unsigned int p2, const State* st) const = 0;

    // Bear in mind that some Domain Specific Utility Models, like
    // full-scale computable general equilibrium (CGE) models, may
    // be very expensive to evaluate. Evaluating one policy position yields
    // a predicted state to which all the actors react with their differing utility fns.
    // Thus, we can get a whole column of the U_ij matrix from one CGE evaluation; 
    // it would be very slow to evaluate the model once for each actor.
    // Of course, if every actor has a completely different model to predict the
    // states that will result from a policy, then you have no choice but to
    // run each model separately for each actor - so it's best to keep them simple.
    // It may turn out that in policy debates, actors actually use very simple
    // heuristics that are *informed* by complex off-line considerations, so you'd
    // be back to using very simple models.
    // Most actors will have functions somewhat like the following, but
    // the number and type of inputs will vary. Use lambda-fns to 
    // construct the 'vfn' for Models::coalition, binding whatever extra
    // parameters your actor needs (e.g. a State* is necessary for strategic voting).
    // double posUtil(const Position * ap1) const = 0; 
    // double vote(const Position * ap1, const Position * ap2) const = 0;


  protected:
  };


  // -------------------------------------------------
  // Similarly, there is not much to say about abstract positions.
  class Position {
  public:
    Position();
    virtual ~Position();
  };

  class VctrPstn;
  class MtchPstn;
  class MtchGene;

  // ------------------------------------------------- 

  // Basic vector position: just a column-vector of numbers.
  // They could be interpretted in many ways, as policies
  // are often described by a vector of numbers (e.g. tax/subsidy rates).
  class VctrPstn : public Position, public KMatrix {
  public:
    VctrPstn();
    VctrPstn(unsigned int nr, unsigned int nc);
    VctrPstn(const KMatrix & m); // copy constructor
    virtual ~VctrPstn();
  };

  // ------------------------------------------------- 
  // this is a matching of N items to M categories.
  // Note that this is intended to be independent of MtchState, MtchActor, etc.
  // Thus, there are M^N possible matchings.
  // Examples are items = pieces of candy, categories = which actor gets them,
  // or items = projects to fund, categories = {High, Medium, Low} priority, actors = interest groups
  // or items = cabinet seats, categories = parties, actors = interest groups
  // or ....
  class MtchPstn : public Position {
  public:
    MtchPstn();
    virtual ~MtchPstn();
    virtual vector<MtchPstn> neighbors(unsigned int nVar) const; 
    // assumes no interaction between items (permutation requires interaction)
    
    unsigned int numItm;
    unsigned int numCat;
    vector<unsigned int> match; // must be of length numItm     
  };


  // bundle up methods relevant to GA over MtchPstn
  class MtchGene : public MtchPstn {
  public:
    MtchGene();
    ~MtchGene();

    void randomize(PRNG* rng);
    MtchGene * mutate(PRNG * rng) const;
    tuple<MtchGene*, MtchGene*> cross(const MtchGene * g2, PRNG * rng) const;
    void show() const;
    bool equiv(const MtchGene * g2) const;

    void setState(vector<Actor*> as, vector<MtchPstn*> ps);

  protected:
    void copySelf(MtchGene*) const;
    // links to the State are necessary to evaluate the net support, EU, etc.
    vector<Actor*> actrs;
    vector<MtchPstn*> pstns;
  };


};


// -------------------------------------------------
#endif

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
