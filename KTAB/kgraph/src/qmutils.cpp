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

#include "qmutils.h"

//---------------------------------------------

using std::cout;
using std::endl;
using std::flush;

//---------------------------------------------

namespace QuadMap {


double xVal(unsigned int i, unsigned int N) {
    assert (i < N);
    return ((double) i)/((double) N);
};

unsigned int iVal(double x, unsigned int N) {
    double f = x * N;
    unsigned int i = ((unsigned int) (f + 0.0));
    assert (i < N);
    return i;
}

bool testI(unsigned int i1, unsigned int N) {
    double x = xVal(i1, N);
    unsigned int i2 = iVal(x, N);
    bool match = (i1 == i2);
    if (!match) {
        printf(" %3i => %.4f => %3i \n", i1, x, i2);
    }
    return match;
}

void testX(unsigned int N) {
    for (unsigned int i =0; i<N; i++) {
        testI(i,N);
    }
    return;
}

double aVal(unsigned int j, unsigned int M, double aLow, double aHigh) {
    assert (j < M);
    double m1 = M - 1.0;
    double a = (j*aHigh + (m1-j)*aLow)/m1;
    return a;
}


KMatrix fillOccuranceMatrix(unsigned int N, unsigned int M,  double aLow, double aHigh) {
    const unsigned int tNum = 100;
    const unsigned int iNum = 17;
    const unsigned int cNum = 50;
    auto om = KMatrix(N, M);
    // scan across the columns, for a
    for (unsigned int jc = 0; jc<M; jc++) {
        const double a = aVal(jc, M, aLow, aHigh);
        // for each of several initial x values, get over transients then iterate
        for (unsigned int ir = 0; ir<iNum; ir++) {
            double x = xVal(ir, iNum);
            for (unsigned int t=0; t<tNum; t++) {
                x = a * x * (1.0 - x);
            }
            for (unsigned int t = 0; t<cNum; t++) {
                x = a * x * (1.0 - x);
                unsigned int ix = iVal(x,N);
                om(ix,jc) = 1.0 + om(ix,jc);
            }
        }

    }

    return om;
}

}; // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
