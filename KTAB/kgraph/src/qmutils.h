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
// start of a quadratic map
//---------------------------------------------
#ifndef QUADMAP_UTILS_H
#define QUADMAP_UTILS_H

#include <assert.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <functional>
#include <future>
#include <math.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <FL/Enumerations.H>
#include "kutils.h"
#include "prng.h"
#include "kgraph.h"

//---------------------------------------------

using std::cout;
using std::endl;
using std::flush;

using KBase::KMatrix;
using KBase::PRNG;
using KGraph::Canvas;

//---------------------------------------------

namespace QuadMap {

class QMApp;

// this fills in a matrix with occurance-counts
// The N rows are the X coordinate, 0 to 1
// The M columns are the 'a' parameter, from aLow to aHigh

double xVal(unsigned int i, unsigned int N);
unsigned int iVal(double x, unsigned int N);
double aVal(unsigned int j, unsigned int M, double aLow, double aHigh);


void testX(unsigned int N);

KMatrix fillOccuranceMatrix(unsigned int N, unsigned int M,  double aLow, double aHigh);

}; // end of namespace

//---------------------------------------------
#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
