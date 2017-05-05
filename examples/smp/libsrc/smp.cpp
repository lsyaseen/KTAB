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

namespace SMPLib {
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
using KBase::StateTransMode;
using KBase::VotingRule;
using KBase::PCEModel;
using KBase::ReportingLevel;


// --------------------------------------------

SMPModel * md0 = nullptr;

std::vector<string> SMPModel::fieldVals;
std::vector<string> SMPModel::dbFieldVals;

// big enough buffer to build all desired SQLite statements
const unsigned int sqlBuffSize = 250;

// --------------------------------------------

// this binds the given parameters and returns the λ-fn necessary to stop the SMP appropriately
function<bool(unsigned int, const State *)>
smpStopFn(unsigned int minIter, unsigned int maxIter, double minDeltaRatio, double minSigDelta) {
    auto  sfn = [minIter, maxIter, minDeltaRatio, minSigDelta](unsigned int iter, const State * s) {
        bool tooLong = (maxIter <= iter);
        bool longEnough = (minIter <= iter);
        bool quiet = false;
        auto sf = [](unsigned int i1, unsigned int i2, double d12) {
            LOG(DEBUG) << KBase::getFormattedString(
              "sDist [%2i,%2i] = %.2E   ", i1, i2, d12);
            return;
        };
        auto s0 = ((const SMPState*)(s->model->history[0]));
        auto s1 = ((const SMPState*)(s->model->history[1]));
        auto d01 = SMPModel::stateDist(s0, s1) + minSigDelta;
        sf(0, 1, d01);
        auto sx = ((const SMPState*)(s->model->history[iter - 0]));
        auto sy = ((const SMPState*)(s->model->history[iter - 1]));
        auto dxy = SMPModel::stateDist(sx, sy);
        sf(iter - 1, iter - 0, dxy);
        const double aRatio = dxy / d01;
        quiet = (aRatio < minDeltaRatio);
        LOG(DEBUG) << KBase::getFormattedString(
          "Fractional change compared to first step: %.4f  (target=%.4f)",
          aRatio, minDeltaRatio);
        return tooLong || (longEnough && quiet);
    };
    return sfn;
};

// --------------------------------------------

SMPActor::SMPActor(string n, string d) : Actor(n, d) {
    vr = VotingRule::Proportional; // just a default
}

SMPActor::~SMPActor() {
}

double SMPActor::vote(unsigned int est, unsigned int i, unsigned int j, const State*st) const {
    unsigned int k = st->model->actrNdx(this);
    auto uk = st->aUtil[est];
    double uhki = uk(k, i);
    double uhkj = uk(k, j);
    const double vij = Model::vote(vr, sCap, uhki, uhkj);
    return vij;
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
    const VctrPstn actorIdeal = as->getIdeal(ai);
    const VctrPstn* p0 = &actorIdeal;
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

    // Note that we randomly assign different voting rules
    //vr = VotingRule::Proportional;
    const unsigned int numVR = KBase::VotingRuleNames.size();
    const unsigned int vrNum = ((unsigned int)(rng->uniform(0.0, numVR -0.01)));
    vr = VotingRule(vrNum);
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



SMPState::SMPState(Model * m) : State(m), turn(m->history.size()) {
}


SMPState::~SMPState() {
    nra = KMatrix();
    ideals = {};
    accomodate = KMatrix();
}

void SMPState::setVDiff(const vector<VctrPstn> & vPos) {
    auto dfn = [vPos, this](unsigned int i, unsigned int j) {
        auto ai = ((const SMPActor*)(model->actrs[i]));
        KMatrix si = ai->vSal;
        auto posJ = ((const VctrPstn*)(pstns[j]));
        double dij = 0.0;
        if (0 == vPos.size()) {
            auto posI = ((const VctrPstn*)(pstns[i]));
            auto idlI = ideals[i];
            //dij = SMPModel::bvDiff((*posI) - (*posJ), si);
            dij = SMPModel::bvDiff(idlI - (*posJ), si);
        }
        else {
            auto vpi = vPos[i];
            dij = SMPModel::bvDiff(vpi - (*posJ), si);
        }
        return dij;
    };

    const unsigned int na = model->numAct;
    assert(na == ideals.size());
    assert(na == accomodate.numR());
    assert(na == accomodate.numC());
    vDiff = KMatrix::map(dfn, na, na);
    return;
}

/*
void SMPModel::setUtilProb(const KMatrix& vR, const KMatrix& vS, const KMatrix& vD,
  KBase::VotingRule vr, KBase::VPModel vpm) {
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
  //auto vpm = VPModel::Linear;
  auto vP = KMatrix(na, na);
  for (unsigned int i = 0; i < na; i++) {
    for (unsigned int j = 0; j < i; j++) {
      //double pij = cs(i, j) / (cs(i, j) + cs(j, i)); // was never used
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

*/

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
    const auto vpmCoalition = model->vpm;
    const unsigned int na = model->numAct;
    auto smod = (const SMPModel*)model;
    const auto vrCoalition = smod->vrCltn;
    const auto ra = smod->bigRAdj;
    const auto rr = smod->bigRRng;

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
        LOG(DEBUG) << "Raw actor-pos value matrix (risk neutral)";
        rnUtil_ij.mPrintf(" %+.3f ");
    }


    auto vfn = [vrCoalition, &w_j, &rnUtil_ij](unsigned int k, unsigned int i, unsigned int j) {
        double vkij = Model::vote(vrCoalition, w_j(0, k), rnUtil_ij(k, i), rnUtil_ij(k, j));
        return vkij;
    };
    const auto c = Model::coalitions(vfn, na, na); // c(i,j) = strength of coaltion for i against j
    const auto pv2 = Model::probCE2(model->pcem, vpmCoalition, c);
    const auto p_i = get<0>(pv2); // column
    const auto pv_ij = get<1>(pv2); // square
    nra = Model::bigRfromProb(p_i, rr);

    if (ReportingLevel::Silent < rl) {
        LOG(DEBUG) << "Inferred risk attitudes:";
        nra.mPrintf(" %+.3f ");
    }

    auto raUtil_ij = KMatrix::map(uFn1, na, na);

    if (ReportingLevel::Silent < rl) {
        LOG(DEBUG) << "Risk-aware actor-pos utility matrix (objective):";
        raUtil_ij.mPrintf(" %+.4f ");
        LOG(DEBUG) << "RMS change in value vs utility: " << norm(rnUtil_ij - raUtil_ij) / na;
    }

    const double duTol = 1E-6;
    assert(duTol < norm(rnUtil_ij - raUtil_ij)); // I've never seen it below 0.07


    if (ReportingLevel::Silent < rl) {
        switch (ra) {
        case BigRAdjust::FullRA:
            LOG(DEBUG) << "Using" << ra << ": r^h_i = ri";
            break;
        case BigRAdjust::TwoThirdsRA:
            LOG(DEBUG) << "Using" << ra << ": r^h_i = (rh + 2*ri)/3";
            break;
        case BigRAdjust::HalfRA:
            LOG(DEBUG) << "Using" << ra << ": r^h_i = (rh + ri)/2";
            break;
        case BigRAdjust::OneThirdRA:
            LOG(DEBUG) << "Using" << ra << ": r^h_i = (2*rh + ri)/3";
            break;
        case BigRAdjust::NoRA:
            LOG(DEBUG) << "Using" << ra << ": r^h_i = rh ";
            break;
        default:
            LOG(ERROR) << "Unrecognized BigRAdjust";
            exit(-1);
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
            LOG(DEBUG) << "Estimate by" << h << "of risk-aware utility matrix:";
            u_h_ij.mPrintf(" %+.4f ");

            LOG(DEBUG) << "RMS change in util^h vs utility:" << norm(u_h_ij - raUtil_ij) / na;
        }

        assert(duTol < norm(u_h_ij - raUtil_ij)); // I've never seen it below 0.03
    }
    return;
}


void SMPState::setOneAUtil(unsigned int perspH, ReportingLevel rl) {
    LOG(DEBUG) << "SMPState::setOneAUtil - not yet implemented";


    return;
}

void SMPState::showBargains(const vector < vector < BargainSMP* > > & brgns) const {
    string msg = "Bargains involving actor %2u: ";
    string brgnFormat = "[%llu, %u:%u]"; // [bargainID, initAct:recvAct]
    string actorBargains;

    auto printOneBargain = [this, &brgns, &actorBargains, &brgnFormat](unsigned int i, unsigned int j) {
      BargainSMP* bij = brgns[i][j];
      assert(nullptr != bij);
      actorBargains += 
        KBase::getFormattedString
        (
          brgnFormat.c_str(),
          bij->getID(),
          model->actrNdx(bij->actInit),
          model->actrNdx(bij->actRcvr)
        );
    };

    for (unsigned int i = 0; i < brgns.size(); i++) {
        actorBargains += KBase::getFormattedString(msg.c_str(), i);
        for (unsigned int j = 0; j < brgns[i].size(); j++) {
            printOneBargain(i, j);
        }
        LOG(DEBUG) << actorBargains;
        actorBargains.clear();
    }
    return;
}

string SMPState::showOneBargain(const BargainSMP* b) const {
    assert(nullptr != b);
    unsigned int ai = model->actrNdx(b->actInit);
    unsigned int aj = model->actrNdx(b->actRcvr);
    uint64_t bid = b->getID();
    string bargain = KBase::getFormattedString("[%llu, %u:%u]", bid, ai, aj);
    return bargain;
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

void  SMPState::setAccomodate(const KMatrix & aMat) {
    const unsigned int na = model->numAct;
    assert(Model::minNumActor <= na);
    assert(na <= Model::maxNumActor);
    assert(na == aMat.numR());
    assert(na == aMat.numC());
    accomodate = aMat;
    identAccMat = KBase::iMatP(accomodate);
    return;
}

VctrPstn SMPState::getIdeal(unsigned int n) const
{
    return ideals[n];
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

    // JAH 20160802 toggle population of PosUtil, PosEquiv, PosVote, and PosBrob
    // en masse based on value at index 1 of the sqlFlags vector
    // VectorPosition, which is in this same group, is handled separately
    if (model->sqlFlags[1])
    {
        model->sqlPosEquiv(turn);
        model->sqlPosProb(turn);
        model->sqlPosVote(turn);
    }
    // That gets recorded upon the next state - but it
    // therefore misses the very last state.
    auto s2 = doBCN();
    gSetup(s2);
    s2->step = [s2]() {
        return s2->stepBCN();
    };
    return s2;
}

void SMPState::newIdeals() {
    const unsigned int na = model->numAct;
    const double tol = 1E-10;

    assert(Model::minNumActor <= na);
    assert(na <= Model::maxNumActor);
    assert(na == accomodate.numC());
    assert(na == accomodate.numR());
    assert(na == ((unsigned int)(ideals.size())));

    const bool identP = (KBase::norm(accomodate - KBase::iMat(na)) < tol);

    const unsigned int nDim = ((SMPModel*)model)->numDim;

    vector<VctrPstn> nIdeals = {};

    auto posK = [this](unsigned int k) {
        auto ppK = ((const VctrPstn*)(pstns[k]));
        const KMatrix pK = KMatrix(*ppK);
        return pK;
    };

    for (unsigned int i = 0; i < na; i++) {
        double si = 0.0;
        auto pI = posK(i);
        auto newIP = KMatrix(nDim, 1); // new ideal point
        for (unsigned int j = 0; j < na; j++) {
            const double aij = accomodate(i, j); // save typing
            assert(0 <= aij);
            assert(aij <= 1.0);
            si = si + aij;
            assert(si <= 1.0 + tol); // cannot be more than slightly above at any point
            auto pJ = posK(j);
            newIP = newIP + (aij * pJ);

            // very temporary!!
            if (identP && (i == j)) {
                assert(fabs(aij - 1.0) < tol);
            }
        }
        si = (1.0 < si) ? 1.0 : si; // clip to 1, if slightly above
        double lagI = 1.0 - si;
        assert(0.0 <= lagI);
        assert(lagI <= 1.0);
        if (identP) {
            assert(fabs(lagI) < tol);
        }
        newIP = newIP + (lagI * ideals[i]);
        if (identP) {
            assert(KBase::norm(newIP - pI) < tol);
        }
        nIdeals.push_back(VctrPstn(newIP));
    }

    ideals = nIdeals;

    if (identP) {
        assert(posIdealDist() < tol);
    }

    return;
}

void SMPState::idealsFromPstns(const vector<VctrPstn> &  ps) {
    const unsigned int na = model->numAct;
    assert(Model::minNumActor <= na);
    assert(na <= Model::maxNumActor);

    const bool givenP = (na == ps.size());
    assert(givenP || (0 == ps.size()));

    ideals = {};

    for (unsigned int i = 0; i < na; i++) {
        if (givenP) {
            ideals.push_back(ps[i]);
        }
        else {
            auto ppJ = ((const VctrPstn*)(pstns[i]));
            auto newIP = VctrPstn(*ppJ);
            ideals.push_back(VctrPstn(newIP));
        }
    }

    return;
}

double SMPState::posIdealDist(ReportingLevel rl) const {
    const unsigned int t = 0; // myTurn();
    double rmsDist = 0.0;
    const unsigned int na = model->numAct;
    assert(na == pstns.size());
    assert(na == ideals.size());
    for (unsigned int i = 0; i < na; i++) {
        auto ppI = ((const VctrPstn*)(pstns[i]));
        const KMatrix pI = KMatrix(*ppI);
        auto iI = ideals[i];

        if (rl > ReportingLevel::Low) {
            LOG(DEBUG) << "postn" << i << "," << t << ":";
            (trans(pI) * 100.0).mPrintf(" %.4f "); // print on the scale of [0,100]
            LOG(DEBUG) << "ideal" << i << "," << t << ":";
            (trans(iI) * 100.0).mPrintf(" %.4f "); // print on the scale of [0,100]
        }
        double dI = KBase::norm(pI - iI);
        if (rl > ReportingLevel::Silent) {
            // print on the scale of [0,100]
            LOG(DEBUG) << KBase::getFormattedString(
              "postn-ideal distance %2u, %2u: %.5f", i, t, dI * 100.0);
        }
        rmsDist = rmsDist + (dI*dI);
    }
    rmsDist = rmsDist / ((double)na);
    rmsDist = sqrt(rmsDist);
    if (rl > ReportingLevel::Silent) {
        LOG(DEBUG) << KBase::getFormattedString(
          "postn-ideal distance RMS %2u: %.5f", t, rmsDist);
    }
    return rmsDist;
}

void SMPState::setAccomodate(double adjRate) {

    // a man's gotta know his limits
    // (with apologies to HC)
    assert(0.0 <= adjRate);
    assert(adjRate <= 1.0);
    const unsigned int na = model->numAct;

    LOG(DEBUG) << KBase::getFormattedString(
      "Setting SMPState::accomodate to %.3f * identity matrix", adjRate);

    // A standard Identity matrix is helpful here because it
    // should keep the behavior same as the original "cynical" model:
    //      ideal_{i,t} := pstn_{i,t}
    auto am = adjRate * KBase::iMat(na);
    setAccomodate(am);
    return;
}

tuple< KMatrix, VUI> SMPState::pDist(int persp) const {
    /// Calculate the probability distribution over states from this perspective

    // TODO: convert this to a single, commonly used setup function
    auto smod = (const SMPModel*)model;
    const VotingRule vr = smod->vrCltn;
    const ReportingLevel rl = ReportingLevel::Silent;

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
        LOG(ERROR) << "SMPState::pDist: unrecognized perspective," << persp;
        exit(-1);
    }

    assert(0 < uIndices.size()); // should have been set with setUENdx();
    if (ReportingLevel::Silent < rl) {
        string logMsg = "Unique positions ";
        logMsg += std::to_string(uIndices.size()) + "/" + std::to_string(na);
        logMsg += " [";
        for (auto i : uIndices) {
            logMsg += string(" ") + std::to_string(i);
        }
        logMsg += " ]";
        LOG(DEBUG) << logMsg;
    }
    auto uufn = [uij, this](unsigned int i, unsigned int j) {
        return uij(i, uIndices[j]);
    };
    auto uUij = KMatrix::map(uufn, na, uIndices.size());
    auto upd = Model::scalarPCE(na, uIndices.size(), w, uUij, vr, model->vpm, model->pcem, rl);

    return tuple< KMatrix, VUI>(upd, uIndices);
}

KMatrix SMPState::getAccomodate() {
    return accomodate;
}

// -------------------------------------------------

string SMPModel::dbPath = ""; 

// JAH 20160711 added rng seed
SMPModel::SMPModel(string desc, uint64_t s, vector<bool> f, string sceName) : Model(desc, s, f, sceName) {
    // note that numDim, posTol, and dimName are initialized in class declaration
    // TODO: get cleaner opening of smpDB
    sqlTest();
}

SMPModel::~SMPModel() {
    // TODO: probably should not close smpDB automatically.
    // With committee selection, we might have dozens of SMP models writing into one database,
    // so we cannot automatically close it when deleting a particular SMP.

    releaseDB();
}

void SMPModel::releaseDB() {
    if (nullptr != smpDB) {
        LOG(DEBUG) << "SMPModel: Closing database";
        int close_result = sqlite3_close(smpDB);
        if (close_result != SQLITE_OK) {
            LOG(ERROR) << "SMPModel: Closing database failed!";
            exit(-1);
        }
        else {
            LOG(DEBUG) << "SMPModel: Closing database succeeded.";
        }
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

void SMPModel::sankeyOutput(string outputFile) const {
    assert(numAct == actrs.size());
    assert(numDim == dimName.size());

    // first prepare the header line
    char* headLine = newChars(300);
    sprintf(headLine,"PRNG Seed:%20llu;VictoryProbModel:%s;VotingRule:%s;PCEModel:%s;StateTransitions:%s;BigRRange:%s;BigRAdjust:%s;ThirdPartyCommit:%s;InterVecBrgn:%s;BargnModel:%s",
            getSeed(),KBase::nameFromEnum<VPModel>(vpm,KBase::VPModelNames).c_str(),
            KBase::nameFromEnum<VotingRule>(vrCltn,KBase::VotingRuleNames).c_str(),
            KBase::nameFromEnum<PCEModel>(pcem,KBase::PCEModelNames).c_str(),
            KBase::nameFromEnum<StateTransMode>(stm,KBase::StateTransModeNames).c_str(),
            KBase::nameFromEnum<BigRRange>(bigRRng,KBase::BigRRangeNames).c_str(),
            KBase::nameFromEnum<BigRAdjust>(bigRAdj,KBase::BigRAdjustNames).c_str(),
            KBase::nameFromEnum<ThirdPartyCommit>(tpCommit,KBase::ThirdPartyCommitNames).c_str(),
            KBase::nameFromEnum<InterVecBrgn>(ivBrgn,InterVecBrgnNames).c_str(),
            KBase::nameFromEnum<SMPBargnModel>(brgnMod,SMPBargnModelNames).c_str());

    unsigned int nameLen = outputFile.length();
    const char* appendEffPwr = "_effPow.csv";
    char* epName = newChars(nameLen + strlen(appendEffPwr) + 1);
    sprintf(epName, "%s%s", outputFile.c_str(), appendEffPwr);
    LOG(DEBUG) << "Record effective power in" << epName << "...";
    FILE* f1 = fopen(epName, "w");
    fprintf(f1,"%s\n",headLine);
    for (unsigned int i = 0; i < numAct; i++) {
        auto ai = ((const SMPActor*)actrs[i]);
        double ci = ai->sCap;
        assert(0.0 < ci);
        fprintf(f1, "%s", ai->name.c_str());
        // loop through dimensions now
        for (unsigned int k = 0; k < numDim; k++) {
            double si = (ai->vSal)(k, 0);
            assert(0.0 < si);
            assert(si <= 1.0);
            double epi = ci * si;
            // increased precision since we divided by 100 when the saliences were import
            fprintf(f1, ",%5.2f", epi);
        }
        fprintf(f1, "\n");
    }
    fclose(f1);
    f1 = nullptr;
    LOG(DEBUG) << "done";
    delete epName;
    epName = nullptr;

    const char* appendPosLog = "_posLog.csv";
    char* plName = newChars(nameLen + strlen(appendPosLog) + 1);
    sprintf(plName, "%s%s", outputFile.c_str(), appendPosLog);
    LOG(DEBUG) << "Record 1D positions over time, without dimension-name in" << plName << "...";
    FILE* f2 = fopen(plName, "w");
    fprintf(f2,"%s\n",headLine);
    for (unsigned int i = 0; i < numAct; i++) {
        fprintf(f2, "%s", actrs[i]->name.c_str());
        for (unsigned int k = 0; k<numDim; k++) {
            for (unsigned int t = 0; t < history.size(); t++) {
                auto st = history[t];
                auto pit = st->pstns[i];
                auto vpit = (const VctrPstn*)pit;
                assert(numDim == vpit->numR());
                fprintf(f2, ",%5.2f", 100 * (*vpit)(k, 0)); // have to print "100.0" sometimes
            }
        }
        fprintf(f2, "\n");
    }
    fclose(f2);
    f2 = nullptr;
    LOG(DEBUG) << "done";
    delete plName;
    plName = nullptr;
    delete headLine;
    headLine = nullptr;

    return;
}

void SMPModel::sankeyOutput(string outputFile, string dbName, string scenarioId)
{
    sqlite3 *db = nullptr;
    char* zErrMsg = nullptr;
    if (sqlite3_open_v2(dbName.c_str(), &db, SQLITE_OPEN_READONLY, NULL)) {
        LOG(ERROR) << "Failed try to open db file :" << dbName;
        LOG(ERROR) << "SQLite Error:" << sqlite3_errmsg(db);

        sqlite3_close(db);
        exit(-1);
    }

    auto sqlExec = [&db, &zErrMsg](string sqlQry)
    {
        int rc = sqlite3_exec(db, sqlQry.c_str(), sankeyCallBack, nullptr, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            LOG(ERROR) << "SQL Error:" << zErrMsg;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
        }
        return rc;
    };

    auto clearDBFields = [](){
        dbFieldVals.clear();
        assert(dbFieldVals.empty() == true);
    };

    auto getActorNames = [](string scenarioID) {
        string query = "SELECT Name FROM ActorDescription WHERE ScenarioId = \'" + scenarioID + "\'";
        return query;
    };

    auto getDimensionsCount = [](string scenarioID) {
        string query = "SELECT Desc from DimensionDescription WHERE ScenarioId = \'" + scenarioID + "\'";
        return query;
    };

    auto getStateCount = [](string scenarioID) {
        string query = "SELECT DISTINCT Turn_t from VectorPosition WHERE ScenarioId = \'" + scenarioID + "\'";
        return query;
    };

    auto getSalienceVal = [](int turn, int dim, string scenarioID) {
        string query = string("SELECT SpatialSalience.Sal from SpatialSalience, ActorDescription WHERE ") +
                " ActorDescription.Act_i = SpatialSalience.Act_i " +
                " and SpatialSalience.ScenarioId = \'" + scenarioID + "\'" +
                " and ActorDescription.ScenarioId = \'" + scenarioID + "\'" +
                " and SpatialSalience.Turn_t = " + std::to_string(turn) +
                " and SpatialSalience.dim_k = " + std::to_string(dim)  ;

        return query;
    };

    vector<vector<double>> salienceData;
    auto saveSalienceValues = [&salienceData](int dim) {
        salienceData.push_back(vector<double>());

        for (int actIndx =0 ; actIndx < dbFieldVals.size(); ++actIndx)
        {
            salienceData[dim].push_back(std::stod(dbFieldVals.at(actIndx)));
        }
    };

    auto getVectorPosition = [](int turn, int dim, string scenarioID) {
        string query = string("SELECT Pos_Coord from VectorPosition WHERE ") +
                " Dim_k = "+ std::to_string(dim) + " and Turn_t = " + std::to_string(turn)+
                " and  ScenarioId = +\'" + scenarioID + "\'";

        return query;
    };

    vector<vector<double>> vectorPositionDataDim0;
    vector<vector<double>> vectorPositionDataDim1;
    vector<vector<double>> vectorPositionDataDim2;

    auto saveVectorPositionValues = [&vectorPositionDataDim0,
            &vectorPositionDataDim1,&vectorPositionDataDim2](int turn, int dim) {

        if(dim==0)
            vectorPositionDataDim0.push_back(vector<double>());
        else if(dim==1)
            vectorPositionDataDim1.push_back(vector<double>());
        else if (dim==2)
            vectorPositionDataDim2.push_back(vector<double>());

        for (int actIndx =0 ; actIndx < dbFieldVals.size(); ++actIndx)
        {
            if(dim==0)
                vectorPositionDataDim0[turn].push_back(std::stod(dbFieldVals.at(actIndx)));
            else if(dim==1)
                vectorPositionDataDim1[turn].push_back(std::stod(dbFieldVals.at(actIndx)));
            else if(dim==2)
                vectorPositionDataDim2[turn].push_back(std::stod(dbFieldVals.at(actIndx)));
        }
    };

    auto getScenarioData = [](string scenarioID) {
        string query = string("SELECT * from ScenarioDesc WHERE ") +
                " ScenarioId = +\'" + scenarioID + "\'";
        return query;
    };

    vector<string > scenarioData; // 3D
    auto saveScenarioColData = [&scenarioData]() {
        for (int scenarioCol =0 ; scenarioCol < dbFieldVals.size(); ++scenarioCol)
        {
            scenarioData.push_back(dbFieldVals.at(scenarioCol));
        }
    };

    auto getSpatialCapabilityData = [](string scenarioID) {
        string query = string("SELECT Cap from SpatialCapability WHERE ") +
                " ScenarioId = +\'" + scenarioID + "\'";
        return query;
    };

    vector<double> capabilityData; // 3D
    auto saveSpataialCapData = [&capabilityData]() {
        for (int actor =0 ; actor < dbFieldVals.size(); ++actor)
        {
            capabilityData.push_back(std::stod(dbFieldVals.at(actor)));
        }
    };

    clearDBFields();
    int rc = sqlExec(getActorNames(scenarioId));
    assert(SQLITE_OK == rc);
    vector <string> actNames;
    int numActors = dbFieldVals.size();
    for(int actIndx=0; actIndx < dbFieldVals.size(); ++actIndx)
    {
        actNames.push_back(dbFieldVals.at(actIndx));
    }

    clearDBFields();
    rc = sqlExec(getDimensionsCount(scenarioId));
    assert(SQLITE_OK == rc);
    int numDims = dbFieldVals.size();

    clearDBFields();
    rc = sqlExec(getStateCount(scenarioId));
    assert(SQLITE_OK == rc);
    int numTurns = dbFieldVals.size();

    clearDBFields();
    for(int dims =0 ; dims <numDims; ++dims)
    {
        rc = sqlExec(getSalienceVal(0,dims, scenarioId)); // 0 is turn, since values are identical, 0 is dimension
        assert(SQLITE_OK == rc);

        saveSalienceValues(dims);
        clearDBFields();
    }

    clearDBFields();
    for(int turns = 0 ; turns < numTurns ; ++turns)
    {
        for(int dims =0 ; dims <numDims; ++dims)
        {
            rc = sqlExec(getVectorPosition(turns,dims, scenarioId)); // turn, dimension, scenario
            assert(SQLITE_OK == rc);

            saveVectorPositionValues(turns,dims);
            clearDBFields();
        }
    }

    clearDBFields();
    rc = sqlExec(getScenarioData(scenarioId));
    assert(SQLITE_OK == rc);
    saveScenarioColData();

    clearDBFields();
    rc = sqlExec(getSpatialCapabilityData(scenarioId));
    assert(SQLITE_OK == rc);
    saveSpataialCapData();

    sqlite3_close_v2(db);

    // first prepare the header line
    char* headLine = newChars(300);
    sprintf(headLine,
            "PRNG Seed:%s;VictoryProbModel:%s;VotingRule:%s;PCEModel:%s;StateTransitions:%s;BigRRange:%s;BigRAdjust:%s;ThirdPartyCommit:%s;InterVecBrgn:%s;BargnModel:%s",
            scenarioData.at(3).c_str(),
            KBase::VPModelNames[std::stoi(scenarioData.at(4))].c_str(),
            KBase::VotingRuleNames[std::stoi(scenarioData.at(7))].c_str(),
            KBase::PCEModelNames[std::stoi(scenarioData.at(5))].c_str(),
            KBase::StateTransModeNames[std::stoi(scenarioData.at(6))].c_str(),
            KBase::BigRRangeNames[std::stoi(scenarioData.at(9))].c_str(),
            KBase::BigRAdjustNames[std::stoi(scenarioData.at(8))].c_str(),
            KBase::ThirdPartyCommitNames[std::stoi(scenarioData.at(10))].c_str(),
            InterVecBrgnNames[std::stoi(scenarioData.at(11))].c_str(),
            SMPBargnModelNames[std::stoi(scenarioData.at(12))].c_str());

    unsigned int nameLen = outputFile.length();
    const char* appendEffPwr = "_effPow.csv";
    char* epName = newChars(nameLen + strlen(appendEffPwr) + 1);
    sprintf(epName, "%s%s", outputFile.c_str(), appendEffPwr);
    LOG(DEBUG) << "Record effective power in " << epName;
    FILE* f1 = fopen(epName, "w");
    fprintf(f1,"%s\n",headLine);

    for (unsigned int actor = 0; actor < numActors; actor++)
    {
        fprintf(f1, "%s", actNames[actor].c_str());
        // loop through dimensions now
        for (unsigned int dimension = 0; dimension < numDims; dimension++)
        {
            // increased precision since we divided by 100 when the saliences were import
            fprintf(f1, ",%5.2f", salienceData[dimension].at(actor) * capabilityData.at(actor));
        }
        fprintf(f1, "\n");
    }
    fclose(f1);
    f1 = nullptr;
    LOG(DEBUG) << "done";
    delete epName;
    epName = nullptr;

    const char* appendPosLog = "_posLog.csv";
    char* plName = newChars(nameLen + strlen(appendPosLog) + 1);
    sprintf(plName, "%s%s", outputFile.c_str(), appendPosLog);
    LOG(DEBUG) << "Record 1D positions over time, without dimension-name in " << plName;
    FILE* f2 = fopen(plName, "w");
    fprintf(f2,"%s\n",headLine);

    for (unsigned int actor = 0; actor < numActors; actor++)
    {
        fprintf(f2, "%s", actNames[actor].c_str());
        for (unsigned int dimension = 0; dimension<numDims; dimension++)
        {
            for(unsigned int turn =0 ; turn < numTurns; ++turn)
            {
                if(dimension==0)
                    fprintf(f2, ",%5.2f",vectorPositionDataDim0[turn].at(actor));
                else if(dimension==1)
                    fprintf(f2, ",%5.2f",vectorPositionDataDim1[turn].at(actor));
                else if(dimension==2)
                    fprintf(f2, ",%5.2f",vectorPositionDataDim2[turn].at(actor));
            }
        }
        fprintf(f2, "\n");
    }
    fclose(f2);
    f2 = nullptr;
    LOG(DEBUG) << "done";
    delete plName;
    plName = nullptr;
    delete headLine;
    headLine = nullptr;

    return;

}

// JAH 20160801 changed to refer to model sqlFlags vector to decide
// whether or not to populate the table
void SMPModel::showVPHistory() const {
    assert(numAct == actrs.size());
    assert(numDim == dimName.size());

    // first need to get the group ID for this table
    // so then we can get the flag to populate the table or not
    // note the implicit assumption that there will never be 43+ groups :-)
    unsigned int grpID = 42;
    for (unsigned int t = 0; t<KTables.size(); t++)
    {
        if (KTables[t]->tabName == "VectorPosition")
        {
            grpID = KTables[t]->tabGrpID;
            break;
        }
    }
    // be sure that it found this table
    assert(grpID != 42);
    assert(grpID < sqlFlags.size());

    // JAH 20160801 only populate the table if this group is turned on
    if (sqlFlags[grpID])
    {
        assert(nullptr != smpDB);
        char* zErrMsg = nullptr;

        //createSQL(Model::NumTables + 0); // Make sure VectorPosition table is present
        auto sqlBuff = newChars(sqlBuffSize);
        sprintf(sqlBuff,
          "INSERT INTO VectorPosition "
          "(ScenarioId, Turn_t, Act_i, Dim_k, Pos_Coord, Idl_Coord, Mover_BargnId)"
          "VALUES ('%s', ?1, ?2, ?3, ?4, ?5, ?6)", scenId.c_str());

        assert(nullptr != smpDB);
        const char* insStr = sqlBuff;
        sqlite3_stmt *insStmt;
        sqlite3_prepare_v2(smpDB, insStr, strlen(insStr), &insStmt, NULL);
        assert(nullptr != insStmt); //make sure it is ready

        // Prepared statements cache the execution plan for a query after the query optimizer has
        // found the best plan, so there is no big gain with simple insertions.
        // What makes a huge difference is bundling a few hundred into one atomic "transaction".
        // For this case, runtime droped from 62-65 seconds to 0.5-0.6 (vs. 0.30-0.33 with no SQL at all).

        sqlite3_exec(smpDB, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

        LOG(DEBUG) << "History of actor positions over time:";
        string actorPosHistory;

        // show positions over time
        for (unsigned int i = 0; i < numAct; i++) {
            for (unsigned int k = 0; k < numDim; k++) {
                actorPosHistory += actrs[i]->name + ", " + dimName[k] + ":";
                for (unsigned int t = 0; t < history.size(); t++) {
                    auto st = history[t];
                    auto pit = st->pstns[i];
                    auto vpit = (const VctrPstn*)pit;
                    auto sst = ((const SMPState*)st);
                    auto vidl = sst->getIdeal(i);
                    assert(1 == vpit->numC());
                    assert(numDim == vpit->numR());
                    const double pCoord = (*vpit)(k, 0) * 100.0; // Use the scale of [0,100]
                    // have to print "100.0" sometimes
                    actorPosHistory += KBase::getFormattedString(" %5.1f", pCoord);
                    int rslt = 0;
                    rslt = sqlite3_bind_int(insStmt, 1, t);
                    assert(SQLITE_OK == rslt);
                    rslt = sqlite3_bind_int(insStmt, 2, i);
                    assert(SQLITE_OK == rslt);
                    rslt = sqlite3_bind_int(insStmt, 3, k);
                    assert(SQLITE_OK == rslt);
                    rslt = sqlite3_bind_double(insStmt, 4, pCoord); // Log at the scale of [0,100]
                    assert(SQLITE_OK == rslt);
                    const double iCoord = vidl(k, 0) * 100.0; // Log at the scale of [0,100];
                    rslt = sqlite3_bind_double(insStmt, 5, iCoord);
                    assert(SQLITE_OK == rslt);

                    try {
                      rslt = sqlite3_bind_int(insStmt, 6, sst->getPosMoverBargain(i));
                      assert(SQLITE_OK == rslt);
                    }
                    catch (const std::out_of_range& oor) { // exception thrown by std::map::at()
                      // do nothing
                    }
                    rslt = sqlite3_step(insStmt);
                    assert(SQLITE_DONE == rslt);
                    sqlite3_clear_bindings(insStmt);
                    assert(SQLITE_DONE == rslt);
                    rslt = sqlite3_reset(insStmt);
                    assert(SQLITE_OK == rslt);
                }
                LOG(DEBUG) << actorPosHistory;
                actorPosHistory.clear();
            }
        }

        sqlite3_exec(smpDB, "END TRANSACTION", NULL, NULL, &zErrMsg);
        sqlite3_finalize(insStmt); // finalize statement to avoid resource leaks

        delete sqlBuff;
        sqlBuff = nullptr;
    }

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
    LOG(DEBUG) << "History of actors' winning probabilities:";
    string winProbsOfOneActr;
    for (unsigned int i = 0; i < numAct; i++) {
        winProbsOfOneActr += actrs[i]->name + ", prob :";
        for (unsigned int t = 0; t < history.size(); t++) {
            winProbsOfOneActr += KBase::getFormattedString(" %.4f", probIT(i, t));
        }
        LOG(DEBUG) << winProbsOfOneActr;
        winProbsOfOneActr.clear();
    }
    return;
}

SMPModel * SMPModel::initModel(vector<string> aName, vector<string> aDesc, vector<string> dName,
                               const KMatrix & cap, // one row per actor
                               const KMatrix & pos, // one row per actor, one column per dimension
                               const KMatrix & sal, // one row per actor, one column per dimension
                               const KMatrix & accM,
                               uint64_t s, vector<bool> f, string scenDesc, string scenName)
{    
    assert(f.size() == Model::NumSQLLogGrps + NumSQLLogGrps);
    SMPModel * sm0 = new SMPModel(scenDesc, s, f, scenName); // JAH 20160711 added rng seed 20160730 JAH added sql flags
    SMPState * st0 = new SMPState(sm0);
    sm0->addState(st0);

    st0->step = [st0]() {
        return st0->stepBCN();
    };

    auto na = ((const unsigned int)(aName.size()));
    auto nd = ((const unsigned int)(dName.size()));


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

    st0->setAccomodate(accM);
    st0->idealsFromPstns();

    return sm0;
}

void SMPModel::setDBPath(std::string dbName)
{
    dbPath = dbName;
}
void SMPModel::displayModelParams(SMPModel *md0)
{
    LOG(DEBUG) << "Model Paramaters to run the model...";
    LOG(DEBUG) << "VictoryProbModel:" << md0->vpm;
    LOG(DEBUG) << "VotingRule:" << md0->vrCltn;
    LOG(DEBUG) << "PCEModel:" << md0->pcem;
    LOG(DEBUG) << "StateTransitions:" << md0->stm;
    LOG(DEBUG) << "BigRRange:" << md0->bigRRng;
    LOG(DEBUG) << "BigRAdjust:" << md0->bigRAdj;
    LOG(DEBUG) << "ThirdPartyCommit:" << md0->tpCommit;
    LOG(DEBUG) << "InterVecBrgn:" << md0->ivBrgn;
    LOG(DEBUG) << "BargnModel:" << md0->brgnMod;
}

string SMPModel::runModel(vector<bool> sqlFlags, string dbFilePath,
                          string inputDataFile, uint64_t seed, bool saveHist, vector<int> modelParams) {
    SMPModel::setDBPath(dbFilePath);
    if (md0 != nullptr) {
        delete md0;
        md0 = nullptr;
    }

    // Supported files for input data: xml, csv
    size_t dotPos = inputDataFile.find_last_of(".");
    assert(dotPos != string::npos); // A file name without extension

    string fileExt = inputDataFile.substr(dotPos+1);
    string fileName= inputDataFile.substr(0,dotPos);

    // convert to all lower case for easy comparison
    std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

    // Make sure the file extension is either csv or xml only
    assert((fileExt == "csv") || (fileExt == "xml"));

    if (fileExt == "xml") {
        md0 = xmlRead(inputDataFile, sqlFlags);

        if (-1 != seed) {
            md0->setSeed(seed);
            LOG(DEBUG) << KBase::getFormattedString(
              "Using PRNG seed provided by the user: %020llu", md0->getSeed());
        }
        else {
            LOG(DEBUG) << KBase::getFormattedString(
              "Using PRNG seed provided by xml file: %020llu", md0->getSeed());
        }
    }
    else if (fileExt == "csv") {
        md0 = csvRead(inputDataFile, seed, sqlFlags);
    }

    if (!modelParams.empty()) {
        SMPModel::updateModelParameters(md0, modelParams);
    }

    displayModelParams(md0);
    configExec(md0);
    md0->releaseDB();
    if (saveHist)
    {
        md0->sankeyOutput(fileName);
    }
    return md0->getScenarioID();
}

string SMPModel::csvReadExec(uint64_t seed, string inputCSV, vector<bool> f, string dbFilePath, vector<int> par) {
    SMPModel::setDBPath(dbFilePath);
    if (md0 != nullptr) {
        delete md0;
        md0 = nullptr;
    }
    
    md0 = csvRead(inputCSV, seed, f);
    if (false == par.empty()) {
        SMPModel::updateModelParameters(md0, par);
    }
    displayModelParams(md0);
    configExec(md0);
    md0->releaseDB();
    return md0->getScenarioID();
}

string SMPModel::xmlReadExec(string inputXML, vector<bool> f, string dbFilePath) {
    SMPModel::setDBPath(dbFilePath);
    md0 = SMPModel::xmlRead(inputXML, f);
    displayModelParams(md0);
    configExec(md0);
    md0->releaseDB();
    return md0->getScenarioID();
}

void SMPModel::configExec(SMPModel * md0)
{
    // setup the stopping criteria and lambda function
    const unsigned int minIter = 2;
    const unsigned int maxIter = 100;
    const double minDeltaRatio = 0.02;
    // suppose that, on a [0,100] scale, the first move was the most extreme possible,
    // i.e. 100 points. One fiftieth of that is just 2, which seems to about the limit
    // of what people consider significant.
    const double minSigDelta = 1E-4;
    // typical first shifts are on the order of numAct/10, so this is low
    // enough not to affect anything while guarding against the theoretical
    // possiblity of 0/0 errors
    //md0->stop = [maxIter](unsigned int iter, const State * s) {
    //    return (maxIter <= iter);
    //};
    md0->stop = smpStopFn(minIter, maxIter, minDeltaRatio, minSigDelta);

    // Drop the indices of the tables before the model run
    md0->dropTableIndices();

    // execute
    LOG(INFO) << "Starting model run";
    md0->run();
    const unsigned int nState = md0->history.size();

    // log data, or not
    // JAH 20160731 added to either log all information tables or none
    // this takes care of info re. actors, dimensions, scenario, capabilities, and saliences
    if (md0->sqlFlags[0])
    {
        md0->LogInfoTables();
    }

    if (md0->sqlFlags[4]) {
        for (auto turn = 0; turn < nState; ++turn) {
            md0->sqlAUtil(turn);
        }
    }

    // JAH 20160802 added logging control flag for the last state
    // also added the sqlPosVote and sqlPosEquiv calls to get the final state
    if (md0->sqlFlags[1])
    {
        md0->sqlPosProb(nState - 1);
        md0->sqlPosEquiv(nState - 1);
        md0->sqlPosVote(nState - 1);
    }

    LOG(DEBUG) << "Completed model run";
    LOG(DEBUG) << KBase::getFormattedString(
      "There were %u states, with %i steps between them", nState, nState - 1);
    md0->showVPHistory();

    //Create indices in the tables
    md0->createTableIndices();

    return;
}

//Model Parameters
void SMPModel::updateModelParameters(SMPModel *md0, vector <int> parameters)
{
    md0->vpm= (VPModel)parameters.at(0); //victProbModel
    md0->pcem = (PCEModel)parameters.at(1); //pCEModel
    md0->stm = (StateTransMode)parameters.at(2); //stateTransitions
    md0->vrCltn = (VotingRule)parameters.at(3); //votingRule
    md0->bigRAdj = (BigRAdjust)parameters.at(4); //bigRAdjust
    md0->bigRRng = (BigRRange)parameters.at(5); //bigRRange
    md0->tpCommit = (ThirdPartyCommit)parameters.at(6); //thirdPartyCommit
    md0->ivBrgn = (InterVecBrgn)parameters.at(7); //interVecBrgn
    md0->brgnMod = (SMPBargnModel)parameters.at(8); //bargnModel
}

vector<int> SMPModel::getDefaultModelParameters()
{
    vector<int> defaultParameters;
    SMPModel * dummyModel = new SMPModel;

    defaultParameters.push_back((int)dummyModel->vpm);
    defaultParameters.push_back((int)dummyModel->pcem);
    defaultParameters.push_back((int)dummyModel->stm);
    defaultParameters.push_back((int)dummyModel->vrCltn);
    defaultParameters.push_back((int)dummyModel->bigRAdj);
    defaultParameters.push_back((int)dummyModel->bigRRng);
    defaultParameters.push_back((int)dummyModel->tpCommit);
    defaultParameters.push_back((int)dummyModel->ivBrgn);
    defaultParameters.push_back((int)dummyModel->brgnMod);

    delete dummyModel;
    return defaultParameters;
}

double SMPModel::getQuadMapPoint(size_t t, size_t est_h, size_t aff_k, size_t init_i, size_t rcvr_j) {
    auto smpState = md0->history[t];
    auto autil = smpState->aUtil;
    double uii = autil[est_h](init_i, init_i);
    double uij = autil[est_h](init_i, rcvr_j);
    double uji = autil[est_h](rcvr_j, init_i);
    double ujj = autil[est_h](rcvr_j, rcvr_j);

    // h's estimate of utility to k of status-quo positions of i and j
    const double euSQ = autil[est_h](aff_k, init_i) + autil[est_h](aff_k, rcvr_j);
    assert((0.0 <= euSQ) && (euSQ <= 2.0));

    // h's estimate of utility to k of i defeating j, so j adopts i's position
    const double uhkij = autil[est_h](aff_k, init_i) + autil[est_h](aff_k, init_i);
    assert((0.0 <= uhkij) && (uhkij <= 2.0));

    // h's estimate of utility to k of j defeating i, so i adopts j's position
    const double uhkji = autil[est_h](aff_k, rcvr_j) + autil[est_h](aff_k, rcvr_j);
    assert((0.0 <= uhkji) && (uhkji <= 2.0));

    auto ai = ((const SMPActor*)(md0->actrs[init_i]));
    double si = KBase::sum(ai->vSal);
    double ci = ai->sCap;
    auto aj = ((const SMPActor*)(md0->actrs[rcvr_j]));
    double sj = KBase::sum(aj->vSal);
    assert((0 < sj) && (sj <= 1));
    double cj = aj->sCap;
    const double minCltn = 1E-10;

    auto contribs = calcContribs(md0->vrCltn, si*ci, sj*cj, tuple<double, double, double, double>(uii, uij, uji, ujj));

    double chij = get<0>(contribs); // strength of complete coalition supporting i over j (initially empty)
    double chji = get<1>(contribs); // strength of complete coalition supporting j over i (initially empty)

    // cache those sums
    double contrib_i_ij = chij;
    double contrib_j_ij = chji;

    // we assess the overall coalition strengths by adding up the contribution of
    // individual actors (including i and j, above). We assess the contribution of third
    // parties (n) by looking at little coalitions in the hypothetical (in:j) or (i:nj) contests.
    for (unsigned int n = 0; n < md0->numAct; n++) {
        if ((n != init_i) && (n != rcvr_j)) { // already got their influence-contributions
            auto an = ((const SMPActor*)(md0->actrs[n]));

            double cn = an->sCap;
            double sn = KBase::sum(an->vSal);
            double uni = autil[est_h](n, init_i);
            double unj = autil[est_h](n, rcvr_j);
            double unn = autil[est_h](n, n);

            // notice that each third party starts afresh,
            // considering only contributions of principals and itself
            double pin = Actor::vProbLittle(md0->vrCltn, sn*cn, uni, unj, contrib_i_ij, contrib_j_ij);

            assert(0.0 <= pin);
            assert(pin <= 1.0);
            double pjn = 1.0 - pin;
            auto vt_uv_ul = Actor::thirdPartyVoteSU(sn*cn, md0->vrCltn, md0->tpCommit, pin, pjn, uni, unj, unn);
            const double vnij = get<0>(vt_uv_ul);
            chij = (vnij > 0) ? (chij + vnij) : chij;
            assert(0 < chij);
            chji = (vnij < 0) ? (chji - vnij) : chji;
            assert(0 < chji);
        }
    }

    const double phij = chij / (chij + chji); // ProbVict, for i
    const double phji = chji / (chij + chji);

    const double euVict = uhkij;  // UtilVict
    const double euCntst = phij*uhkij + phji*uhkji; // UtilContest,
    const double euChlg = (1 - sj)*euVict + sj*euCntst; // UtilChlg

    return (euChlg - euSQ);
}

int SMPModel::callBack(void *data, int numCol, char **stringFields, char **colNames)
{
    fieldVals.clear(); assert(fieldVals.empty() == true);

    for (int i = 0; i < numCol; i++)
    {
                fieldVals.push_back(stringFields[i] ? stringFields[i] : "NULL");
    }

    assert(fieldVals.size() > 0);
    return (int)0;
};


int SMPModel::sankeyCallBack(void *data, int numCol, char **stringFields, char **colNames)
{
    for (int i = 0; i < numCol; i++)
    {
        dbFieldVals.push_back(stringFields[i] ? stringFields[i] : "NULL");
    }

    assert(dbFieldVals.size() > 0);
    return (int)0;
};


double SMPModel::getQuadMapPoint(string dbname, string scenarioID, size_t turn, size_t est_h,
                                 size_t aff_k, size_t init_i, size_t rcvr_j) {

    sqlite3 *db = nullptr;
    char* zErrMsg = nullptr;
    if (sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READONLY, NULL)) {
        LOG(ERROR) << "Failed try to open db file :" << dbname;
        LOG(ERROR) << "SQLite Error:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        exit(-1);
    }

    auto sqlExec = [&db, &zErrMsg](string sqlQry) {
        int rc = sqlite3_exec(db, sqlQry.c_str(), callBack, nullptr, &zErrMsg);
        if (rc != SQLITE_OK) {
            LOG(ERROR) << "SQL Error:" << zErrMsg;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
        }
        return rc;
    };

    auto getUtilQuery = [scenarioID, turn, est_h](size_t initiator, size_t receiver) {
        string query = "SELECT Util FROM PosUtil WHERE ScenarioId = \'" + scenarioID
                + "\' AND Turn_t = " + std::to_string(turn) + " AND Est_h = " + std::to_string(est_h)
                + " AND Act_i = " + std::to_string(initiator) + " AND Pos_j =  " + std::to_string(receiver);
        return query;
    };

    auto getVSalQuery = [scenarioID, turn](size_t actor) {
        string query = "SELECT SUM(Sal) FROM SpatialSalience WHERE ScenarioId=\'" + scenarioID
                + "\' AND Turn_t = " + std::to_string(turn) + " AND Act_i = " + std::to_string(actor);

        return query;
    };

    auto getSCapQuery = [scenarioID, turn](size_t actor) {
        string query = "SELECT Cap FROM SpatialCapability WHERE ScenarioId=\'" + scenarioID
                + "\' AND Turn_t = " + std::to_string(turn) + " AND Act_i = " + std::to_string(actor);

        return query;
    };

    // Get voting rule and third party commit for this scenario
    string query = "SELECT VotingRule, ThirdPartyCommit FROM ScenarioDesc WHERE ScenarioId=\'" + scenarioID + "\'";
    int rc = sqlExec(query);
    assert(SQLITE_OK == rc);
    assert(fieldVals.size() == 2 );

    //voting rule
    int vr = stoi(fieldVals[0]);
    VotingRule vrCltn = static_cast<VotingRule>(vr);

    //third party commit
    int tpc = stoi(fieldVals[1]);
    ThirdPartyCommit tpCommit = static_cast<ThirdPartyCommit>(tpc);

    rc = sqlExec(getUtilQuery(init_i, init_i));
    assert(SQLITE_OK == rc);
    double uii = stod(fieldVals[0]);

    rc = sqlExec(getUtilQuery(init_i, rcvr_j));
    assert(SQLITE_OK == rc);
    double uij = stod(fieldVals[0]);

    rc = sqlExec(getUtilQuery(rcvr_j, init_i));
    assert(SQLITE_OK == rc);
    double uji = stod(fieldVals[0]);

    rc = sqlExec(getUtilQuery(rcvr_j, rcvr_j));
    assert(SQLITE_OK == rc);
    double ujj = stod(fieldVals[0]);

    rc = sqlExec(getUtilQuery(aff_k, init_i));
    assert(SQLITE_OK == rc);
    double uki = stod(fieldVals[0]);

    rc = sqlExec(getUtilQuery(aff_k, rcvr_j));
    assert(SQLITE_OK == rc);
    double ukj = stod(fieldVals[0]);

    double euSQ = uki + ukj;

    double uhkij = 2 * uki;
    assert((0.0 <= uhkij) && (uhkij <= 2.0));

    double uhkji = 2 * ukj;
    assert((0.0 <= uhkji) && (uhkji <= 2.0));

    rc = sqlExec(getVSalQuery(init_i));
    assert(SQLITE_OK == rc);
    double si = stod(fieldVals[0]);
    assert((0 < si) && (si <= 1));
    
    rc = sqlExec(getVSalQuery(rcvr_j));
    assert(SQLITE_OK == rc);
    double sj = stod(fieldVals[0]);
    assert((0 < sj) && (sj <= 1));

    rc = sqlExec(getSCapQuery(init_i));
    assert(SQLITE_OK == rc);
    double ci = stod(fieldVals[0]);

    rc = sqlExec(getSCapQuery(rcvr_j));
    assert(SQLITE_OK == rc);
    double cj = stod(fieldVals[0]);

    auto contribs = calcContribs(vrCltn, si*ci, sj*cj, tuple<double, double, double, double>(uii,uij,uji,ujj));

    double chij = get<0>(contribs); // strength of complete coalition supporting i over j (initially empty)
    double chji = get<1>(contribs); // strength of complete coalition supporting j over i (initially empty)

    // cache those sums
    double contrib_i_ij = chij;
    double contrib_j_ij = chji;

    // Get count of actors for this scenario
    query = "SELECT MAX(Act_i) FROM ActorDescription WHERE ScenarioId=\'" + scenarioID + "\'";
    rc = sqlExec(query);
    assert(SQLITE_OK == rc);
    size_t numAct = stoul(fieldVals[0])+1;

    // we assess the overall coalition strengths by adding up the contribution of
    // individual actors (including i and j, above). We assess the contribution of third
    // parties (n) by looking at little coalitions in the hypothetical (in:j) or (i:nj) contests.
    for (size_t n = 0; n < numAct; n++) {
        if ((n != init_i) && (n != rcvr_j)) { // already got their influence-contributions

            rc = sqlExec(getVSalQuery(n));
            assert(SQLITE_OK == rc);
            double sn = stod(fieldVals[0]);

            rc = sqlExec(getSCapQuery(n));
            assert(SQLITE_OK == rc);
            double cn = stod(fieldVals[0]);

            rc = sqlExec(getUtilQuery(n, init_i));
            assert(SQLITE_OK == rc);
            double uni = stod(fieldVals[0]);

            rc = sqlExec(getUtilQuery(n, rcvr_j));
            assert(SQLITE_OK == rc);
            double unj = stod(fieldVals[0]);

            rc = sqlExec(getUtilQuery(n, n));
            assert(SQLITE_OK == rc);
            double unn = stod(fieldVals[0]);

            // notice that each third party starts afresh,
            // considering only contributions of principals and itself
            double pin = Actor::vProbLittle(vrCltn, sn*cn, uni, unj, contrib_i_ij, contrib_j_ij);

            assert(0.0 <= pin && pin <= 1.0);
            double pjn = 1.0 - pin;
            auto vt_uv_ul = Actor::thirdPartyVoteSU(sn*cn, vrCltn, tpCommit, pin, pjn, uni, unj, unn);
            const double vnij = get<0>(vt_uv_ul);
            chij = (vnij > 0) ? (chij + vnij) : chij;
            assert(0 < chij);
            chji = (vnij < 0) ? (chji - vnij) : chji;
            assert(0 < chji);
        }
    }

    const double phij = chij / (chij + chji); // ProbVict, for i
    const double phji = chji / (chij + chji);

    const double euVict = uhkij;  // UtilVict
    const double euCntst = phij*uhkij + phji*uhkji; // UtilContest,
    const double euChlg = (1 - sj)*euVict + sj*euCntst; // UtilChlg

    sqlite3_close_v2(db);

    return (euChlg - euSQ);
}

tuple<double, double> SMPModel::calcContribs(VotingRule vrCltn, double wi, double wj, tuple<double, double, double, double>(utils)) {
    const double minCltn = 1E-10;

    // get h's estimate of the principal actors' contribution to their own contest

    // h's estimate of i's unilateral influence contribution to (i:j).
    // When ideals perfectly track positions, this must be positive

    double contrib_i_ij = Model::vote(vrCltn, wi, get<0>(utils), get<1>(utils));
    assert(0 <= contrib_i_ij);

    // h's estimate of j's unilateral influence contribution to (i:j).
    // When ideals perfectly track positions, this must be negative
    double contrib_j_ij = Model::vote(vrCltn, wj, get<2>(utils), get<3>(utils));
    assert(contrib_j_ij <= 0);

    double chij = minCltn; // strength of complete coalition supporting i over j (initially empty)
    double chji = minCltn; // strength of complete coalition supporting j over i (initially empty)

    // add i's contribution to the appropriate coalition
    if (contrib_i_ij > 0.0) {
        chij = chij + contrib_i_ij;
    }
    assert(0.0 < chij);

    if (contrib_i_ij < 0.0) {
        chji = chji - contrib_i_ij;
    }
    assert(0.0 < chji);

    // add j's contribution to the appropriate coalition
    if (contrib_j_ij > 0.0) {
        chij = chij + contrib_j_ij;
    }
    assert(0.0 < chij);

    if (contrib_j_ij < 0.0) {
        chji = chji - contrib_j_ij;
    }
    assert(0.0 < chji);

    return tuple<double, double>(chij, chji);
}

void SMPModel::destroyModel() {
    delete md0;
}

void SMPModel::randomSMP(unsigned int numA, unsigned int sDim, bool accP, uint64_t s, vector<bool> f, string inputDBname) {
    SMPModel::setDBPath(inputDBname);
    // JAH 20160711 added rng seed 20160730 JAH added sql flags
    md0 = new SMPModel("", s, f);

    if (0 == numA) {
        double lnMin = log(4);
        double lnMax = log(25);
        // median is 10  = exp( [log(4)+log(25)] /2 ) = sqrt(4*25)
        double na = exp(md0->rng->uniform(lnMin, lnMax));
        numA = ((unsigned int)(na + 0.5)); // i.e. [5,10] inclusive
    }

    if (0 == sDim) {
        sDim = 1 + (md0->rng->uniform() % 3); // i.e. [1,3] inclusive
    }

    LOG(DEBUG) << "EU State for SMP actors with scalar capabilities";
    LOG(DEBUG) << "Number of actors:" << numA;
    LOG(DEBUG) << "Number of SMP dimensions:" << sDim;

    assert(0 < sDim);
    assert(2 < numA);

    for (unsigned int i = 0; i < sDim; i++) {
        auto buff = KBase::newChars(100);
        sprintf(buff, "SDim-%02u", i);
        md0->addDim(buff);
        delete buff;
        buff = nullptr;
    }

    assert(sDim == md0->numDim);

    SMPState* st0 = new SMPState(md0);
    md0->addState(st0); // now state 0 of the histor

    st0->step = [st0]() {
        return st0->stepBCN();
    };

    for (unsigned int i = 0; i < numA; i++) {
        unsigned int nbSize = 15;
        char * nameBuff = new char[nbSize];
        for (unsigned int j = 0; j < nbSize; j++) {
            nameBuff[j] = (char)0;
        }
        sprintf(nameBuff, "SActor-%02u", i);
        auto ni = string(nameBuff);
        delete[] nameBuff;
        nameBuff = nullptr;
        string di = "Random spatial actor";

        auto ai = new SMPActor(ni, di);
        ai->randomize(md0->rng, sDim);
        auto iPos = new VctrPstn(KMatrix::uniform(md0->rng, sDim, 1, 0.0, 1.0)); // SMP is always on [0,1] scale
        md0->addActor(ai);
        st0->addPstn(iPos);
    }

    for (unsigned int i = 0; i < numA; i++) {
        auto ai = ((SMPActor*)(md0->actrs[i]));
        double ri = 0.0; // st0->aNRA(i);
        LOG(INFO) << KBase::getFormattedString(
          "%2u: %s, %s", i, ai->name.c_str(), ai->desc.c_str());
        string vrs = KBase::nameFromEnum<VotingRule>(ai->vr, KBase::VotingRuleNames);
        LOG(INFO) << "voting rule:" << vrs;
        LOG(DEBUG) << "Pos vector:";
        VctrPstn * pi = ((VctrPstn*)(st0->pstns[i]));
        (trans(*pi) * 100.0).mPrintf(" %+7.4f "); // print on the scale of [0,100]
        LOG(DEBUG) << "Sal vector: ";
        trans(ai->vSal).mPrintf(" %+7.4f ");
        LOG(INFO) << KBase::getFormattedString("Capability: %.3f", ai->sCap);
        LOG(INFO) << KBase::getFormattedString("Risk attitude: %+.4f", ri);
    }

    auto aMat = KBase::iMat(md0->numAct);
    if (accP) {
        LOG(DEBUG) << "Using randomized matrix for ideal-accomodation";
        for (unsigned int i = 0; i<md0->numAct; i++) {
            aMat(i, i) = md0->rng->uniform(0.1, 0.5); // make them lag noticably
        }
    }
    else {
        LOG(DEBUG) << "Using identity matrix for ideal-accomodation";
    }
    LOG(DEBUG) << "Accomodate matrix:";
    aMat.mPrintf(" %.3f ");

    st0->setAccomodate(aMat);
    st0->idealsFromPstns();
    st0->setUENdx();
    st0->setAUtil(-1, ReportingLevel::Silent);
    st0->setNRA(); // TODO: simple setting of NRA

    // with SMP actors, we can always read their ideal position.
    // with strategic voting, they might want to advocate positions
    // separate from their ideal, but this simple demo skips that.
    auto uFn1 = [st0](unsigned int i, unsigned int j) {
        auto ai = ((SMPActor*)(st0->model->actrs[i]));
        auto pj = ((VctrPstn*)(st0->pstns[j])); // aj->iPos;
        double uij = ai->posUtil(pj, st0);
        return uij;
    };

    auto u = KMatrix::map(uFn1, numA, numA);
    LOG(DEBUG) << "Raw actor-pos util matrix";
    u.mPrintf(" %.4f ");

    auto w = st0->actrCaps(); //  KMatrix::map(wFn, 1, numA);

    // no longer need external reference to the state
    st0 = nullptr;

    // arbitrary but illustrates that we can do an election with arbitrary
    // voting rules - not necessarily the same as the actors would do.
    auto vr = VotingRule::Binary;
    string vrs = KBase::nameFromEnum<VotingRule>(vr, KBase::VotingRuleNames);
    LOG(DEBUG) << "Using voting rule";

    const KBase::VPModel vpm = md0->vpm;
    const KBase::PCEModel pcem = md0->pcem;

    KMatrix p = Model::scalarPCE(numA, numA, w, u, vr, vpm, pcem, ReportingLevel::Medium);

    LOG(DEBUG) << "Expected utility to actors:";
    (u*p).mPrintf(" %.3f ");

    LOG(DEBUG) << "Net support for positions:";
    (w*u).mPrintf(" %.3f ");

    auto aCorr = [](const KMatrix & x, const KMatrix & y) {
        using KBase::lCorr;
        using KBase::mean;
        return  lCorr(x - mean(x), y - mean(y));
    };

    // for nearly flat distributions, and nearly flat net support,
    // one can sometimes see negative affine-correlations because of
    // random variations in 3rd or 4th decimal places.
    LOG(INFO) << KBase::getFormattedString(
      "L-corr of prob and net support: %+.4f", KBase::lCorr((w*u), trans(p)));
    LOG(INFO) << KBase::getFormattedString(
      "A-corr of prob and net support: %+.4f", aCorr((w*u), trans(p)));

    SMPModel::configExec(md0);

    delete md0;
    md0 = nullptr;

    return;
}

}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
