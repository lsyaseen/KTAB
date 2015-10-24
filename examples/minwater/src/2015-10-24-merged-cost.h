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

#ifndef DEMO_WATERMIN_COST_H
#define DEMO_WATERMIN_COST_H

#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"

// --------------------------------------------
// "resource costs" are in this case tons of water per ton of product
// --------------------------------------------
namespace DemoWaterMin {
using KBase::KMatrix;
  // 38 rows, 1 column
//water/crop
const double rsrcCostArray [] = {
    0 ,
    2223 ,
    1887 ,
    3059 ,
    4753 ,
    2233 ,
    330 ,
    4281 ,
    3420 ,
    3556 ,
    502 ,
    524 ,
    1448 ,
    338 ,
    407 ,
    549 ,
    631 ,
    761 ,
    334 ,
    646 ,
    3374 ,
    634 ,
    330 ,
    137 ,
    243 ,
    330 ,
    427 ,
    594 ,
    4848 ,
    398 ,
    1544 ,
    6568 ,
    427 ,
    375 ,
    1457 ,
    183 ,
    276 ,
    0
};

const KMatrix rsrcCost = KMatrix::arrayInit(rsrcCostArray, 38, 1);

} ; // end of namespace
// --------------------------------------------
#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
