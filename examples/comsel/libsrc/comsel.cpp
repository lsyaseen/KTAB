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
// define these:
//  tuple< KMatrix, VUI> CSState::pDist(int persp) const;
//  bool CSState::equivNdx(unsigned int i, unsigned int j) const;
//  void CSState::setAllAUtil(ReportingLevel rl);
//  CSState* CSState::doSUSN
//  CSState* CSState::stepSUSN
// --------------------------------------------
	
#include "comsel.h"

namespace ComSelLib {
  using std::cout;
  using std::endl;
  using std::flush;
  
  using KBase::VBool;

  // --------------------------------------------
  CSModel::CSModel(unsigned int nd, PRNG* r, string d) : Model(r, d) {

    assert (nd > 0);

    numDims = nd;
  }

  
  CSModel::~CSModel(){
    // nothing yet
  }
  
  bool CSModel::equivStates(const CSState * rs1, const CSState * rs2) {
    const unsigned int numA = rs1->pstns.size();
    assert(numA == rs2->pstns.size());
    bool rslt = true;
    for (unsigned int i = 0; i < numA; i++) {
      auto m1 = ((MtchPstn*)(rs1->pstns[i]));
      auto m2 = ((MtchPstn*)(rs2->pstns[i]));
      bool same = (m1->match == m2->match);
      //cout << "Match on position " << i << "  " << same << endl << flush;
      rslt = rslt && same;
    }
    return rslt;
  }
  // --------------------------------------------
  CSState::CSState(CSModel * m) : State(m) {
    // nothing yet
  }

  CSState::~CSState()  {
    // nothing yet
  }
  
  void CSState::show() const {
    cout << "CSState::show - not yet implemented" << endl << flush;
    return;
  }
  
  tuple< KMatrix, VUI> CSState::pDist(int persp) const {
    tuple< KMatrix, VUI> stuff;
    cout << "CSState::pDist not yet implemented" << endl; // TODO: complete this
    assert(false);
    return stuff;
  }
  
  CSState* CSState::stepSUSN() {
    cout << endl << flush;
    cout << "State number " << model->history.size() - 1 << endl << flush;
    if ((0 == uIndices.size()) || (0 == eIndices.size())) {
      setUENdx();
    }
    setAUtil(-1, ReportingLevel::Silent);
    show();

    auto s2 = doSUSN(ReportingLevel::Silent);
    s2->step = [s2]() {
      return s2->stepSUSN();
    };
    cout << endl << flush;
    return s2;
  }

  
  CSState * CSState::doSUSN(ReportingLevel rl) const {
    CSState * cs2 = nullptr;
    cout << "CSState::doSUSN not yet implemented" << endl; // TODO: finish this
    assert (false);
    return cs2;
  }

  
  CSState * CSState::doBCN(ReportingLevel rl) const {
    CSState * cs2 = nullptr;
    cout << "CSState::doBCN not yet implemented" << endl; // TODO: finish this
    assert (false);
    return cs2;
  }
    
  bool CSState::equivNdx(unsigned int i, unsigned int j) const {
    bool stuff = false;
    cout << "CSState::equivNdx not yet implemented" << endl; // TODO: finish this
    assert(false);
    return stuff;
  }
    
  void CSState::setAllAUtil(ReportingLevel rl){
    cout << "CSState::setAllAUtil not yet implemented" << endl; // TODO: finish this
    assert(false);
    return;
  }
};
// end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
 
