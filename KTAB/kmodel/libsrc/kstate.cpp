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

#include "kmodel.h"

namespace KBase {
using std::get;
using std::tuple;
using KBase::PRNG;
using KBase::KMatrix;


State::State(Model * m) {
  if (nullptr == m) {
    throw KException("State::State: Model is null pointer.");
  }
  model = m;

  // As we pre-size the pstns array, we must make certain the pointers do not point at junk.
  // If the Model has been populated, then this allocates a non-zero vector.
  // But if it is a brand-new Model, it might not have any actors, so you still
  // have to allocate/push later.
  const unsigned int na = model->numAct;
  pstns.resize(na);
  for (unsigned int i = 0; i < na; i++) {
    pstns[i] = nullptr;
  }
}

State::~State() {
  clear();
}

void State::clear() {
  // We delete positions because they are part of the state.
  // Actors persist across states, so they are not deleted here.
  aUtil = {}; // vector<KMatrix>();
  for (auto p : pstns) {
    if (nullptr != p) {
      delete p;
    }
  }
  pstns = {}; // vector<Position*>();
  step = nullptr;
}

void State::pushPstn(Position* p) {
  if (nullptr == p) {
    throw KException("State::pushPstn: Position's pointer is null");
  }
  pstns.push_back(p);
  return;
}


void State::randomizeUtils(double minU, double maxU, double uNoise) {
  auto rng = model->rng;
  unsigned int na = model->numAct;
  aUtil = vector<KMatrix>();
  auto u = KMatrix::uniform(rng, na, na, minU, maxU);
  for (unsigned int i = 0; i < na; i++) {
    auto un = KMatrix::uniform(rng, na, na, -uNoise, +uNoise);
    aUtil.push_back(u + un);
  }
  return;
}
double State::posProb(unsigned int i, const VUI & unq, const KMatrix & pdt) const {
  const unsigned int numA = model->numAct;
  auto nUnq = ((const unsigned int)(unq.size()));
  unsigned int k = numA + 1; // impossibly high
  for (unsigned int j1 = 0; j1 < nUnq; j1++) { // scan unique positions
    unsigned int j2 = unq[j1]; // get ordinary index of the position
    if (equivNdx(i, j2)) {
      k = j1;
    }
  }
  if (k >= numA) {
    throw KException("State::posProb: k can not be more than number of actors");
  }
  if (1 != pdt.numC()) {
    throw KException("State::posProb: pdt matrix must be a column matrix");
  }
  if (k >= pdt.numR()) {
    throw KException("State::posProb: kth row number doesn't exist in pdt matrix");
  }
  if (nUnq != pdt.numR()) {
    throw KException("State::posProb: row count of pdt matrix should be equal to nUnq");
  }
  double pr = pdt(k, 0);
  return pr;
}

// return the turn-number of this state.
// 0 == initial state, and error if not in the model's history
unsigned int State::myTurn() const {
  int t = -1; // flag an impossible value
  if (nullptr == model) {
    throw KException("State::myTurn: model is a null pointer");
  }
  auto hLen = ((const unsigned int)(model->history.size()));
  for (unsigned int i = 0; i < hLen; i++) { // cannot use range-for, as I need the value of 'i'
    State* si = model->history[i];
    if (nullptr == si) {
      throw KException("State::myTurn: si is a null pointer");
    }
    if (this == si) {
      t = i;
    }
  }
  if (0 > t) {
    throw KException("State::myTurn: turn can't be negative");
  }
  return t;
}

void State::setUENdx() {
  /// Looking only at the positions in this state, return a vector of indices of unique positions.
  if (0 != uIndices.size()) {
    throw KException("State::setUENdx: uIndices must be empty");
  }

  if (0 != eIndices.size()) {
    throw KException("State::setUENdx: eIndices must be empty");
  }

  // Note that we have to lambda-bind 'this'. Otherwise, we'd need a 'static' function
  // to give to uIndices.
  auto efn = [this](unsigned int i, unsigned int j) {
    return equivNdx(i, j);
  };
  const unsigned int na = model->numAct;
  if (Model::minNumActor > na) {
    throw KException(string("State::setUENdx: Number of actors can not be less than ")
      + std::to_string(Model::minNumActor));
  }

  if (Model::maxNumActor < na) {
    throw KException(string("State::setUENdx: Number of actors can not be more than")
      + std::to_string(Model::maxNumActor));
  }

  auto ns = KBase::uiSeq(0, na - 1);
  auto uePair = KBase::ueIndices<unsigned int>(ns, efn);

  uIndices = get<0>(uePair);
  auto nu = ((const unsigned int)(uIndices.size()));
  if (0 >= nu || nu > na) {
    throw KException(string("State::setUENdx: size of uIndices must be in the range of (0,") + std::to_string(na) + "]");
  }


  eIndices = get<1>(uePair);
  auto ne = ((const unsigned int)(eIndices.size()));
  if (na != ne) {
    throw KException("State::setUENdx: Count of actors not matching with the size of eIndices");
  }


  return;
}


void State::setAUtil(int perspH, ReportingLevel rl) {
  // we want to make sure that data is calculated at most once.
  // This is necessary because some utilities are very expensive to calculate,
  // it is easiest to be precise all the time.

  if (-1 == perspH) { // calculate them all at once
    if (0 != aUtil.size()) {
      throw KException("State::setAUtil: Util vector is not empty");
    }

    setAllAUtil(rl);
  }
  else { // we might get the perspectives of just a few actors
    const unsigned int na = model->numAct;
    if (0 > perspH || perspH >= na) {
      throw KException(string("State::setAUtil: Perspective of h must be in the range of [0,") + std::to_string(na) + ")");
    }

    bool firstP = (0 == aUtil.size());
    bool firstForH = ((na == aUtil.size()) && (0 == aUtil[perspH].numR()) && (0 == aUtil[perspH].numC()));
    if (!(firstP || firstForH)) {
      throw KException("State::setAUtil: No first perspective");
    }
    if (firstP) {
      aUtil.resize(na);
      for (unsigned int i = 0; i < na; i++) {
        aUtil[i] = KMatrix(0, 0);
      }
    }
    setOneAUtil(perspH, rl);
  }
  return;
}

void State::setOneAUtil(unsigned int perspH, ReportingLevel rl) {
  // TODO: make this non-dummy
  throw KException("State::setOneAUtil: A dummy function");
  return;
}

} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
