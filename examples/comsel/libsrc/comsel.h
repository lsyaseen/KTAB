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

#include "csv_parser.hpp"
#include "sqlite3.h"
#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"
#include "gaopt.h"
#include "kmodel.h"

#include "smp.h"

namespace ComSelLib {
  // namespace to which KBase has no access
  using std::function;
  using std::shared_ptr;
  using std::string;
  using std::tuple;
  using std::vector;
  using KBase::newChars;
  using KBase::KMatrix;
  using KBase::PRNG;
  using KBase::Actor;
  using KBase::Position;
  using KBase::State;
  using KBase::Model;
  using KBase::VotingRule;
  using KBase::ReportingLevel;
  using KBase::MtchPstn;

  const string appVersion = "0.1";

  class CSModel : public Model {
  public:
    CSModel(unsigned int np, unsigned int nd, PRNG* r, string d="");
    virtual ~CSModel();
    
  protected:
    unsigned int numPrty = 0;
    unsigned int numDims = 0;
  private:
  };

  class CSActor : public Actor  {
  public:
    CSActor();
    virtual ~CSActor();
  protected:
  private:
  };

  class CSState : public State  {
  public:
  protected:
  private:
  };

};// end of namespace

// --------------------------------------------
#endif
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
