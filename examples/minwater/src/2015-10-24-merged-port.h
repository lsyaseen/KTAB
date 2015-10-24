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

#ifndef DEMO_WATERMIN_PORT_H
#define DEMO_WATERMIN_PORT_H

#include "kutils.h"
#include "prng.h"
#include "kmatrix.h"

// --------------------------------------------
// "portfolio weights" are riyals/ ton in the first column,
// then 1/0 for whether it is in the product portfolio
// --------------------------------------------


namespace DemoWaterMin {
using KBase::KMatrix;
//revenue ,  Fodder ,  Grains ,  Fruit ,  Veg ,  Meat ,  Dairy
  // 38 rows, 7 columns
const double portCoefficientsArray[] {
    0 , 0 , 0 , 0 , 0 , 0 , 0  ,
    0.28 , 1 , 0 , 0 , 0 , 0 , 0  ,
    0.28 , 1 , 0 , 0 , 0 , 0 , 0  ,
    0.51 , 0 , 0 , 1 , 0 , 0 , 0  ,
    0.35 , 0 , 0 , 1 , 0 , 0 , 0  ,
    0.16 , 0 , 1 , 0 , 0 , 0 , 0  ,
    0.31 , 0 , 0 , 0 , 0 , 0 , 1  ,
    0.45 , 0 , 0 , 0 , 0 , 0 , 0  ,
    0.15 , 0 , 1 , 0 , 0 , 0 , 0  ,
    0.14 , 0 , 1 , 0 , 0 , 0 , 0  ,
    1.42 , 0 , 0 , 0 , 0 , 1 , 0  ,
    0.17 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.57 , 0 , 0 , 1 , 0 , 0 , 0  ,
    0.37 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.11 , 0 , 0 , 1 , 0 , 0 , 0  ,
    0.18 , 0 , 0 , 1 , 0 , 0 , 0  ,
    2.7 , 0 , 0 , 0 , 0 , 1 , 0  ,
    0.19 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.83 , 0 , 0 , 0 , 0 , 0 , 1  ,
    0.18 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.56 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.21 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.34 , 0 , 0 , 0 , 0 , 0 , 1  ,
    0.2 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.21 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.34 , 0 , 0 , 0 , 0 , 0 , 1  ,
    0.25 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.39 , 0 , 0 , 0 , 0 , 0 , 1  ,
    0.18 , 0 , 1 , 0 , 0 , 0 , 0  ,
    0.64 , 0 , 0 , 0 , 1 , 0 , 0  ,
    0.12 , 0 , 1 , 0 , 0 , 0 , 0  ,
    0.68 , 0 , 1 , 0 , 0 , 0 , 0  ,
    2.1 , 0 , 0 , 0 , 0 , 1 , 0  ,
    2.72 , 0 , 0 , 0 , 0 , 1 , 0  ,
    0.45 , 0 , 1 , 0 , 0 , 0 , 0  ,
    0.15 , 0 , 0 , 0 , 1 , 0 , 0  ,
    2.4 , 0 , 0 , 0 , 0 , 1 , 0  ,
    2.51 , 0 , 0 , 1 , 0 , 0 , 0

};

const KMatrix portCoefficients = KMatrix::arrayInit(portCoefficientsArray, 38, 7);

} ; // end of namespace
// --------------------------------------------
#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
