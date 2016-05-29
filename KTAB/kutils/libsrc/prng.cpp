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
// -------------------------------------------------


#include <assert.h>

#include "prng.h"


namespace KBase {
  using std::cout;
  using std::endl;
  using std::flush;

W64 qTrans(W64 s) {
    W64 n = 2; // even, != 0
    W64 c = 3; // odd
    W64 a = 0xF1FF7B8227A447F5; // any number
    W64 r = (s + a) * ((n*s) + c);
    return r;
}

W64 rotl(const W64 x, unsigned int n) {
    W64 z = x;
    n = n % WordLength; // drop excessive loops
    if (0 != n) {
        z = (x << n) | (x >> (WordLength - n));
    }

    assert(z == (z & MASK64)); // just double-check
    assert(x == rotr(z, n)); // just double-check
    return z;
}

W64 rotr(const W64 x, unsigned int n) {
    W64 z = x;
    n = n % WordLength; // drop excessive loops
    if (0 != n) {
        z = (x >> n) | (x << (WordLength - n));
    }

    assert(z == (z & MASK64)); // just double-check
    return z;
}


PRNG::PRNG() {
    uint64_t seed_val = 0xD67CC16FE69C185C; // one of my favorite integers
    mt.seed(seed_val);
}


PRNG::~PRNG() { }


uint64_t PRNG::setSeed(uint64_t s) {
    if (0 == s) {
        std::random_device rd;
        mt19937_64 mt1(rd());
        std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);
        s = dist(mt1);
    }
    mt.seed(s);
    return s;
}


double PRNG::uniform(double a, double b) {
    uint64_t n = uniform();
    double x = ((double)n) / ((double)0xFFFFFFFFFFFFFFFF);
    assert(0.0 <= x);
    assert(x <= 1.0);
    x = a + ((b - a)*x);
    return x;
}

unsigned int PRNG::probSel (const KMatrix & cv) {
    const unsigned int nr = cv.numR();
    assert(0 < nr); 
    assert (1 == cv.numC());
    const double pTol = 1E-8;
    assert(fabs(KBase::sum(cv) - 1.0) < pTol);

    int iMax = -1;
    const double p = uniform(0.0, 1.0);
    double sum = 0.0;
    for (unsigned int i=0; (i<nr) && (iMax < 0); i++) {
        sum = sum + cv(i,0);
        if (p <= sum) {
            iMax = i;
        }
    }
    if (iMax < 0 ) { // round-off error
        iMax = nr - 1 ;
    }
    // obviously, now 0 <= iMax <= nr-1
    return ((unsigned int) iMax);
};


uint64_t PRNG::uniform() {
    const uint64_t max = 0xFFFFFFFFFFFFFFFF;
    std::uniform_int_distribution<uint64_t> dist(0, max);
    uint64_t n = dist(mt);
    return qTrans(n);
}

VBool PRNG::bits(unsigned int nb) {
  VBool bv = {};
    bv.resize(nb);
    uint64_t rNum = uniform(); // random bits
    const int nAvail = 64;
    int nUsed = 0;
    for (unsigned int i = 0; i < nb; i++) {
        bool b = (1 == (rNum & 0x1));
        rNum = rNum >> 1;
        nUsed = nUsed + 1;
        if (nAvail <= nUsed) {
            rNum = uniform();
            nUsed = 0;
        }
        bv[i] = b;
    }
    return bv;
}

} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
