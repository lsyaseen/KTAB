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

#include <assert.h>
#include <iostream>

#include "kmodel.h"


namespace KBase {

  using std::cout;
  using std::endl;
  using std::flush;

  Position::Position() {}
  Position::~Position() {}


  Model::Model(PRNG * r) {
    history = vector<State*>();
    actrs = vector<Actor*>();
    numAct = 0;
    stop = nullptr;
    assert(nullptr != r);
    rng = r;
  }


  Model::~Model() {
    while (0 < history.size()){
      State* s = history[history.size() - 1];
      delete s;
      history.pop_back();
    }

    while (0 < actrs.size()) {
      Actor * a = actrs[actrs.size() - 1];
      delete a;
      actrs.pop_back();
    }
    numAct = 0;
    rng = nullptr;
  }


  void Model::run() {
    assert(1 == history.size());
    State* s0 = history[0];
    bool done = false;
    unsigned int iter = 0;
    while (!done) {
      assert(nullptr != s0);
      assert(nullptr != s0->step);
      iter++;
      cout << "Starting Model::run iteration " << iter << endl;
      auto s1 = s0->step();
      addState(s1);
      done = stop(iter, s1);
      s0 = s1;
    }
    return;
  }

  string Model::VPMName(VPModel vpm) {
    string s;
    switch (vpm) {
    case  Model::VPModel::Linear:
      s = "Linear";
      break;
    case Model::VPModel::Square:
      s = "Square";
      break;
    default:
      cout << "Model::VPMName: unrecognized VPModel" << endl;
      assert(false);
      break;
    }
    return s;
  }


  unsigned int Model::addActor(Actor* a) {
    assert(nullptr != a);
    actrs.push_back(a);
    numAct = actrs.size();
    return numAct;
  }


  int Model::addState(State* s) {
    assert(nullptr != s);
    assert(this == s->model);
    s->model = this;
    history.push_back(s);
    return history.size();
  }


  int Model::actrNdx(const Actor* a) const {
    int ai = -1;
    for (unsigned int i = 0; i < numAct; i++) {
      if (a == actrs[i]) {
        ai = i;
      }
    }
    return ai; // negative iff this "actor" was not found
  }

  double Model::nProd(double x, double y) {
    if ((0.0 < x) && (0.0 < y)) { return (x * y); }
    if ((x < 0.0) && (y < 0.0)) { return (x + y); }
    return ((x < y) ? x : y);
  }

  string vrName(VotingRule vr) {
    string vrn = "";
    switch (vr) {
    case VotingRule::Binary:
      vrn = "Binary";
      break;
    case VotingRule::PropBin:
      vrn = "PropBin";
      break;
    case VotingRule::Proportional:
      vrn = "Prop";
      break;
    case VotingRule::PropCbc:
      vrn = "PropCbc";
      break;
    case VotingRule::Cubic:
      vrn = "Cubic";
      break;
    default:
      throw KException("vrName - Unrecognized VotingRule");
      break;
    }
    return vrn;
  }

  string tpcName(ThirdPartyCommit tpc) {
    string tpcn;
    switch (tpc) {
    case ThirdPartyCommit::FullCommit:
      tpcn = "FullCommit";
      break;
    case ThirdPartyCommit::SemiCommit:
      tpcn = "SemiCommit";
      break;
    case ThirdPartyCommit::NoCommit:
      tpcn = "NoCommit";
      break;

    default:
      throw KException("tpcName - Unrecognized ThirdPartyCommit");
      break;

    }

    return tpcn;
  }

  double Model::vote(VotingRule vr, double wi, double uij, double uik) {
    double v = 0.0;
    const double du = uij - uik;

    double rBin = 0; // binary response
    const double sTol = 1E-10;
    rBin = du / sTol;
    rBin = (rBin > +1) ? +1 : rBin;
    rBin = (rBin < -1) ? -1 : rBin;

    double rProp = du; // proportional response
    double rCubic = du * du * du; // cubic reponse

    // the following weights determine how much the hybrids deviate from proportional
    const double rbp = 0.8;
    // rbp = 0.8 makes LHS slope and RHS slope of equal size (0.8 each), and twice the center jump (0.4)
    const double rpc = 0.5196;
    // rpc is chosen so max deviation of PropCbc (at 1/sqrt(3)) equals max deviation by PropBin (at 0):
    // rpc = (3*sqrt(3)*rbp)/2
    switch (vr) {
    case VotingRule::Binary:
      v = wi * rBin;
      break;

    case VotingRule::PropBin:
      v = wi * ((1 - rbp)*rProp + rbp*rBin);
      break;

    case VotingRule::Proportional:
      v = wi * rProp;
      break;

    case VotingRule::PropCbc:
      v = wi * ((1 - rpc)*rProp + rpc*rCubic);
      break;

    case VotingRule::Cubic:
      v = wi* rCubic;
      break;

    default:
      throw KException("Model::vote - Unrecognized VotingRule");
      break;
    }
    return v;
  }


  // note that while the C_ij can be any arbitrary positive matrix,
  // the p_ij matrix has the symmetry pij + pji = 1,
  KMatrix Model::vProb(VPModel vpm, const KMatrix & c) {
    unsigned int numOpt = c.numR();
    assert(numOpt == c.numC());
    auto p = KMatrix(numOpt, numOpt);
    for (unsigned int i = 0; i < numOpt; i++) {
      for (unsigned int j = 0; j < i; j++) {
        double cij = c(i, j);
        assert(0 <= cij);
        double cji = c(j, i);
        assert(0 <= cji);
        assert((0 < cij) || (0 < cji));
        switch (vpm) {
        case VPModel::Linear:
          // no change
          break;
        case VPModel::Square:
          cij = cij*cij;
          cji = cji*cji;
          break;
        }
        p(i, j) = cij / (cij + cji);  // set the lower left probability
        p(j, i) = cji / (cij + cji);  // set the upper right probability
      }
      p(i, i) = 0.5; // set the diagonal probability
    }
    return p;
  }

  KMatrix Model::coalitions(function<double(unsigned int ak, unsigned int pi, unsigned int pj)> vfn,
    unsigned int numAct, unsigned int numOpt) {
    // if several actors occupy the same position, then numAct > numOpt 
    const double minC = 1E-8;
    auto c = KMatrix(numOpt, numOpt);
    for (unsigned int i = 0; i < numOpt; i++) {
      for (unsigned int j = 0; j < numOpt; j++) {
        if (j < i) { // I have my reasons for doing it this way this time
          double cij = minC;
          double cji = minC;
          for (unsigned int k = 0; k < numAct; k++) {
            double vkij = vfn(k, i, j);
            if (vkij > 0) { cij = cij + vkij; }
            if (vkij < 0) { cji = cji - vkij; }
          }
          c(i, j) = cij;  // set the lower left coalition
          c(j, i) = cji;  // set the upper right coalition
        }
      }
      c(i, i) = minC; // set the diagonal coalition
    }
    return c;
  }

  // these are assumed to be unique options.
  // returns a square matrix.
  KMatrix Model::vProb(VotingRule vr, VPModel vpm, const KMatrix & w, const KMatrix & u) {
    // u_ij is utility to actor i of the position advocated by actor j
    unsigned int numAct = u.numR();
    unsigned int numOpt = u.numC();
    // w_j is row-vector of actor weights, for simple voting 
    assert(numAct == w.numC()); // require 1-to-1 matching of actors and strengths
    assert(1 == w.numR()); // weights must be a row-vector

    auto vfn = [vr, &w, &u](unsigned int k, unsigned int i, unsigned int j) {
      double vkij = vote(vr, w(0, k), u(k, i), u(k, j));
      return vkij;
    };

    auto c = coalitions(vfn, numAct, numOpt); // c(i,j) = strength of coaltion for i against j
    KMatrix p = vProb(vpm, c);  // p(i,j) = prob Ai defeats Aj
    return p;
  }



  // Given square matrix of Prob[i>j] returns a column vector for Prob[i]
  KMatrix Model::probCE(const KMatrix & pv) {
    const double pTol = 1E-6;
    unsigned int numOpt = pv.numR();
    assert(numOpt == pv.numC()); // must be square
    auto test = [&pv, pTol](unsigned int i, unsigned int j) {
      assert(0 <= pv(i, j));
      assert(fabs(pv(i, j) + pv(j, i) - 1.0) < pTol);
      return;
    };
    KMatrix::mapV(test, numOpt, numOpt); // catch gross errors 

    // TODO: add a switch between markovPCE and condPCE?
    //KMatrix p = markovPCE(pv); 
    KMatrix p = condPCE(pv); 

    assert(fabs(sum(p) - 1.0) < pTol);
    return p;
  }


  // Given square matrix of Prob[i>j] returns a column vector for Prob[i].
  // Uses Markov process, not 1-step conditional probability
  KMatrix Model::markovPCE(const KMatrix & pv) {
    const double pTol = 1E-6;
    unsigned int numOpt = pv.numR();
    auto p = (KMatrix(numOpt, 1) + 1) / numOpt;  // all 1/n
    auto q = p;
    unsigned int iMax = 1000;  // 10-30 is typical
    unsigned int iter = 0;
    double change = 1.0;
    while (pTol < change) {
      change = 0;
      for (unsigned int i = 0; i < numOpt; i++){
        double pi = 0.0;
        for (unsigned int j = 0; j < numOpt; j++) {
          pi = pi + pv(i, j)*(p(i, 0) + p(j, 0));
        }
        assert(0 <= pi); // double-check
        q(i, 0) = pi / numOpt;
        double c = fabs(q(i, 0) - p(i, 0));
        change = (c>change) ? c : change;
      }
      p = q;
      iter++;
      assert(fabs(sum(p) - 1.0) < pTol); // double-check
    }
    assert(iter < iMax); // no way to recover
    return p;
  }


  // Given square matrix of Prob[i>j] returns a column vector for Prob[i].
  // Uses 1-step conditional probabilities, not Markov process
  KMatrix Model::condPCE(const KMatrix & pv) { 
    unsigned int numOpt = pv.numR();
    auto p = KMatrix(numOpt, 1); 
    for (unsigned int i = 0; i < numOpt; i++) {
      double pi = 1.0;
      for (unsigned int j = 0; j < numOpt; j++) {
        pi = pi * pv(i, j);
      }
      // double-check
      assert(0 <= pi); 
      assert(pi <= 1);
      p(i, 0) = pi; // probability that i beats all alternatives
    }
    double probOne = sum(p); // probability that one option, any option, beats all alternatives
    p = (p / probOne); // conditional probability that i is that one.
    return p;
  }

  // This assumes scalar capabilities of actors (w), so that the voting strength
  // is a direct function of difference in utilities.Therefore, we can use 
  // Model::vProb(VotingRule vr, const KMatrix & w, const KMatrix & u)
  KMatrix Model::scalarPCE(unsigned int numAct, unsigned int numOpt, const KMatrix & w, const KMatrix & u,
    VotingRule vr, VPModel vpm, ReportingLevel rl) {

    auto pv = Model::vProb(vr, vpm, w, u);
    auto p = Model::probCE(pv);

    if (ReportingLevel::Low < rl) {
      printf("Num actors: %i \n", numAct);
      printf("Num options: %i \n", numOpt);


      if ((numAct <= 20) && (numOpt <= 20)) {
        cout << "Actor strengths: " << endl;
        w.mPrintf(" %6.2f ");
        cout << endl << flush;
        cout << "Voting rule: " << KBase::vrName(vr) << endl;
        // printf("         aka %s \n", KBase::vrName(vr).c_str());
        cout << flush;
        cout << "Utility to actors of options: " << endl;
        u.mPrintf(" %+8.3f ");
        cout << endl;

        auto vfn = [vr, &w, &u](unsigned int k, unsigned int i, unsigned int j) {
          double vkij = vote(vr, w(0, k), u(k, i), u(k, j));
          return vkij;
        };

        auto c = coalitions(vfn, numAct, numOpt); // c(i,j) = strength of coaltion for i against j
        KMatrix p2 = vProb(vpm, c);  // p(i,j) = prob Ai defeats Aj

        cout << "Coalition strengths of (i:j): " << endl;
        c.mPrintf(" %8.3f ");
        cout << endl;

        assert(norm(pv - p2) < 1E-8); // better be close

        cout << "Probability Opt_i > Opt_j" << endl;
        pv.mPrintf(" %.4f ");
        cout << "Probability Opt_i" << endl;
        p.mPrintf(" %.4f ");
      }
      cout << "Found stable PCE distribution" << endl << flush;
    }
    return p;
  }


  // -------------------------------------------------
  Actor::Actor(string n, string d) {
    name = n;
    desc = d;
  }


  Actor::~Actor() {
    // empty
  }

  // this uses the evenly-weighted "Sum of Utilities" model
  // Note that the pi and pj numbers already include the contribution of k to the hypothetical little 
  // i:j conflict with just i,j,k involved
  // TODO: implement and offer the "Expected Value of Utilities" model
  double Actor::thirdPartyVoteSU(double wk, VotingRule vr, ThirdPartyCommit comm,
    double pik, double pjk, double uki, double ukj, double ukk) {
    const double pTol = 1E-8;
    assert(0 <= pik);
    assert(0 <= pjk);
    assert(fabs(pik + pjk - 1.0) < pTol);
    double p_ik_j = pik;
    double p_jk_i = pjk;
    double p_j_ik = pjk;
    double p_i_jk = pik;
    double u_ik_def_j = 0;
    double u_j_def_ik = 0;
    double u_i_def_jk = 0;
    double u_jk_def_i = 0;
    // strictly speaking, we should add in all the utilities of unchanged positions, then divide by numAct,
    // so that the utility of each alternative state is between 0 and 1. 
    // However, 
    // A: subtraction will make all utilities of unchanged positions cancel here when vk is determined, and 
    // B: dividing in P[1>2] = C12 / (C12 + C21) will cancel the factors of 1/4.
    //
    // Therefore, we can drop those terms as long we make sure all comparisons use the sum 
    // of three position-utilities, and we obviously do so.
    switch (comm) {
    case ThirdPartyCommit::FullCommit:
      u_ik_def_j = uki + uki + uki;
      u_j_def_ik = ukj + ukj + ukj;
      u_i_def_jk = uki + uki + uki;
      u_jk_def_i = ukj + ukj + ukj;
      break;
    case ThirdPartyCommit::SemiCommit:
      u_ik_def_j = uki + uki + ukk;
      u_j_def_ik = ukj + ukj + ukj;
      u_i_def_jk = uki + uki + uki;
      u_jk_def_i = ukj + ukj + ukk;
      break;
    case ThirdPartyCommit::NoCommit:
      u_ik_def_j = uki + uki + ukk;
      u_j_def_ik = ukj + ukj + ukk;
      u_i_def_jk = uki + uki + ukk;
      u_jk_def_i = ukj + ukj + ukk;
      break;
    }
    double u_ik_j = (p_ik_j * u_ik_def_j) + (p_j_ik * u_j_def_ik);
    double u_i_jk = (p_i_jk * u_i_def_jk) + (p_jk_i * u_jk_def_i);
    double vk = Model::vote(vr, wk, u_ik_j, u_i_jk);
    return vk;
  }


  // Actor n determines his contribution on influence by analyzing the hypothetical 
  // "little conflict" of just three actors, i.e.  (i:j) with n weighing in on whichever side it favors.
  double Actor::vProbLittle(VotingRule vr, double wn, double uni, double unj, double contrib_i_ij, double contrib_j_ij) {

    // the primary actors are assigned at least the minisucle
    // minimum influence contribution, to avoid 0/0 errors.
    if (0 < contrib_i_ij){
      assert(0 < contrib_i_ij);
    }
    if (0 < contrib_j_ij){
      assert(0 < contrib_j_ij);
    }

    double contrib_n_ij = Model::vote(vr, wn, uni, unj);
    double cni = (contrib_n_ij > 0) ? contrib_i_ij + contrib_n_ij : contrib_i_ij;
    double cnj = (contrib_n_ij < 0) ? contrib_j_ij - contrib_n_ij : contrib_j_ij;
    if (0 < cni) {
      assert(0 < cni);
    }
    if (0 < cnj) {
      assert(0 < cnj);
    }
    double pin = cni / (cni + cnj);
    //double pjn = cnj / (cni + cnj); // FYI
    return pin;
  }

} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
