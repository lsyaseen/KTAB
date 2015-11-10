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
// This defines a very simple interface to some
// simple matrix operations. The basic implementation
// is straightforward and inefficient; feel free to subclass
// or modify as your problem requires (e.g. high performance
// parallel implementation for large, sparse matrices).
// "... rather antiquated, but adequate to our simple needs."
// -------------------------------------------------
#ifndef KMATRIX_H
#define KMATRIX_H

#include <cstdint>
#include <functional> 
#include <tuple>
#include <vector>

#include "kutils.h"

namespace KBase { 
  
  using std::function;
  using std::tuple;

  class KMatrix;
  class PRNG;

  KMatrix trans(const KMatrix & m);
  double  norm(const KMatrix & m);
  double  sum(const KMatrix & m);
  double  mean(const KMatrix & m);
  double  stdv(const KMatrix & m);
  double  maxAbs(const KMatrix & m);
  tuple<unsigned int, unsigned int>  ndxMaxAbs(const KMatrix & m);
  double  dot(const KMatrix & m1, const KMatrix & m2);
  double  lCorr(const KMatrix & m1, const KMatrix & m2);
  KMatrix  inv(const KMatrix & m);
  KMatrix iMat(unsigned int n);
  KMatrix makePerp(const KMatrix & x, const KMatrix & p);
  KMatrix  joinH(const KMatrix & mL, const KMatrix & mR);
  KMatrix  joinV(const KMatrix & mT, const KMatrix & mB);
  KMatrix operator+ (const KMatrix & m1, const KMatrix & m2);
  KMatrix operator+ (const KMatrix & m1, double x);
  KMatrix operator- (const KMatrix & m1, const KMatrix & m2);
  KMatrix operator- (const KMatrix & m1, double x);
  KMatrix operator* (double x, const KMatrix & m1);
  KMatrix operator/ (const KMatrix & m1, double x);
  bool sameShape(const KMatrix & m1, const KMatrix & m2);
  KMatrix operator* (const KMatrix & m1, const KMatrix & m2);


  class KMatrix {
    friend KMatrix  inv(const KMatrix & m);
  public:

    KMatrix();
    KMatrix(unsigned int nr, unsigned int nc);
    // default copy constructor, copy assigment, etc. are sufficient 
    double operator() (unsigned int i, unsigned int j) const;  // readable rvalue
    double& operator() (unsigned int i, unsigned int j);       // assignable lvalue
    void mPrintf(string) const;
    unsigned int numR() const;
    unsigned int numC() const;
    static KMatrix uniform(PRNG* rng, unsigned int nr, unsigned int nc, double a, double b);
    static KMatrix map(function<double(unsigned int i, unsigned int j)> f, unsigned int nr, unsigned int nc);
    static void mapV(function<void(unsigned int i, unsigned int j)> f, unsigned int nr, unsigned int nc);
    
    static KMatrix arrayInit(const double mv[], const unsigned int & rows, const unsigned int & clms);

    // For those rare cases when we do not need explicit indices inside the loop, 
    // the standard C++11 iterators are provided to support range-for
    vector<double>::iterator begin()  { return vals.begin(); };
    vector<double>::iterator end() { return vals.end(); };
    vector<double>::const_iterator cbegin() { return vals.cbegin(); };
    vector<double>::const_iterator cend() { return vals.cend(); };
    vector<double>::const_iterator begin() const { return vals.begin(); };
    vector<double>::const_iterator end() const { return vals.end(); };
    
    virtual ~KMatrix();

  private:
    void zeroFillVec(unsigned int nr, unsigned int nv);
    void pivot(unsigned int r, unsigned int c);
    unsigned int rows = 0;
    unsigned int clms = 0;
    vector<double> vals = vector<double>();
    inline unsigned int nFromRC(const unsigned int r, const unsigned int c) const;
    void rcFromN(const unsigned int n, unsigned int & r, unsigned int &c) const;
  };



};
// -------------------------------------------------
#endif

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
