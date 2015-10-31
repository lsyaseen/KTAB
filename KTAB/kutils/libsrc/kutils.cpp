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
// Define a few generally useful functions.
// -------------------------------------------------

#include <assert.h>
#include <iostream>
#include <tuple>

#include "kutils.h"
#include "prng.h"

namespace KBase {

using std::cout;
using std::endl;
using std::flush;


// -------------------------------------------------

std::chrono::time_point<std::chrono::system_clock>  displayProgramStart() {
  std::chrono::time_point<std::chrono::system_clock> st;
  st = std::chrono::system_clock::now();
  std::time_t start_time = std::chrono::system_clock::to_time_t(st);
  cout << "  Start time: " << std::ctime(&start_time) << endl << flush;
  return st;
}

void displayProgramEnd(std::chrono::time_point<std::chrono::system_clock> st) {
  std::chrono::time_point<std::chrono::system_clock> ft;
  ft = std::chrono::system_clock::now();
  std::chrono::duration<double> eTime = ft - st;
  std::time_t fTime = std::chrono::system_clock::to_time_t(ft);
  cout << "  Finish time: " << std::ctime(&fTime) << endl << flush;
  printf("  Elapsed time: %.4f seconds \n", eTime.count());
  return;
}


char* newChars(unsigned int len) {
  auto rslt = new char[len];
  for (unsigned int i = 0; i < len; i++) {
    rslt[i] = ((char)0);
  }
  return rslt;
};

double rescale(double x, double x0, double x1, double y0, double y1) {
    assert ((x0 < x1) ||( x1 < x0));
    const double f = (x-x0)/(x1-x0);
    return y0 + (y1-y0)*f;
}


// -------------------------------------------------
KException::KException(string m) {
    msg = m;
}

KException::~KException() { }


// -------------------------------------------------
CoordMap::CoordMap(int s1, double d1, int s2, double d2) {
    ad = (d2 - d1)/((double)(s2 - s1));
    bd = ((s2*d1) - (s1*d2))/((double)(s2 - s1));

    as = ((double)(s2 - s1))/(d2 - d1);
    bs = ((s1*d2)-(s2*d1))/(d2-d1);

    if (testMap) {
        // quickly test the transforms
        const double dTol = fabs(d2-d1) / 1.0E6;

        auto s2dTest = [dTol, this] (double dA, int sA) {
            assert (fabs(dA - s2d(sA)) < dTol);
            return;
        };

        s2dTest(d1, s1);
        s2dTest(d2, s2);

        auto d2sTest = [this] (int sA, double dA) {
            assert (sA == d2s(dA));
            return;
        };

        d2sTest(s1, d1);
        d2sTest(s2, d2);
    }
}


CoordMap::~CoordMap() {
    as = 0;
    bs = 0;
    ad = 0;
    bd = 0;
}

int CoordMap::d2s (double d) {
    int s = cmRound(as*d + bs);
    return s;
}

double CoordMap::s2d (int s) {
    double d = ad*s + bd;
    return d;
}


int CoordMap::cmRound(double x) {
    int y = ((int) (x+0.5));
    if (x < 0.0) {
        y = - cmRound(-x);
    }
    return y;
}

}; // namespace


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
