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
// Define a few generally useful functions.
//
// BTW, this is the header for which Findkutils.cmake will
// look to verify that is has located the right directory
// for the whole set of related header files.
// -------------------------------------------------
#ifndef KBASE_UTIL_H
#define KBASE_UTIL_H

#include <assert.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>  
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

namespace KBase {
using std::string;
using std::vector;

class PRNG;
class KMatrix;

enum class ReportingLevel {
    Silent, Low, Medium, High, Debugging
};


std::chrono::time_point<std::chrono::system_clock>  displayProgramStart();
void displayProgramEnd(std::chrono::time_point<std::chrono::system_clock> st);

// linearly (actually, affine) rescale the [x0,x1] range into the [y0,y1] range
double rescale(double x, double x0, double x1, double y0, double y1);

// I find it irritating that the 'pop_xxx' operations of C++11 all return void,
// unlike the "pop" operations taught in CS courses which return the popped element.
// Note the use of a reference to avoid the problem of modifying only a copy.
template<typename T> T popBack(vector<T> & v) {
    unsigned int n = v.size();
    assert(0 < n);
    auto el = v[n - 1];
    v.pop_back();
    return el;
}


class KException {  // just something to throw around
public:
    KException(string m);
    virtual ~KException();
    string msg;
};


class CoordMap {
    /// map back and forth between screen and domain coords
public:
    CoordMap(int s1, double d1, int s2, double d2);
    virtual ~CoordMap();

    int d2s (double d);
    double s2d (int s);

protected:
    double as;
    double bs;

    double ad;
    double bd;

private:
    const bool testMap = true; // for a while
    int cmRound(double x);
};

}; // namespace
// -------------------------------------------------
#endif

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
