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

//#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>

#include "prng.h"
#include "kmatrix.h"
#include <easylogging++.h>


namespace KBase {

KMatrix subMatrix(const KMatrix & m1,
                  unsigned int i1, unsigned int i2,
                  unsigned int j1, unsigned int j2) {

    if (i1 > i2) {
      throw KException("subMatrix: i1 should not be greater than i2 ");
    }
    if (i2 > m1.numR()) {
      throw KException("subMatrix: i2 can't be more than rows of m1 matrix");
    }
    const unsigned int nr = 1 + (i2 - i1);

    if (j1 > j2) {
      throw KException("subMatrix: j1 should not be greater than j2");
    }
    if (j2 >= m1.numC()) {
      throw KException("subMatrix: j2 can't be more than columns of m1 matrix");
    }
    const unsigned int nc = 1 + (j2 - j1);

    KMatrix m2 = KMatrix(nr, nc);
    for (unsigned int i = 0; i < nr; i++) {
        for (unsigned int j = 0; j < nc; j++) {
            m2(i, j) = m1(i1 + i, j1 + j);
        }
    }
    return m2;
}

// return row-vector from row number i
KMatrix hSlice(const KMatrix & m1, unsigned int i) {
    auto m2 = subMatrix(m1, i, i, 0, m1.numC() - 1);
    return m2;
}


// return clm-vector from clm number j
KMatrix vSlice(const KMatrix & m1, unsigned int j) {
    auto m2 = subMatrix(m1, 0, m1.numR() - 1, j, j);
    return m2;
}

KMatrix trans(const KMatrix & m1) {
    unsigned int nr2 = m1.numC();
    unsigned int nc2 = m1.numR();
    auto m2 = KMatrix(nr2, nc2);
    for (unsigned int i = 0; i < nr2; i++) {
        for (unsigned int j = 0; j < nc2; j++) {
            m2(i, j) = m1(j, i);
        }
    }
    return m2;
}


double norm(const KMatrix & m1) {
    double s = 0.0;
    for (auto x : m1) {
        s = s + (x*x);
    }
    return sqrt(s);
}

double rms(const KMatrix& m) {
    auto n = norm(m);
    auto nr = m.numR();
    auto nc = m.numC();
    return n/sqrt(nr*nc);
}

KMatrix  unitize(const KMatrix & m) {
    double s = norm(m);
    return (m/s);
}

double sum(const KMatrix & m1) {
    double s = 0.0;
    for (auto x : m1) {
        s = s + x;
    }
    return s;
}


double mean(const KMatrix & m1) {
    return sum(m1) / (m1.numC()*m1.numR());
}


double stdv(const KMatrix & m1) {
    return norm(m1 - mean(m1)) / sqrt(m1.numR() * m1.numC());
}


double  maxAbs(const KMatrix & m) {
    double ma = 0;
    for (auto x : m) {
        double a = fabs(x);
        ma = (a > ma) ? a : ma;
    }
    return ma;
}

tuple<unsigned int, unsigned int>  ndxMaxAbs(const KMatrix & m) {
    const unsigned int nr = m.numR();
    const unsigned int nc = m.numC();

    // mark with obviously impossible values
    double ma = -1.0;
    unsigned int ndxI = 1+nr;
    unsigned int ndxJ = 1+nc;

    for (unsigned int i=0; i<nr; i++) {
        for (unsigned int j=0; j<nc; j++) {
            double a = fabs(m(i,j));
            if (ma < a) {
                ndxI = i;
                ndxJ = j;
                ma = a;
            }
        }
    }
    if (0.0 > ma) {
      throw KException("ndxMaxAbs: ma must be non-negative");
    }
    if (ndxI >= nr) {
      throw KException("ndxMaxAbs: invalid index for init actor");
    }
    if (ndxJ >= nc) {
      throw KException("ndxMaxAbs: invalid index for receiver actor");
    }
    auto ndx = tuple<unsigned int, unsigned int>(ndxI, ndxJ);
    return ndx;
}

// Notice that this is strictly, mathematically "linear" correlation,
// based on the model Y = aX.
// It is simple: a-est = (YX)/(XX), with correlation (YX)/sqrt((YY)(XX))
//
// When people say "linear correlation" and "linear model", they usually
// mean "affine", like Y = aX+b.
// If you want affine correlation, it is simply lCorr(y-mean(y), x-mean(x))
double  lCorr(const KMatrix & m1, const KMatrix & m2) {
    return dot(m1, m2) / sqrt(dot(m1, m1)*dot(m2, m2));
}


double  dot(const KMatrix & m1, const KMatrix & m2) {
    if (!sameShape(m1, m2)) {
      throw KException("dot: m1 and m2 matrices don't have same shapes");
    }
    double s12 = 0;
    for (unsigned int i = 0; i < m1.numR(); i++) {
        for (unsigned int j = 0; j < m1.numC(); j++) {
            s12 = s12 + m1(i, j)*m2(i, j);
        }
    }
    return s12;
}


KMatrix::KMatrix() {
    vFillVec(0, 0, 0.0); // totally empty
}

KMatrix::~KMatrix() {
    vFillVec(0, 0, 0.0); // totally empty
}


KMatrix::KMatrix(unsigned int nr, unsigned int nc, double iv) {
    vFillVec(nr, nc, iv);
}

// if double mv[] = { 11, 12, 13, 21, 22, 23 }, then
// mArrayInit (mv, 2, 3) yields
// 11  12  13
// 21  22  23
// Trying to embed the mv declaration in the mArrayInit
// call does not seem to work easily.
//
// You have to make very sure that the mv[] really is of size nr*nc. If not, it just reads in random memory.
KMatrix KMatrix::arrayInit(const double mv[], const unsigned int & nr, const unsigned int & nc) {
    KMatrix m = KMatrix(nr, nc);
    for (unsigned int i = 0; i < nr; i++) {
        for (unsigned int j = 0; j < nc; j++) {
            unsigned int n = m.nFromRC(i, j);
            m(i, j) = mv[n];
        }
    }
    return m;
}


void KMatrix::mPrintf(string fs, string msg) const {
    const char * fc = fs.c_str();
    string rowVals = msg;
    auto pf = [fc, this, &rowVals, msg](unsigned int i, unsigned int j) {
        rowVals += KBase::getFormattedString(fc, (*this)(i, j));
        if (j == (this->numC() - 1)) {
            LOG(INFO) << rowVals;
            // Reset the string object before processing next row in the matrix
            rowVals.clear();
        }
        return;
    };
    mapV(pf, rows, clms);
    return;
}


void KMatrix::vFillVec(unsigned int nr, unsigned int nc, double iv) {
    rows = nr;
    clms = nc;

    const unsigned int n = nr*nc;
    vals = {}; // 0-length vector
    vals.resize(n);
    for (unsigned int i = 0; i < n; i++) {
        vals[i] = iv;
    }
    return;
}


double KMatrix::operator () (unsigned int i, unsigned int j) const {
    const unsigned int n = nFromRC(i, j);
    return vals[n]; // rvalue
}


double& KMatrix::operator() (unsigned int i, unsigned int j) {
    const unsigned int n = nFromRC(i, j);
    return vals[n]; // lvalue
}


unsigned int KMatrix::numR() const {
    return rows;
}
unsigned int KMatrix::numC() const {
    return clms;
}


unsigned int KMatrix::nFromRC(const unsigned int r, const unsigned int c) const {
    if (r >= rows) {
      throw KException("KMatrix::nFromRC: r can not be more than number of rows");
    }
    if (c >= clms) {
      throw KException("KMatrix::nFromRC: c can not be more than number of columns");
    }
    return (r*clms + c);
}


void KMatrix::rcFromN(const unsigned int n, unsigned int & r, unsigned int &c) const {
    if (n >= rows*clms) {
      throw KException("KMatrix::rcFromN: n must be less than total number of elements");
    }
    r = n / clms;
    c = n - (r*clms);
    if (r >= rows) {
      throw KException("KMatrix::rcFromN: row number is invalid");
    }
    if (c >= clms) {
      throw KException("KMatrix::rcFromN: column number is invalid");
    }
    return;
}


KMatrix operator+ (const KMatrix & m1, double x) {
    auto af = [x, &m1](unsigned int i, unsigned int j) {
        return m1(i, j) + x;
    };
    return KMatrix::map(af, m1.numR(), m1.numC());
}


KMatrix operator- (const KMatrix & m1, double x) {
    auto sf = [x, &m1](unsigned int i, unsigned int j) {
        return m1(i, j) - x;
    };
    return KMatrix::map(sf, m1.numR(), m1.numC());
}


bool sameShape(const KMatrix & m1, const KMatrix & m2) {
    const bool sameR = (m1.numR() == m2.numR());
    const bool sameC = (m1.numC() == m2.numC());
    return (sameR && sameC);
}


KMatrix operator+ (const KMatrix & m1, const KMatrix & m2) {
    if (!sameShape(m1, m2)) {
      throw KException("operator+: m1 and m2 matrices are not of same shape");
    }
    auto af = [m1, m2](unsigned int i, unsigned int j) {
        return m1(i, j) + m2(i, j);
    };
    return KMatrix::map(af, m1.numR(), m1.numC());
}


KMatrix operator- (const KMatrix & m1, const KMatrix & m2) {
  if (!sameShape(m1, m2)) {
    throw KException("operator-: m1 and m2 matrices are not of same shape");
  }
  auto sf = [&m1, &m2](unsigned int i, unsigned int j) {
        return m1(i, j) - m2(i, j);
    };
    return KMatrix::map(sf, m1.numR(), m1.numC());
}


KMatrix operator* (double x, const KMatrix & m1) {
    auto mf = [x, &m1](unsigned int i, unsigned int j) {
        return x*m1(i, j);
    };
    return KMatrix::map(mf, m1.numR(), m1.numC());
}


KMatrix operator* (const KMatrix & m1, double x) {
    auto mf = [x, &m1](unsigned int i, unsigned int j) {
        return x*m1(i, j);
    };
    return KMatrix::map(mf, m1.numR(), m1.numC());
}


KMatrix operator/ (const KMatrix & m1, double x) {
    auto df = [x, &m1](unsigned int i, unsigned int j) {
        return m1(i, j) / x;
    };
    return KMatrix::map(df, m1.numR(), m1.numC());
}


KMatrix operator* (const KMatrix & m1, const KMatrix & m2) {
    const unsigned int nr3 = m1.numR();
    const unsigned int nm3 = m1.numC();
    if (nm3 != m2.numR()) {
      throw KException("operator*: m1 and m2 matrices don't qualify for matrix multiplication");
    }
    const unsigned int nc3 = m2.numC();
    auto f = [nm3, &m1, &m2](unsigned int i, unsigned int j) {
        double sij = 0.0;
        for (unsigned int k = 0; k < nm3; k++) {
            sij = sij + m1(i, k)*m2(k, j);
        }
        return sij;
    };
    return KMatrix::map(f, nr3, nc3);
}


KMatrix KMatrix::map(function<double(unsigned int i, unsigned int j)> f, unsigned int nr, unsigned int nc) {
  if (f == nullptr) {
    throw KException("KMatrix::map: f is a null pointer");
  }
  auto m = KMatrix(nr, nc);
    for (unsigned int i = 0; i < nr; i++) {
        for (unsigned int j = 0; j < nc; j++) {
            m(i, j) = f(i, j);
        }
    }
    return m;
}

KMatrix KMatrix::map(function<double(double mij, unsigned int i, unsigned int j)> f,
                     const KMatrix & mat) {
    if (f == nullptr) {
      throw KException("KMatrix::map: f is a null pointer");
    }
    const unsigned int nr = mat.numR();
    const unsigned int nc = mat.numC();
    auto m = KMatrix(nr,nc);
    for (unsigned int i = 0; i < nr; i++) {
        for (unsigned int j = 0; j < nc; j++) {
            m(i, j) = f(mat(i,j), i, j);
        }
    }
    return m;
}


KMatrix KMatrix::map(function<double(double x)> f,
                     const KMatrix & mat) {
    if (f == nullptr) {
      throw KException("KMatrix::map: f is a null pointer");
    }
    const unsigned int nr = mat.numR();
    const unsigned int nc = mat.numC();
    auto m = KMatrix(nr,nc);
    for (unsigned int i = 0; i < nr; i++) {
        for (unsigned int j = 0; j < nc; j++) {
            m(i, j) = f(mat(i,j));
        }
    }
    return m;
}


void KMatrix::mapV(function<void(unsigned int i, unsigned int j)> f, unsigned int nr, unsigned int nc) {
    if (f == nullptr) {
      throw KException("KMatrix::mapV: f is a null pointer");
    }
    for (unsigned int i = 0; i < nr; i++) {
        for (unsigned int j = 0; j < nc; j++) {
            f(i, j);
        }
    }
    return;
}


KMatrix KMatrix::uniform(PRNG* rng, unsigned int nr, unsigned int nc, double a, double b) {
    auto rf = [rng, a, b](unsigned int, unsigned int) {
        return rng->uniform(a, b);
    };
    return map(rf, nr, nc);
}


void KMatrix::pivot(unsigned int r, unsigned int c) {
    auto x = [this](unsigned int i, unsigned int j) {
        return vals[nFromRC(i, j)];
    };
    double xrc = x(r, c);
    double xrcAbs = fabs(xrc);
    const double minPivot = 1E-8;
    if (xrcAbs <= minPivot) { // nearly-singular?
      throw KException("KMatrix::pivot: xrcAbs is very low");
    }

    // pivot the four blocks outside the main row and main column
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < clms; j++) {
            if ((r != i) && (c != j)) {
                double vij = x(i, j) - (x(i, c)*x(r, j)) / xrc;
                (*this)(i, j) = vij;
            }
        }
    }

    // pivot the main row
    for (unsigned int j = 0; j < clms; j++) {
        if (c != j) {
            double vrj = x(r, j) / xrc;
            (*this)(r, j) = vrj;
        }
    }

    // pivot the main column
    for (unsigned int i = 0; i < rows; i++) {
        if (r != i) {
            (*this)(i, c) = 0;
        }
    }

    // finally the pivot element itself
    (*this)(r, c) = 1;
    return;
}

// textbook algorithm
KMatrix inv(const KMatrix & m) {
    const unsigned int n = m.numR();
    if (n != m.numC()) {
      throw KException("inv: m is not a square matrix");
    }

    KMatrix m2 = joinH(KMatrix(m), iMat(n));
    VBool ok = {};
    ok.resize(n);
    for (unsigned int i = 0; i < n; i++) {
        ok[i] = true; // at start, ok to pivot on any
    }

    for (unsigned int iter = 0; iter < n; iter++) {
        double maxD = -1;
        unsigned int pk = 0;
        for (unsigned int k = 0; k < n; k++) {
            if (ok[k]) {
                const double mk = m2(k,k);
                const double pe = fabs(mk);
                if (pe > maxD) {
                    maxD = pe;
                    pk = k;
                }
            }
        }
        if (0 >= maxD) {
          throw KException("KMatrix::inv: maxD should be positive");
        }
        m2.pivot(pk, pk);
        ok[pk] = false;
    }

    auto m3 = KMatrix(n, n);
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n; j++) {
            m3(i, j) = m2(i, n + j);
        }
    }
    return m3;
}


KMatrix clip(const KMatrix & m, double xMin, double xMax) {
    if (xMin > xMax) {
      throw KException("clip: xMin can not be more than xMax");
    }
    const unsigned int nr = m.numR();
    const unsigned int nc = m.numC();
    auto cfn = [m, xMin, xMax] (unsigned int i, unsigned int j) {
        double mij = m(i,j);
        mij = (mij < xMin) ? xMin : mij;
        mij = (xMax < mij) ? xMax : mij;
        return mij;
    };
    return KMatrix::map(cfn, nr, nc);
}


KMatrix iMat(unsigned int n) {
    auto idm = KMatrix(n, n); // all 0.0
    for (unsigned int i = 0; i < n; i++) {
        idm(i, i) = 1.0;
    }
    return idm;
}

bool iMatP(const KMatrix & m) {
    const unsigned int nr = m.numR();
    bool okP = (nr == m.numC());
    if (okP) {
        const double tol = 1E-10;
        // tolerable round-off error on a [0,1] range
        const auto im = iMat(nr);
        okP = (norm(m - im) < tol);
    }
    return okP;
}

// return y to min |y-x| s.t. perpendicular(y,p)
KMatrix makePerp(const KMatrix & x, const KMatrix & p) {
    double lambda = dot(x,p) / dot(p,p);
    auto y = x - lambda*p;
    return y;
}

KMatrix  joinH(const KMatrix & mL, const KMatrix & mR) {
    unsigned int nr3 = mL.numR();
    unsigned int nc1 = mL.numC();
    if (nr3 != mR.numR()) {
      throw KException("joinH: mL and mR can not be joined");
    }
    unsigned int nc2 = mR.numC();
    auto m3 = KMatrix(nr3, nc1 + nc2);
    for (unsigned int i = 0; i < nr3; i++) {
        for (unsigned int j = 0; j < nc1; j++) {
            m3(i, j) = mL(i, j);
        }
        for (unsigned int j = 0; j < nc2; j++) {
            m3(i, j + nc1) = mR(i, j);
        }
    }
    return m3;
}


KMatrix  joinV(const KMatrix & mT, const KMatrix & mB) {
    unsigned int nr1 = mT.numR();
    unsigned int nr2 = mB.numR();
    unsigned int nc3 = mT.numC();
    if (nc3 != mB.numC()) {
      throw KException("joinV: mT and mB can not be joined");
    }
    auto m3 = KMatrix(nr1 + nr2, nc3);
    for (unsigned int j = 0; j < nc3; j++) {
        for (unsigned int i = 0; i < nr1; i++) {
            m3(i, j) = mT(i, j);
        }
        for (unsigned int i = 0; i < nr2; i++) {
            m3(i + nr1, j) = mB(i, j);
        }
    }
    return m3;
}

KMatrix rescaleRows(const KMatrix& m1, const double vMin, const double vMax) {
    if (vMin >= vMax) {
      throw KException("rescaleRows: vMax can not be less than vMin");
    }
    const unsigned int nr = m1.numR();
    const unsigned int nc = m1.numC();
    KMatrix m2 = KMatrix(nr, nc);

    for (unsigned int i = 0; i < nr; i++) {
        double rowMin = m1(i, 0);
        double rowMax = m1(i, 0);
        for (unsigned int j = 0; j < nc; j++) {
            const double mij = m1(i, j);
            if (mij < rowMin) {
                rowMin = mij;
            }
            if (mij > rowMax) {
                rowMax = mij;
            }
        }
        const double rowRange = rowMax - rowMin;
        if (0 >= rowRange) {
          throw KException("rescaleRows: rowRange must be positive");
        }

        for (unsigned int j = 0; j < nc; j++) {
            const double mij = m1(i, j);
            const double nij = (mij - rowMin) / rowRange; // normalize into [0, 1]
            double rij = vMin + (vMax - vMin)*nij; // rescale into [vMin, vMax]

            // fix tiny round-off errors
            if (rij < vMin) {
                rij = vMin;
            }
            if (vMax < rij) {
                rij = vMax;
            }

            m2(i, j) = rij;
        }
    }
    return m2;
}

// JAH 20160809 create a matrix by reshaping from a vector
    // BPW 20170403 more consistent name, without double-mention of KMatrix in KMatrix::vecToKmat
KMatrix KMatrix::vecInit(const vector<double> & vec, const unsigned int nr, const unsigned int nc)
{
    // be sure the size of the desired matrix is consistent with the data
    if (vec.size() != nr*nc) {
      throw KException("KMatrix::vecInit: size of the matrix is not consistent with the data");
    }

    // because we are subtracting from unsigned int, make sure the result does not underflow
    if (1 > nc) {
      throw KException("KMatrix::vecInit: nc must not be less than 1");
    }
    if (1 > nr) {
      throw KException("KMatrix::vecInit: nr must not be less than 1");
    }

    // init to the right size ...
    auto mat = KMatrix(nr,nc,0.0);
    int rowShift = 0; // rowshift is used to "block" chunks of data in the vector to create rows
    // ... then fill in the data from the vector
    for(unsigned int r = 0; r<nr; r++)
    {
        for(unsigned int c = 0; c<nc; c++)
        {
            mat(r,c) = vec[r+rowShift+c];
        }
        rowShift += (nc-1);
    }
    return mat;
}


KMatrix firstEigenvector( const KMatrix& A, double tol) {
    const unsigned int n = A.numR();
    if (A.numC() != n) { // must be square
      throw KException("firstEigenvector: A is not a square matrix");
    }
    if (1 >= n) {
      throw KException("firstEigenvector: n must be greater than 1");
    }
    if (0.0 >= tol) {
      throw KException("firstEigenvector: tol must be positive");
    }

    auto mDelta = [](const KMatrix& m1, const KMatrix& m2) {
        auto diff = norm(m1-m2);
        auto sum = norm(m1)+norm(m2);
        auto d = (2.0 *diff)/sum;
        return d;
    };


    double change = 2.0 * tol;
    unsigned int iter = 0;
    const unsigned int maxIter = 10000;
    auto x = unitize(KMatrix(n, 1, 1.0));

    while (change > tol) {
        auto y = unitize(A*x);

        // eigenvalue is dot(y,x)/dot(x,x), and x is unit-length
        auto eVal = dot(y,x);
        if (eVal < 0.0) { // avoid near-cancellation when the vector flips signs
            y = y *(-1.0); // would be nice to have unitary -y operator
        }
        change = mDelta(x,y);

        if (false) {
            LOG(INFO) << "X";
            x.mPrintf("%+.4f ");
            LOG(INFO) << "Y";
            y.mPrintf("%+.4f ");
            LOG(INFO) << KBase::getFormattedString("At iteration %u, delta is %.4e", iter, change);
        }

        x = unitize((x+y)/2.0); // reduces oscillations
        iter++;
        if (iter > maxIter) {
            throw KException("firstEigenvector:: iteration limit exceeded");
        }
    }

    if (true) {
        LOG(INFO) << KBase::getFormattedString("After iteration %u, delta is %.4e", iter, change);
    }

    // The eigenvector is unique only up to the sign.
    // So when the answer can be all negative or all positive,
    // we prefer the all positive version.
    auto xSum = sum(x);
    if (xSum < 0.0) {
        x = x * (-1.0);
    }
    return x;
}

} // end of namespace

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
