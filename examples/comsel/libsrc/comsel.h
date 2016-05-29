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

#ifndef COMSEL_LIB_H
#define COMSEL_LIB_H

#include <algorithm>
//#include "csv_parser.hpp"
#include "sqlite3.h"
#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"
#include "gaopt.h"
#include "kmodel.h"

#include "smp.h"

namespace ComSelLib {
  // namespace to which KBase has no access
  //  using std::function;
  using std::shared_ptr;
  using std::string;
  using std::tuple;
  using std::vector;
  using KBase::newChars;
  using KBase::KMatrix;
  using KBase::PRNG;
  using KBase::VUI;
  using KBase::Actor;
  using KBase::Position;
  using KBase::State;
  using KBase::Model;
  using KBase::VotingRule;
  using KBase::ReportingLevel;
  using KBase::MtchPstn;


  class CSActor;
  class CSState;
  class CSModel;

  // -------------------------------------------------

  const string appName = "csdemo";
  const string appVersion = "0.2";

  // -------------------------------------------------
  // class declarations

  class CSModel : public Model {
  public:
    explicit CSModel(unsigned int nd, PRNG* r, string d="");
    virtual ~CSModel();
    

    static bool equivStates(const CSState * rs1, const CSState * rs2);
    
  protected:
    // the positions are matchings.
    // the number of items is the number of actors (e.g. political parties)
    // the number of categories is 2: out or in, respectively
    unsigned int numDims = 0;
  private:
  };

  
  class CSActor : public Actor  {
  public:
    explicit CSActor();
    virtual ~CSActor();
    
    VotingRule vr = VotingRule::PropBin; // fairly arbitrary default
    double sCap = 0.0; // scalar capacity, which must be positive
    
  protected:
  private:
  };

  
  class CSState : public State  {
  public:
    explicit CSState(CSModel* mod);
    virtual ~CSState();
    
    // use the parameters of your state to compute the relative probability of each unique position.
    // persp = -1 means use everyone's separate perspectives (i.e. get actual probabilities, not one actor's beliefs).
    // Because the voting mechanisms may differ, so v_k(i:j) could differ widely from sub-class to sub-class,
    // it is tricky to make a single function to do this.
    virtual tuple< KMatrix, VUI> pDist(int persp) const;
    
    CSState * stepSUSN();
    CSState * stepBCN();
    
    void show() const;
    
  protected:
    virtual bool equivNdx(unsigned int i, unsigned int j) const;
    virtual void setAllAUtil(ReportingLevel rl);

    CSState * doSUSN(ReportingLevel rl) const;
    CSState * doBCN(ReportingLevel rl) const;
    
  private:
  };

};// end of namespace

  
// --------------------------------------------
#endif
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
