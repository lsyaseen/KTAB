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
// Very simple interface to standard Mersenne Twister.
// -------------------------------------------------
#ifndef KTAB_PRNG_H
#define KTAB_PRNG_H

#include <cstdint>
#include <random> 

#include "kutils.h"

namespace KBase {
  using std::mt19937_64;

  uint64_t qTrans(uint64_t s);

  class PRNG {
  public:
    PRNG();
    virtual ~PRNG();
    uint64_t uniform();
    double uniform(double a, double b);
    vector<bool> bits(unsigned int nb);
    uint64_t setSeed(uint64_t);
  protected:
    mt19937_64 mt = mt19937_64();
  };

};

// -------------------------------------------------
#endif
// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
