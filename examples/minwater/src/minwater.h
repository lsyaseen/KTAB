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

#ifndef DEMO_WATERMIN_H
#define DEMO_WATERMIN_H

#include <assert.h>
#include <chrono>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <vector>

#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"
#include "gaopt.h"
#include "hcsearch.h"
#include "kmodel.h"

namespace DemoWaterMin {
// namespace to which KBase has no access

using std::string;
using std::tuple;
using std::vector;

using KBase::KMatrix;
using KBase::PRNG;

using KBase::Actor;
using KBase::Position;
using KBase::State;
using KBase::Model;
using KBase::VotingRule;
using KBase::ReportingLevel;
 

const unsigned int numA = 38;
const unsigned int numP = 27;
// 38 rows/actors, 27 columns/positions
// Column 0 is the base-year, Column 1 is the 'nominal policy'


// all but the first are base-year total revenue
double w2Array[] {
   2000000.0 ,   // 01
    744645.7 ,   // 02
    369065.2 ,   // 03
    558530.62 ,  // 04
    126000.0 ,   // 05
    105623.2 ,   // 06
    602330.0 ,   // 07
    44558.6 ,    // 08
    16544.9 ,    // 09
    13349.8 ,    // 10
    857680.0 ,   // 11
    66344.0 ,    // 12
    76655.9 ,    // 13
    201451.7 ,   // 14
    44116.4 ,    // 15
    41444.3 ,    // 16
    469800.0 ,   // 17
    100017.5 ,   // 18
    199953.6 ,   // 19
    21577.1 ,    // 20
    7840.0 ,     // 21
    12394.8 ,    // 22
    35700.0 ,    // 23
    49397.2 ,    // 24
    23620.4 ,    // 25
    27200.0 ,    // 26
    14030.3 ,    // 27
    26520.0 ,    // 28
    807.5 ,      // 29
    28450.6 ,    // 30
    1352.0 ,     // 31
    1691.2 ,     // 32
    47359.2 ,    // 33
    44273.4 ,    // 34
    922.5 ,      // 35
    1173.3 ,     // 36
    8001.6 ,     // 37
    271.1        // 38
};

const KMatrix w0 = KMatrix::arrayInit(w2Array, numA, 1);

const double trgtP0 = 0.80; // probability of base-year scenario (#0) when waterMin actor has little strength
const double trgtP1 = 0.80; // probability of nominal-policy scenarios when waterMin actor has full strength
// these are the cases where dates are cut by 5% or less, and wheat is cut by 75% or more.
double prmsW = 0.020; //  weight of RMS weight-factors, compared to weight 1 of RMS prob-error
 

// -------------------------------------------------
// define a simple class of Linear Program that
// minimize resource usage, subject to three kinds of constraints:
// 1: Bounds on reduction or growth of each item
//      (1-r)*x0 <= x <= (1+g)*x0
// 2: Bounds on reduction of portfolio components. 
//    For example, total livestock production cannot fall more than 5%.
//      dot(p, x) >= (1-r) * dot(p, x0)
// 3: Minimum supply-to-meet-demand constraints.
//    For example, Livestock creates demand for nutrition, fodder supplies nutrition,
//    so fodder/livestock ratio cannot decrease: final fodder/livestock >= initial fodder/livestock
//    In symbols,
//      dot(s,x) * (dot(d, x0) / dot(s,x0)) >= dot(d, x)
//      dot(s, x) / dot(d, x) >= dot(s, x0) / dot(d, x0).
//    As the right hand side is a constant, r = dot(s,x0)/dot(d,x0), this means
//      dot(s,x) >= r * dot(d,x) or dot(s-r*d, x) >= 0

class RsrcMinLP {
public:
  RsrcMinLP();
  virtual ~RsrcMinLP();
  static RsrcMinLP* makeRMLP(PRNG* rng, unsigned int numPd, unsigned int numPt, unsigned int numSD);
  tuple<KMatrix, KMatrix> makeMq() const;

  unsigned int numProd = 0; // number of products
  KMatrix xInit = KMatrix();
  KMatrix rCosts = KMatrix();
  KMatrix bounds = KMatrix();
  // first column is max reduction fraction, second is max growth
  // i.e. (1-ri)*xInit <= x <= (1+gi)*xInit
  // Note that -1 <= gi < 0 is allowed to force reductions, provided
  // that 0 <= (1-r1) <= (1+gi)

  // a portfolio constraint says that the weighted sum cannot decline by more
  // than a certain percentage: dot (w, x) >= (1-r)*dot(w, x0).
  // One example would be a diet problem, where the amount of protein cannot fall below a threshold.
  unsigned int numPortC = 0; // number of portfolio constraints
  KMatrix portWghts = KMatrix(); // matrix of portfolio weights (all non-negative, probably all 0 or 1)
  KMatrix portRed = KMatrix(); // column vector of max reduction fractions
  
  // supply constraints say that the ratio of supply over demand cannot fall below the
  // initial value.
  // One example would be an agricultural output problem, where the ratio of 
  // cattle-feed over cattle-meat cannot fall below the initial value.
  unsigned int numSpplyC = 0; // number of supply constraints
  KMatrix spplyWghts = KMatrix(); // matrix of supply weights (all non-negative, probably all 0 or 1)
  KMatrix dmndWghts = KMatrix(); // matrix of demand weights (all non-negative, probably all 0 or 1)
  
  vector<string> pNames = {}; // product names

protected:
private:
  void clear();

};

}; // end of namespace

// -------------------------------------------------
#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
