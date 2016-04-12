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
//
// Demonstrate a very basic, but highly parameterizable, Spatial Model of Politics.
//
// --------------------------------------------

#include "smp.h"
#include "csv_parser.hpp"


namespace SMPLib {
  using std::cout;
  using std::endl;
  using std::flush;
  using std::function;
  using std::get;
  using std::string;

  using KBase::PRNG;
  using KBase::KMatrix;
  using KBase::KException;
  using KBase::Actor;
  using KBase::Model;
  using KBase::Position;
  using KBase::VctrPstn;
  using KBase::BigRAdjust;
  using KBase::BigRRange;
  using KBase::VPModel;
  using KBase::State;
  using KBase::VotingRule;
  using KBase::PCEModel;
  using KBase::ReportingLevel;




  BargainSMP::BargainSMP(const SMPActor* ai, const SMPActor* ar, const VctrPstn & pi, const VctrPstn & pr) {
    assert(nullptr != ai);
    assert(nullptr != ar);
    actInit = ai;
    actRcvr = ar;
    posInit = pi;
    posRcvr = pr;
  }

  BargainSMP::~BargainSMP() {
    actInit = nullptr;
    actRcvr = nullptr;
    posInit = VctrPstn(KMatrix(0, 0));
    posRcvr = VctrPstn(KMatrix(0, 0));
  }

  SMPActor::SMPActor(string n, string d) : Actor(n, d) {
    vr = VotingRule::Proportional; // just a default
  }

  SMPActor::~SMPActor() {
  }

  double SMPActor::vote(unsigned int, unsigned int, const State*) const {
    assert(false);  // TDO: finish this
    return 0;
  }


  double SMPActor::vote(const Position * ap1, const Position * ap2, const SMPState* ast) const {
    double u1 = posUtil(ap1, ast);
    double u2 = posUtil(ap2, ast);
    double v = Model::vote(vr, sCap, u1, u2);
    return v;
  }


  double SMPActor::posUtil(const Position * ap1, const SMPState* as) const {
    assert(nullptr != as);
    int ai = as->model->actrNdx(this);
    double ri = as->aNRA(ai); //as->nra(ai, 0);
    assert(0 <= ai);
    const VctrPstn* p0 = ((const VctrPstn*)(as->pstns[ai]));
    assert(nullptr != p0);
    auto p1 = ((const VctrPstn*)ap1);
    assert(nullptr != p1);
    double u1 = SMPModel::bvUtil((*p0) - (*p1), vSal, ri);
    return u1;
  }


  void SMPActor::randomize(PRNG* rng, unsigned int numD) {
    sCap = rng->uniform(10.0, 200.0);

    // assign an overall salience, and then by-component saliences
    double s = rng->uniform(0.75, 0.99);
    vSal = KMatrix::uniform(rng, numD, 1, 0.1, 1.0);
    vSal = (s * vSal) / sum(vSal);
    assert(fabs(s - sum(vSal)) < 1E-4);

    // I could randomly assign different voting rules
    // to different actors, but that would just be too cute.
    vr = VotingRule::Proportional;

    return;
  }



  void SMPActor::interpBrgnSnPm(unsigned int n, unsigned int m,
    double tik, double sik, double prbI,
    double tjk, double sjk, double prbJ,
    double & bik, double & bjk) {

    auto round4 = [](const double x1) {
      const double s = 10000.0;
      const int y = ((int)(0.5 + (x1*s)));
      double x2 = ((double)y) / s;
      return x2;
    };
    assert((1 == n) || (2 == n));
    assert((1 == m) || (2 == m));

    double wsi = pow(sik, n);
    double wpi = pow(prbI, m);
    double wik = wsi * wpi;

    double wsj = pow(sjk, n);
    double wpj = pow(prbJ, m);
    double wjk = wsj * wpj;

    // imagine that either neither actor cares, or neither actor can coerce the other,
    // so that wik = 0 = wjk. We need to avoid 0/0 error, and have bi=ti and bj=tj.
    // Thus, the asymmetry is intentional when wik = 0 = wjk.
    // to avoid spurious asymmetry in other cases, and spurious precision always, round to 4 decimals
    const double minW = 1e-6;
    bik = round4(((wik + minW)*tik + wjk*tjk) / (wik + minW + wjk));
    bjk = round4((wik*tik + (minW + wjk)*tjk) / (wik + minW + wjk));

    return;
  }


  void SMPActor::interpBrgnS2PMax(double tik, double sik, double prbI,
    double tjk, double sjk, double prbJ,
    double & bik, double & bjk) {
    double di = (prbJ > prbI) ? (prbJ - prbI) : 0;  // max(0, prbJ - prbI);
    double dj = (prbI > prbJ) ? (prbI - prbJ) : 0;  // max(0, prbI - prbJ);
    double sik2 = sik * sik;
    double sjk2 = sjk * sjk;

    const double minW = 1e-6;
    double dik = (di * sjk2) / ((di * sjk2) + minW + ((1 - di) * sik2));
    double djk = (dj * sik2) / ((dj * sik2) + minW + ((1 - dj) * sjk2));


    bik = tik + dik * (tjk - tik);
    bjk = tjk + djk * (tik - tjk);
    return;
  }


  BargainSMP* SMPActor::interpolateBrgn(const SMPActor* ai, const SMPActor* aj,
    const VctrPstn* posI, const VctrPstn * posJ,
    double prbI, double prbJ, InterVecBrgn ivb) {
    assert((1 == posI->numC()) && (1 == posJ->numC()));
    unsigned int numD = posI->numR();
    assert(numD == posJ->numR());
    auto brgnI = VctrPstn(numD, 1);
    auto brgnJ = VctrPstn(numD, 1);

    for (unsigned int k = 0; k < numD; k++) {
      double tik = (*posI)(k, 0);
      double sik = ai->vSal(k, 0);

      double tjk = (*posJ)(k, 0);
      double sjk = aj->vSal(k, 0);
      double & bik = tik;
      double & bjk = tjk;
      switch (ivb) {
      case InterVecBrgn::S1P1:
        interpBrgnSnPm(1, 1, tik, sik, prbI, tjk, sjk, prbJ, bik, bjk);
        break;
      case InterVecBrgn::S2P2:
        interpBrgnSnPm(2, 2, tik, sik, prbI, tjk, sjk, prbJ, bik, bjk);
        break;
      case InterVecBrgn::S2PMax:
        interpBrgnS2PMax(tik, sik, prbI, tjk, sjk, prbJ, bik, bjk);
        break;
      default:
        throw KException("interpolateBrgn: unrecognized InterVecBrgn value");
        break;
      }
      brgnI(k, 0) = bik;
      brgnJ(k, 0) = bjk;
    }

    auto brgn = new BargainSMP(ai, aj, brgnI, brgnJ);
    return brgn;
  }



  SMPState::SMPState(Model * m) : State(m) {
    nra = KMatrix();
  }


  SMPState::~SMPState() {
    nra = KMatrix();
  }



  void SMPState::setVDiff(const vector<VctrPstn> & vpos) {
    auto dfn = [vpos, this](unsigned int i, unsigned int j) {
      auto ai = ((const SMPActor*)(model->actrs[i]));
      KMatrix si = ai->vSal;
      auto pj = ((const VctrPstn*)(pstns[j]));
      double dij = 0.0;
      if (0 == vpos.size()) {
        auto pi = ((const VctrPstn*)(pstns[i]));
        dij = SMPModel::bvDiff((*pi) - (*pj), si);
      }
      else {
        auto vpi = vpos[i];
        dij = SMPModel::bvDiff(vpi - (*pj), si);
      }
      return dij;
    };

    const unsigned int na = model->numAct;
    vDiff = KMatrix::map(dfn, na, na);
    return;
  }


  void SMPModel::setUtilProb(const KMatrix& vR, const KMatrix& vS, const KMatrix& vD, KBase::VotingRule vr) {
    // vR(i,0) = risk attitude used for actor i (possibly 0, possibly someone's estimate)
    // vS(i,0) = overall salience for actor i
    // vD(i,j) = difference, using i's salience, between i's position and j's position
    const unsigned int na = vD.numR();
    assert(na == vD.numC());
    assert(na == vR.numR());
    assert(1 == vR.numC());
    assert(KBase::sameShape(vR, vS));

    // calculate utility matrix
    // utils(i,j) = utility to actor i of position of actor j
    auto uFn = [vR, vD](unsigned int i, unsigned int j) {
      const double ri = vR(i, 0);
      const double dij = vD(i, j);
      const double uij = bsUtil(dij, ri);
      return uij;
    };
    auto utils = KMatrix::map(uFn, na, na);
    const double aeTol = 1E-8; // arithmetic error tolerance
    const double minCS = 1E-10; // avoid 0/0 errors

    // calculate coalition strengths
    // cs(i,j) = strength of actor i's position against actor j's position
    auto cs = KMatrix(na, na, minCS);
    for (unsigned int i = 0; i < na; i++) {
      for (unsigned int j = 0; j < i; j++) {
        for (unsigned int k = 0; k < na; k++) {
          double sk = vS(k, 0);
          assert(0 < sk);
          assert(sk <= 1.0);
          double uki = utils(k, i);
          double ukj = utils(k, j);
          double vkij = Model::vote(vr, sk, uki, ukj);
          double vkji = Model::vote(vr, sk, ukj, uki);
          assert(fabs(vkij + vkji) < aeTol); // these should exactly cancel
          if (0 < vkij) {
            cs(i, j) = cs(i, j) + vkij;
            assert(0 < cs(i, j));
          }
          if (0 < vkji) {
            cs(j, i) = cs(j, i) + vkji;
            assert(0 < cs(j, i));
          }
        }
      }
    }

    // calculate pairwise victory probabilities
    auto vpm = VPModel::Linear;
    auto vP = KMatrix(na, na);
    for (unsigned int i = 0; i < na; i++) {
      for (unsigned int j = 0; j < i; j++) {
        double pij = cs(i, j) / (cs(i, j) + cs(j, i));
        auto ppr = vProb(vpm, cs(i, j), cs(j, i));
        vP(i, j) = get<0>(ppr); // set the lower left  probability: if Linear, cij / (cij + cji)
        vP(j, i) = get<1>(ppr); // set the upper right probability: if Linear, cji / (cij + cji)
      }
    }
    for (unsigned int i = 0; i < na; i++) { // check arithmetic
      for (unsigned int j = 0; j < na; j++) {
        assert(fabs(vP(i, j) + vP(j, i) - 1.0) < aeTol);
      }
    }


    return;
  };



  double SMPState::estNRA(unsigned int h, unsigned int i, BigRAdjust ra) const {
    double rh = nra(h, 0);
    double ri = nra(i, 0);
    double rhi = Model::estNRA(rh, ri, ra);
    return rhi;
  }

  KMatrix SMPState::actrCaps() const {
    auto wFn = [this](unsigned int i, unsigned int j) {
      auto aj = ((SMPActor*)(model->actrs[j]));
      return aj->sCap;
    };

    auto w = KMatrix::map(wFn, 1, model->numAct);
    return w;
  }

  void SMPState::setAllAUtil(ReportingLevel rl) {
    // you can change these parameters
    auto vr = VotingRule::Proportional;
    auto ra = BigRAdjust::OneThirdRA;
    auto rr = BigRRange::Mid; // use [-0.5, +1.0] scale
    auto vpm = VPModel::Linear;


    const unsigned int na = model->numAct;

    // make sure prerequisities are at least somewhat setup
    assert(na == eIndices.size());
    assert(0 < uIndices.size());
    assert(uIndices.size() <= na);

    auto w_j = actrCaps();
    setVDiff();
    nra = KMatrix(na, 1); // zero-filled, i.e. risk neutral
    auto uFn1 = [this](unsigned int i, unsigned int j) {
      return  SMPModel::bsUtil(vDiff(i, j), nra(i, 0));
    };

    auto rnUtil_ij = KMatrix::map(uFn1, na, na);

    if (ReportingLevel::Silent < rl) {
      cout << "Raw actor-pos value matrix (risk neutral)" << endl;
      rnUtil_ij.mPrintf(" %+.3f ");
      cout << endl << flush;
    }

    auto pv_ij = Model::vProb(vr, vpm, w_j, rnUtil_ij);
    auto p_i = Model::probCE(PCEModel::ConditionalPCM, pv_ij);
    nra = Model::bigRfromProb(p_i, rr);


    if (ReportingLevel::Silent < rl) {
      cout << "Inferred risk attitudes: " << endl;
      nra.mPrintf(" %+.3f ");
      cout << endl << flush;
    }

    auto raUtil_ij = KMatrix::map(uFn1, na, na);

    if (ReportingLevel::Silent < rl) {
      cout << "Risk-aware actor-pos utility matrix (objective):" << endl;
      raUtil_ij.mPrintf(" %+.4f ");
      cout << endl;
      cout << "RMS change in value vs utility: " << norm(rnUtil_ij - raUtil_ij) / na << endl << flush;
    }

    const double duTol = 1E-6;
    assert(duTol < norm(rnUtil_ij - raUtil_ij)); // I've never seen it below 0.07


    if (ReportingLevel::Silent < rl) {
      switch (ra) {
      case BigRAdjust::FullRA:
        cout << "Using " << ra << ": r^h_i = ri" << endl;
        break;
      case BigRAdjust::TwoThirdsRA:
        cout << "Using " << ra << ": r^h_i = (rh + 2*ri)/3" << endl;
        break;
      case BigRAdjust::HalfRA:
        cout << "Using " << ra << ": r^h_i = (rh + ri)/2" << endl;
        break;
      case BigRAdjust::OneThirdRA:
        cout << "Using " << ra << ": r^h_i = (2*rh + ri)/3" << endl;
        break;
      case BigRAdjust::NoRA:
        cout << "Using " << ra << ": r^h_i = rh " << endl;
        break;
      default:
        cout << "Unrecognized BigRAdjust" << endl;
        assert(false);
        break;
      }
    }

    aUtil = vector<KMatrix>();
    for (unsigned int h = 0; h < na; h++) {
      auto u_h_ij = KMatrix(na, na);
      for (unsigned int i = 0; i < na; i++) {
        double rhi = estNRA(h, i, ra);
        for (unsigned int j = 0; j < na; j++) {
          double dij = vDiff(i, j);
          u_h_ij(i, j) = SMPModel::bsUtil(dij, rhi);
        }
      }
      aUtil.push_back(u_h_ij);


      if (ReportingLevel::Silent < rl) {
        cout << "Estimate by " << h << " of risk-aware utility matrix:" << endl;
        u_h_ij.mPrintf(" %+.4f ");
        cout << endl;

        cout << "RMS change in util^h vs utility: " << norm(u_h_ij - raUtil_ij) / na << endl;
        cout << endl;
      }

      assert(duTol < norm(u_h_ij - raUtil_ij)); // I've never seen it below 0.03
    }
    return;
  }


  void SMPState::setOneAUtil(unsigned int perspH, ReportingLevel rl) {
    cout << "SMPState::setOneAUtil - not yet implemented" << endl << flush;


    return;
  }

  void SMPState::showBargains(const vector < vector < BargainSMP* > > & brgns) const {
    for (unsigned int i = 0; i < brgns.size(); i++) {
      printf("Bargains involving actor %u: ", i);
      for (unsigned int j = 0; j < brgns[i].size(); j++) {
        BargainSMP* bij = brgns[i][j];
        if (nullptr != bij) {
          int a1 = model->actrNdx(bij->actInit);
          int a2 = model->actrNdx(bij->actRcvr);
          printf(" [%i:%i] ", a1, a2);
        }
        else {
          printf(" SQ ");
        }
      }
      cout << endl << flush;
    }
    return;
  }

  void SMPState::setNRA() {
    const unsigned int nr = nra.numR();
    nra = KMatrix(nr, 1);
    return;
  }

  double SMPState::aNRA(unsigned int i) const {
    const unsigned int nr = nra.numR();
    assert(nr == model->numAct);
    assert(i < nr);
    const double ri = nra(i, 0);
    return ri;
  }

  void SMPState::addPstn(Position* ap) {
    auto sp = (VctrPstn*)ap;
    auto sm = (SMPModel*)model;
    assert(1 == sp->numC());
    assert(sm->numDim == sp->numR());

    State::addPstn(ap);
    return;
  }


  bool SMPState::equivNdx(unsigned int i, unsigned int j) const {
    /// Compare two actual positions in the current state
    auto vpi = ((const VctrPstn *)(pstns[i]));
    auto vpj = ((const VctrPstn *)(pstns[j]));
    assert(vpi != nullptr);
    assert(vpj != nullptr);
    double diff = norm((*vpi) - (*vpj));
    auto sm = ((const SMPModel*)model);
    bool rslt = (diff < sm->posTol);
    return rslt;
  }


  // set the diff matrix, do probCE for risk neutral,
  // estimate Ri, and set all the aUtil[h] matrices
  SMPState* SMPState::stepBCN() {
    auto gSetup = [](SMPState* s) {
      if ((0 == s->uIndices.size()) || (0 == s->eIndices.size())) {
        s->setUENdx();
      }
      if (0 == s->aUtil.size()) {
        s->setAUtil(-1, ReportingLevel::Low);
      }
      return;
    };
    gSetup(this);
    int myT = -1;
    for (unsigned int t = 0; t < model->history.size(); t++) {
      if (this == model->history[t]) {
        myT = t;
      }
    }
    assert(0 <= myT);
    model->sqlAUtil(myT);
    // Notice that this does not record the next state.
    // That gets recorded upon the next state - but it
    // therefore misses the very last state.
    auto s2 = doBCN();
    gSetup(s2);
    s2->step = [s2]() {
      return s2->stepBCN();
    };
    return s2;
  }


  SMPState* SMPState::doBCN() const {
    auto brgns = vector< vector < BargainSMP* > >();
    const unsigned int na = model->numAct;
    brgns.resize(na);
    for (unsigned int i = 0; i < na; i++) {
      brgns[i] = vector<BargainSMP*>();
      brgns[i].push_back(nullptr); // null bargain is SQ
    }

    auto ivb = SMPActor::InterVecBrgn::S2P2;
    // For each actor, identify good targets, and propose bargains to them.
    // (This loop would be an excellent place for high-level parallelism)
    for (unsigned int i = 0; i < na; i++) {
      auto chlgI = bestChallenge(i);
      int bestJ = get<0>(chlgI);
      double piJ = get<1>(chlgI);
      double bestEU = get<2>(chlgI);
      if (0 < bestEU) {
        assert(0 <= bestJ);

        printf("Actor %u has most advantageous target %i worth %.3f\n", i, bestJ, bestEU);

        auto ai = ((const SMPActor*)(model->actrs[i]));
        auto aj = ((const SMPActor*)(model->actrs[bestJ]));
        auto posI = ((const VctrPstn*)pstns[i]);
        auto posJ = ((const VctrPstn*)pstns[bestJ]);
        BargainSMP* brgnIJ = SMPActor::interpolateBrgn(ai, aj, posI, posJ, piJ, 1 - piJ, ivb);
        auto nai = model->actrNdx(brgnIJ->actInit);
        auto naj = model->actrNdx(brgnIJ->actRcvr);

        brgns[i].push_back(brgnIJ); // initiator's copy, delete only it later
        brgns[bestJ].push_back(brgnIJ); // receiver's copy, just null it out later

        printf(" %2i proposes %2i adopt: ", nai, nai);
        KBase::trans(brgnIJ->posInit).mPrintf(" %.3f ");
        printf(" %2i proposes %2i adopt: ", nai, naj);
        KBase::trans(brgnIJ->posRcvr).mPrintf(" %.3f ");
      }
      else {
        printf("Actor %u has no advantageous targets \n", i);
      }
    }


    cout << endl << "Bargains to be resolved" << endl << flush;
    showBargains(brgns);

    auto w = actrCaps();
    cout << "w:" << endl;
    w.mPrintf(" %6.2f ");

    // of course, you  can change these two parameters
    auto vr = VotingRule::Proportional;
    auto vpm = VPModel::Linear;

    auto ndxMaxProb = [](const KMatrix & cv) {

      const double pTol = 1E-8;
      assert(fabs(KBase::sum(cv) - 1.0) < pTol);

      assert(0 < cv.numR());
      assert(1 == cv.numC());

      auto ndxIJ = ndxMaxAbs(cv);
      unsigned int iMax = get<0>(ndxIJ);
      return iMax;
    };

    // what is the utility to actor nai of the state resulting after
    // the nbj-th bargain of the k-th actor is implemented?
    auto brgnUtil = [this, brgns](unsigned int nk, unsigned int nai, unsigned int nbj) {
      const unsigned int na = model->numAct;
      BargainSMP * b = brgns[nk][nbj];
      double uAvrg = 0.0;

      if (nullptr == b) { // SQ bargain
        uAvrg = 0.0;
        for (unsigned int n = 0; n < na; n++) {
          // nai's estimate of the utility to nai of position n, i.e. the true value
          uAvrg = uAvrg + aUtil[nai](nai, n);
        }
      }

      if (nullptr != b) { // all positions unchanged, except Init and Rcvr
        uAvrg = 0.0;
        auto ndxInit = model->actrNdx(b->actInit);
        assert((0 <= ndxInit) && (ndxInit < na)); // must find it
        double uPosInit = ((SMPActor*)(model->actrs[nai]))->posUtil(&(b->posInit), this);
        uAvrg = uAvrg + uPosInit;

        auto ndxRcvr = model->actrNdx(b->actRcvr);
        assert((0 <= ndxRcvr) && (ndxRcvr < na)); // must find it
        double uPosRcvr = ((SMPActor*)(model->actrs[nai]))->posUtil(&(b->posRcvr), this);
        uAvrg = uAvrg + uPosRcvr;

        for (unsigned int n = 0; n < na; n++) {
          if ((ndxInit != n) && (ndxRcvr != n)) {
            // again, nai's estimate of the utility to nai of position n, i.e. the true value
            uAvrg = uAvrg + aUtil[nai](nai, n);
          }
        }
      }

      uAvrg = uAvrg / na;

      assert(0.0 < uAvrg); // none negative, at least own is positive
      assert(uAvrg <= 1.0); // can not all be over 1.0
      return uAvrg;
    };
    // end of λ-fn


    // TODO: finish this
    // For each actor, assess what bargains result from CDMP, and put it into s2

    // The key is to build the usual matrix of U_ai (Brgn_m) for all bargains in brgns[k],
    // making sure to divide the sum of the utilities of positions by 1/N
    // so 0 <= Util(state after Brgn_m) <= 1, then do the standard scalarPCE for bargains involving k.


    SMPState* s2 = new SMPState(model);
    // (This loop would be a good place for high-level parallelism)
    for (unsigned int k = 0; k < na; k++) {
      unsigned int nb = brgns[k].size();
      auto buk = [brgnUtil, k](unsigned int nai, unsigned int nbj) {
        return brgnUtil(k, nai, nbj);
      };
      auto u_im = KMatrix::map(buk, na, nb);

      cout << "u_im: " << endl;
      u_im.mPrintf(" %.5f ");

      cout << "Doing probCE for the " << nb << " bargains of actor " << k << " ... " << flush;
      auto p = Model::scalarPCE(na, nb, w, u_im, vr, vpm, ReportingLevel::Medium);
      assert(nb == p.numR());
      assert(1 == p.numC());
      cout << "done" << endl << flush;
      unsigned int mMax = ndxMaxProb(p); // indexing actors by i, bargains by m
      cout << "Chosen bargain: " << mMax << endl;



      // TODO: create a fresh position for k, from the selected bargain mMax.
      VctrPstn * pk = nullptr;
      auto bkm = brgns[k][mMax];
      if (nullptr == bkm) {
        auto oldPK = (VctrPstn *)pstns[k];
        pk = new VctrPstn(*oldPK);
      }
      else {
        const unsigned int ndxInit = model->actrNdx(bkm->actInit);
        const unsigned int ndxRcvr = model->actrNdx(bkm->actRcvr);
        if (ndxInit == k) {
          pk = new VctrPstn(bkm->posInit);
        }
        else if (ndxRcvr == k) {
          pk = new VctrPstn(bkm->posRcvr);
        }
        else {
          cout << "SMPState::doBCN: unrecognized actor in bargain" << endl;
          assert(false);
        }
      }
      assert(nullptr != pk);

      assert(k == s2->pstns.size());
      s2->pstns.push_back(pk);

      cout << endl << flush;
    }


    // Some bargains are nullptr, and there are two copies of every non-nullptr randomly
    // arranged. If we delete them as we find them, then the second occurance will be corrupted,
    // so the code crashes when it tries to access the memory to see if it matches something
    // already deleted. Hence, we scan them all, building a list of unique bargains,
    // then delete those in order.
    auto uBrgns = vector<BargainSMP*>();

    for (unsigned int i = 0; i < brgns.size(); i++) {
      auto ai = ((const SMPActor*)(model->actrs[i]));
      for (unsigned int j = 0; j < brgns[i].size(); j++) {
        BargainSMP* bij = brgns[i][j];
        if (nullptr != bij) {
          if (ai == bij->actInit) {
            uBrgns.push_back(bij); // this is the initiator's copy, so save it for deletion
          }
        }
        brgns[i][j] = nullptr; // either way, null it out.
      }
    }

    for (auto b : uBrgns) {
      //int aI = model->actrNdx(b->actInit);
      //int aR = model->actrNdx(b->actRcvr);
      //printf("Delete bargain [%2i:%2i] \n", aI, aR);
      delete b;
    }

    // TODO: this really should do all the assessment: ueIndices, rnProb, all U^h_{ij}, raProb
    s2->setUENdx();
    return s2;
  }


  // h's estimate of the victory probability and expected change in utility for k from i challenging j,
  // compared to status quo.
  // Note that the  aUtil vector of KMatrix must be set before starting this.
  // TODO: offer a choice the different ways of estimating value-of-a-state: even sum or expected value.
  // TODO: we may need to separate euConflict from this at some point
  // TODO: add a boolean flag to record this is SQLite, which touches at least three tables
  tuple<double, double> SMPState::probEduChlg(unsigned int h, unsigned int k, unsigned int i, unsigned int j) const {

    // you could make other choices for these two sub-models
    auto vr = VotingRule::Proportional;
    auto tpc = KBase::ThirdPartyCommit::SemiCommit;

    double uii = aUtil[h](i, i);
    double uij = aUtil[h](i, j);
    double uji = aUtil[h](j, i);
    double ujj = aUtil[h](j, j);

    // h's estimate of utility to k of status-quo positions of i and j
    double euSQ = aUtil[h](k, i) + aUtil[h](k, j);
    assert((0.0 <= euSQ) && (euSQ <= 2.0));

    // h's estimate of utility to k of i defeating j, so j adopts i's position
    double uhkij = aUtil[h](k, i) + aUtil[h](k, i);
    assert((0.0 <= uhkij) && (uhkij <= 2.0));

    // h's estimate of utility to k of j defeating i, so i adopts j's position
    double uhkji = aUtil[h](k, j) + aUtil[h](k, j);
    assert((0.0 <= uhkji) && (uhkji <= 2.0));

    auto ai = ((const SMPActor*)(model->actrs[i]));
    double si = KBase::sum(ai->vSal);
    double ci = ai->sCap;
    auto aj = ((const SMPActor*)(model->actrs[j]));
    double sj = KBase::sum(aj->vSal);
    assert((0 < sj) && (sj <= 1));
    double cj = aj->sCap;
    double minCltn = 1E-10;

    // h's estimate of i's unilateral influence contribution to (i:j), hence positive
    double contrib_i_ij = Model::vote(vr, si*ci, uii, uij);
    assert(0 <= contrib_i_ij);
    double chij = minCltn + contrib_i_ij; // strength of complete coalition supporting i over j
    assert(0.0 < chij);

    // h's estimate of j's unilateral influence contribution to (i:j), hence negative
    double contrib_j_ij = Model::vote(vr, sj*cj, uji, ujj);
    assert(contrib_j_ij <= 0);
    double chji = minCltn - contrib_j_ij; // strength of complete coalition supporting j over i
    assert(0.0 < chji);

    const unsigned int na = model->numAct;

    // we assess the overall coalition strengths by adding up the contribution of
    // individual actors (including i and j, above). We assess the contribution of third
    // parties by looking at little coalitions in the hypothetical (in:j) or (i:nj) contests.
    for (unsigned int n = 0; n < na; n++) {
      if ((n != i) && (n != j)) { // already got their influence-contributions
        auto an = ((const SMPActor*)(model->actrs[n]));
        double cn = an->sCap;
        double sn = KBase::sum(an->vSal);
        double uni = aUtil[h](n, i);
        double unj = aUtil[h](n, j);
        double unn = aUtil[h](n, n);

        double pin = Actor::vProbLittle(vr, sn*cn, uni, unj, chij, chji);
        assert(0.0 <= pin);
        assert(pin <= 1.0);
        double pjn = 1.0 - pin;

        double vnij = Actor::thirdPartyVoteSU(sn*cn, vr, tpc, pin, pjn, uni, unj, unn);

        chij = (vnij > 0) ? (chij + vnij) : chij;
        assert(0 < chij);
        chji = (vnij < 0) ? (chji - vnij) : chji;
        assert(0 < chji);
      }
    }

    // UtilContest, ProbVict, UtilChlg
    // UtilSQ, UtilVict
    const double phij = chij / (chij + chji);
    const double phji = chji / (chij + chji);

    const double euCh = (1 - sj)*uhkij + sj*(phij*uhkij + phji*uhkji);
    const double euChlg = euCh - euSQ;
    // printf ("SMPState::probEduChlg(%2i, %2i, %2i, %i2) = %+6.4f - %+6.4f = %+6.4f\n", h, k, i, j, euCh, euSQ, euChlg);
    auto rslt = tuple<double, double>(phij, euChlg);
    return rslt;
  }



  tuple<int, double, double> SMPState::bestChallenge(unsigned int i) const {
    int bestJ = -1;
    double piJ = 0;
    double bestEU = 0;

    // for SMP, positive ej are typically in the 0.5 to 0.01 range, so I take 1/1000 of the minimum,
    const double minSig = 1e-5;

    for (unsigned int j = 0; j < model->numAct; j++) {
      if (j != i) {
        auto pej = probEduChlg(i, i, i, j);
        double pj = get<0>(pej); // i's estimate of the victory-Prob for i challengeing j
        double ej = get<1>(pej); // i's estimate of the change in utility to i of i challengeing j, compared to SQ
        if ((minSig < ej) && (bestEU < ej)) {
          bestJ = j;
          piJ = pj;
          bestEU = ej;
        }
      }
    }
    auto rslt = tuple<int, double, double>(bestJ, piJ, bestEU);
    return rslt;
  }


  tuple< KMatrix, VUI> SMPState::pDist(int persp) const {
    /// Calculate the probability distribution over states from this perspective

    // TODO: convert this to a single, commonly used setup function
    const VotingRule vr = VotingRule::Proportional;
    const ReportingLevel rl = ReportingLevel::Silent;
    const VPModel vpm = VPModel::Linear;

    const unsigned int na = model->numAct;
    const KMatrix w = actrCaps();

    auto uij = KMatrix(na, na); // full utility matrix, including duplicate columns
    assert(na == aUtil.size()); // must have been filled in
    if ((0 <= persp) && (persp < na)) {
      uij = aUtil[persp];
    }
    else if (-1 == persp) {
      for (unsigned int i = 0; i < na; i++) {
        for (unsigned int j = 0; j < na; j++) {
          auto ui = aUtil[i];
          uij(i, j) = aUtil[i](i, j);
        }
      }
    }
    else {
      cout << "SMPState::pDist: unrecognized perspective, " << persp << endl << flush;
      assert(false);
    }

    assert(0 < uIndices.size()); // should have been set with setUENdx();
    //auto uNdx2 = uniqueNdx(); // get the indices to unique positions
    printf("Unique positions %i/%i ", uIndices.size(), na);
    cout << "[ ";
    for (auto i : uIndices) {
      printf(" %i ", i);
    }
    cout << " ] " << endl << flush;
    auto uufn = [uij, this](unsigned int i, unsigned int j) {
      return uij(i, uIndices[j]);
    };
    auto uUij = KMatrix::map(uufn, na, uIndices.size());
    auto upd = Model::scalarPCE(na, uIndices.size(), w, uUij, vr, vpm, rl);

    return tuple< KMatrix, VUI>(upd, uIndices);
  }


  // -------------------------------------------------


  SMPModel::SMPModel(PRNG * r, string desc) : Model(r, desc) {
    // note that numDim, posTol, and dimName are initialized in class declaration

    // TODO: get cleaner opening of smpDB
    sqlTest();
  }

  SMPModel::~SMPModel() {
    // TODO: probably should not close smpDB automatically.
    // With committee selection, we might have dozens of SMP models writing into one database,
    // so we cannot automatically close it when deleting a particular SMP.

    if (nullptr != smpDB) {
      cout << "SMPModel::~SMPModel Closing database" << endl << flush;
      sqlite3_close(smpDB);
      smpDB = nullptr;
    }

  }



  void SMPModel::addDim(string dn) {
    dimName.push_back(dn);
    numDim = dimName.size();
    return;
  }

  double SMPModel::stateDist(const SMPState* s1, const SMPState* s2) {
    unsigned int n = s1->pstns.size();
    assert(n == s2->pstns.size());
    double dSum = 0;
    for (unsigned int i = 0; i < n; i++) {
      auto vp1i = ((const VctrPstn*)(s1->pstns[i]));
      auto vp2i = ((const VctrPstn*)(s2->pstns[i]));
      dSum = dSum + KBase::norm((*vp1i) - (*vp2i));
    }
    return dSum;
  }


  // 0 <= d <= 1 is the difference in normalized position
  // -1 <= R <= +1 is normalized risk-aversion
  double SMPModel::bsUtil(double sd, double R) {
    double u = 0;
    assert(0 <= sd);
    if (sd <= 1) {
      u = (1 - sd)*(1 + sd*R);
    }
    else { // searches and round-off can drive sd > 1, slightly
      u = (1 - sd)*(1 + R);
    }
    return u;
  }

  double SMPModel::bvDiff(const  KMatrix & vd, const  KMatrix & vs) {
    assert(KBase::sameShape(vd, vs));
    double dsSqr = 0;
    double ssSqr = 0;
    for (unsigned int i = 0; i < vd.numR(); i++) {
      for (unsigned int j = 0; j < vd.numC(); j++) {
        const double dij = vd(i, j);
        const double sij = vs(i, j);
        assert(0 <= sij);
        const double ds = dij * sij;
        const double s = sij;
        dsSqr = dsSqr + (ds*ds);
        ssSqr = ssSqr + (s*s);
      }
    }
    assert(0 < ssSqr);
    double sd = sqrt(dsSqr / ssSqr);
    return sd;
  };

  double SMPModel::bvUtil(const  KMatrix & vd, const  KMatrix & vs, double R) {
    const double sd = bvDiff(vd, vs);
    const double u = bsUtil(sd, R);
    return u;
  };

  /// the probability of the position occupied by actor i
  double SMPState::posProb(unsigned int i, const VUI & unq, const KMatrix & pdt) const {
    const unsigned int numA = model->numAct;
    const unsigned int nUnq = unq.size();
    unsigned int k = numA + 1; // impossibly high
    for (unsigned int j1 = 0; j1 < nUnq; j1++) { // scan unique positions
      unsigned int j2 = unq[j1]; // get ordinary index of the position
      if (equivNdx(i, j2)) {
        k = j1;
      }
    }
    assert(k < numA);
    assert(1 == pdt.numC());
    assert(k < pdt.numR());
    assert(nUnq == pdt.numR());
    double pr = pdt(k, 0);
    return pr;
  }


  void SMPModel::sankeyOutput(string inputCSV) const {
    assert(numAct == actrs.size());
    assert(numDim == dimName.size());
    if (1 == numDim) {
      unsigned int nameLen = inputCSV.length();
      cout << endl;
      const char* appendEffPwr = "_effPow.csv";
      char* epName = newChars(nameLen + strlen(appendEffPwr) + 1);
      sprintf(epName, "%s%s", inputCSV.c_str(), appendEffPwr);
      cout << "Record effective power in " << epName << "  ...  " << flush;
      FILE* f1 = fopen(epName, "w");
      for (unsigned int i = 0; i < numAct; i++) {
        auto ai = ((const SMPActor*)actrs[i]);
        double ci = ai->sCap;
        assert(0.0 < ci);
        double si = KBase::sum(ai->vSal);
        assert(0.0 < si);
        assert(si <= 1.0);
        double epi = ci * si;
        fprintf(f1, "%s,%5.1f\n", ai->name.c_str(), epi);
      }
      fclose(f1);
      f1 = nullptr;
      cout << "done" << endl;

      const char* appendPosLog = "_posLog.csv";
      char* plName = newChars(nameLen + strlen(appendPosLog) + 1);
      sprintf(plName, "%s%s", inputCSV.c_str(), appendPosLog);
      cout << "Record 1D positions over time, without dimension-name in " << plName << "  ...  " << flush;
      FILE* f2 = fopen(plName, "w");
      for (unsigned int i = 0; i < numAct; i++) {
        fprintf(f2, "%s", actrs[i]->name.c_str());
        for (unsigned int t = 0; t < history.size(); t++) {
          auto st = history[t];
          auto pit = st->pstns[i];
          auto vpit = (const VctrPstn*)pit;
          assert(1 == vpit->numC());
          assert(numDim == vpit->numR());
          fprintf(f2, ",%5.1f", 100 * (*vpit)(0, 0)); // have to print "100.0" sometimes
        }
        fprintf(f2, "\n");
      }
      fclose(f2);
      f2 = nullptr;
      cout << "done." << endl;
    }
    return;
  }

  void SMPModel::showVPHistory(bool sqlP) const {
    assert(numAct == actrs.size());
    assert(numDim == dimName.size());

    assert(nullptr != smpDB);
    char* zErrMsg = nullptr;
    createSMPTableSQL(0); // VectorPosition
    auto sqlBuff = newChars(200);
    sprintf(sqlBuff,
      "INSERT INTO VectorPosition (Scenario, Turn_t, Act_i, Dim_k, Coord) VALUES ('%s', ?1, ?2, ?3, ?4)",
      scenName.c_str());
    const char* insStr = sqlBuff;
    sqlite3_stmt *insStmt;
    sqlite3_prepare_v2(smpDB, insStr, strlen(insStr), &insStmt, NULL);
    // Prepared statements cache the execution plan for a query after the query optimizer has
    // found the best plan, so there is no big gain with simple insertions.
    // What makes a huge difference is bundling a few hundred into one atomic "transaction".
    // For this case, runtime droped from 62-65 seconds to 0.5-0.6 (vs. 0.30-0.33 with no SQL at all).

    sqlite3_exec(smpDB, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

    // show positions over time
    for (unsigned int i = 0; i < numAct; i++) {
      for (unsigned int k = 0; k < numDim; k++) {
        printf("%s , %s , ", actrs[i]->name.c_str(), dimName[k].c_str());
        for (unsigned int t = 0; t < history.size(); t++) {
          auto st = history[t];
          auto pit = st->pstns[i];
          auto vpit = (const VctrPstn*)pit;
          assert(1 == vpit->numC());
          assert(numDim == vpit->numR());
          printf("%5.1f , ", 100 * (*vpit)(k, 0)); // have to print "100.0" sometimes
          if (sqlP) {
            int rslt = 0;
            rslt = sqlite3_bind_int(insStmt, 1, t);
            assert(SQLITE_OK == rslt);
            rslt = sqlite3_bind_int(insStmt, 2, i);
            assert(SQLITE_OK == rslt);
            rslt = sqlite3_bind_int(insStmt, 3, k);
            assert(SQLITE_OK == rslt);
            const double coord = (*vpit)(k, 0);
            rslt = sqlite3_bind_double(insStmt, 4, coord);
            assert(SQLITE_OK == rslt);
            rslt = sqlite3_step(insStmt);
            assert(SQLITE_DONE == rslt);
            sqlite3_clear_bindings(insStmt);
            assert(SQLITE_DONE == rslt);
            rslt = sqlite3_reset(insStmt);
            assert(SQLITE_OK == rslt);
          }
        }
        cout << endl;
      }
    }

    sqlite3_exec(smpDB, "END TRANSACTION", NULL, NULL, &zErrMsg);
    cout << endl;

    // show probabilities over time.
    // Note that we have to set the aUtil matrices for the last one.
    vector<KMatrix> prbHist = {};
    vector<VUI> unqHist = {};
    for (unsigned int t = 0; t < history.size(); t++) {
      auto sst = (SMPState*)history[t];
      assert(numAct == sst->aUtil.size()); // should be fully initialized
      auto pn = sst->pDist(-1);
      auto pdt = std::get<0>(pn); // note that these are unique positions
      auto unq = std::get<1>(pn);
      prbHist.push_back(pdt);
      unqHist.push_back(unq);
    }

    auto probIT = [this, prbHist, unqHist](unsigned int i, unsigned int t) {
      auto pdt = prbHist[t];
      auto unq = unqHist[t];
      auto sst = ((const SMPState*)(history[t]));
      double pr = sst->posProb(i, unq, pdt);
      return pr;
    };

    // TODO: displaying the probabilities of actors winning is a bit odd,
    // as we display the probability of their position winning. As multiple
    // actors often occupy the equivalent positions, this means the displayed probabilities
    // will often add up to more than 1.
    for (unsigned int i = 0; i < numAct; i++) {
      printf("%s , prob , ", actrs[i]->name.c_str());
      for (unsigned int t = 0; t < history.size(); t++) {
        printf("%.4f , ", probIT(i, t)); //  prbHist[t](i, 0),
      }
      cout << endl << flush;
    }
    return;
  }


  SMPModel * SMPModel::readCSV(string fName, PRNG * rng) {
    using KBase::KException;
    char * errBuff = newChars(100); // as sprintf requires
    csv_parser csv(fName);
    // Get values according to row number and column number.
    // Remember it starts from (1,1) and not (0,0)
    string scenName = csv.get_value(1, 1);
    cout << "Scenario name: |" << scenName << "|" << endl;
    cout << flush;
    string numActorString = csv.get_value(1, 3);
    unsigned int numActor = atoi(numActorString.c_str());
    string numDimString = csv.get_value(1, 4);
    int numDim = atoi(numDimString.c_str());
    printf("Number of actors: %u \n", numActor);
    printf("Number of dimensions: %i \n", numDim);
    cout << endl << flush;

    if (numDim < 1) { // lower limit
      throw(KBase::KException("SMPModel::readCSV: Invalid number of dimensions"));
    }
    assert(0 < numDim);
    if ((numActor < minNumActor) || (maxNumActor < numActor)) { // avoid impossibly low or ridiculously large
      throw(KBase::KException("SMPModel::readCSV: Invalid number of actors"));
    }
    assert(minNumActor <= numActor);
    assert(numActor <= maxNumActor);

    // Read actor data
    auto actorNames = vector<string>();
    auto actorDescs = vector<string>();
    auto cap = KMatrix(numActor, 1);
    auto nra = KMatrix(numActor, 1);
    for (unsigned int i = 0; i < numActor; i++) {
      // get short names
      string nis = csv.get_value(3 + i, 1);
      assert(0 < nis.length());
      actorNames.push_back(nis);
      printf("Actor %3u name: %s \n", i, actorNames[i].c_str());

      // get long descriptions
      string descsi = csv.get_value(3 + i, 2);
      actorDescs.push_back(descsi);
      printf("Actor %3u desc: %s \n", i, actorDescs[i].c_str());

      // get capability/power, often on 0-100 scale
      string psi = csv.get_value(3 + i, 3);
      double pi = atof(psi.c_str());
      printf("Actor %3u power: %5.1f \n", i, pi);
      assert(0 <= pi); // zero weight is pointless, but not incorrect
      assert(pi < 1E8); // no real upper limit, so this is just a sanity-check
      cap(i, 0) = pi;


      cout << endl << flush;

    } // loop over actors, i


    // get issue names
    auto dNames = vector<string>();
    for (unsigned int j = 0; j < numDim; j++) {
      string insi = csv.get_value(2, 4 + 2 * j);
      dNames.push_back(insi);
      printf("Dimension %2u: %s \n", j, dNames[j].c_str());
    }
    cout << endl;

    // get position/salience data
    auto pos = KMatrix(numActor, numDim);
    auto sal = KMatrix(numActor, numDim);
    for (unsigned int i = 0; i < numActor; i++) {
      double salI = 0.0;
      for (unsigned int j = 0; j < numDim; j++) {
        string posSIJ = csv.get_value(3 + i, 4 + 2 * j);
        double posIJ = atof(posSIJ.c_str());
        printf("pos[%3u , %3u] =  %5.3f \n", i, j, posIJ);
        cout << flush;
        if ((posIJ < 0.0) || (+100.0 < posIJ)) { // lower and upper limit
          errBuff = newChars(100);
          sprintf(errBuff, "SMPModel::readCSV: Out-of-bounds position for actor %u on dimension %u:  %f",
            i, j, posIJ);
          throw(KException(errBuff));
        }
        assert(0.0 <= posIJ);
        assert(posIJ <= 100.0);
        pos(i, j) = posIJ;

        string salSIJ = csv.get_value(3 + i, 5 + 2 * j);
        double salIJ = atof(salSIJ.c_str());
        //printf("sal[%3i , %3i] = %5.3f \n", i, j, salIJ);
        //cout << flush;
        if ((salIJ < 0.0) || (+100.0 < salIJ)) { // lower and upper limit
          errBuff = newChars(100);
          sprintf(errBuff, "SMPModel::readCSV: Out-of-bounds salience for actor %u on dimension %u:  %f",
            i, j, salIJ);
          throw(KException(errBuff));
        }
        assert(0.0 <= salIJ);
        salI = salI + salIJ;
        //printf("sal[%3i] = %5.3f \n", i, salI);
        //cout << flush;
        if (+100.0 < salI) { // upper limit: no more than 100% of attention to all issues
          errBuff = newChars(100);
          sprintf(errBuff,
            "SMPModel::readCSV: Out-of-bounds total salience for actor %u:  %f",
            i, salI);
          throw(KException(errBuff));
        }
        assert(salI <= 100.0);
        sal(i, j) = (double)salIJ;
        //cout << endl << flush;
      }
    }

    cout << "Position matrix:" << endl;
    pos.mPrintf("%5.1f  ");
    cout << endl << endl << flush;
    cout << "Salience matrix:" << endl;
    sal.mPrintf("%5.1f  ");
    cout << endl << flush;

    // get them into the proper internal scale:
    pos = pos / 100.0;
    sal = sal / 100.0;

    // now that it is read and verified, use the data
    auto sm0 = SMPModel::initModel(actorNames, actorDescs, dNames, cap, pos, sal, rng);
    return sm0;
  }



  SMPModel * SMPModel::initModel(vector<string> aName, vector<string> aDesc, vector<string> dName,
    KMatrix cap, KMatrix pos, KMatrix sal, PRNG * rng) {
    SMPModel * sm0 = new SMPModel(rng);
    SMPState * st0 = new SMPState(sm0);
    st0->step = [st0]() {
      return st0->stepBCN();
    };
    sm0->addState(st0);


    const unsigned int na = aName.size();
    const unsigned int nd = dName.size();


    for (auto dn : dName) {
      sm0->addDim(dn);
    }

    for (unsigned int i = 0; i < na; i++) {
      auto ai = new SMPActor(aName[i], aDesc[i]);
      ai->sCap = cap(i, 0);
      ai->vSal = KMatrix(nd, 1);
      auto vpi = new VctrPstn(nd, 1);
      for (unsigned int j = 0; j < nd; j++) {
        ai->vSal(j, 0) = sal(i, j);
        (*vpi)(j, 0) = pos(i, j);
      }
      sm0->addActor(ai);
      st0->addPstn(vpi);
    }

    return sm0;
  }



}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
