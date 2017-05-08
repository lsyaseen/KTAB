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
// Demonstrate some basic utility functions from
// the fundamental KBase namespace
// -------------------------------------------------
// To get it to run under Linux, you need to provide extra
// compile options (as of March 2015).
// With clang++, you need to specify -pthread
// With g++, you need to specify -std=c++11 -pthread
// Under Windows, with Visual Studio 2013, neither.
// This is setup in the CMakeLists.txt file.
//
// -------------------------------------------------

#include <inttypes.h>
#include "demo.h"
#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

using KBase::newChars;
using KBase::PRNG;
using KBase::KMatrix;
using KBase::VHCSearch;
using KBase::GHCSearch;
using KBase::getFormattedString;

// -------------------------------------------------
namespace UDemo {
using std::function;
using std::get;
using std::string;
using std::mutex;
using std::thread;

using KBase::ReportingLevel;
using KBase::VUI;
using KBase::nProd;


void demoPCA(PRNG* rng);

// return a tuple of updated weights and updated features (one per row)
// xMat has one sample per row, one column per dimension
// ftr has one component per row, one column per dimension
// wght has one sample per row, one component per column.
tuple<KMatrix, KMatrix> extendPCA (const KMatrix& xMat,
                                   const KMatrix& wght,
                                   const KMatrix& ftr);

void demoUIndices() {
    auto showIS = [](KBase::VUI is) {
        for (unsigned int i = 0; i < is.size(); i++) {
            LOG(DEBUG) << KBase::getFormattedString("%2u: %2i", i, is[i]);
        }
        return;
    };
    auto is1 = KBase::uiSeq(10, 19);
    LOG(DEBUG) << "Space by 1:";
    showIS(is1);
    auto is2 = KBase::uiSeq(10, 19, 2);
    LOG(DEBUG) << "Space by 2:";
    showIS(is2);

    VUI xs = { 10, 11, 20, 12, 30,  9, 23, 29, 40, 22, 43 };
    auto eFn = [](const int &a, const int &b) {
        return (abs(a - b) <= 3); // so 10 is equivalent to anything in [7 ... 13]
    };

    auto uePair = KBase::ueIndices<unsigned int>(xs, eFn);
    VUI uns = get<0>(uePair);
    VUI ens = get<1>(uePair);

    LOG(DEBUG) << "Items:"; // should be [0,2,4,8]
    showIS(xs);
    LOG(DEBUG) << "Indices of unique items:"; // should be [0,2,4,8]
    showIS(uns);
    LOG(DEBUG) << "Indices of equivalent items:"; // should be [0,2,4,8]
    showIS(ens);
    return;
}

void show(string str, const KMatrix & m, string fs) {
    LOG(DEBUG) << str;
    m.mPrintf(fs.c_str());
    return;
}


double bsu(double d, double R) {
    double u = KBase::quadUfromV(1.0-d, R);
    return u;
}

double bvu(const KBase::KMatrix & d, const KBase::KMatrix & s, double R) {
    // same as Model::bvUtil but we do not want to depend on anything but kutils
    assert(KBase::sameShape(d, s));
    double dsSqr = 0;
    double ssSqr = 0;
    for (unsigned int i = 0; i < d.numR(); i++) {
        for (unsigned int j = 0; j < d.numC(); j++) {
            double dij = d(i, j);
            double sij = s(i, j);
            assert(0 <= sij);
            double ds = dij * sij;
            double ss = sij * sij;
            dsSqr = dsSqr + (ds*ds);
            ssSqr = ssSqr + ss;
        }
    }
    assert(0 < ssSqr);
    double sd = sqrt(dsSqr / ssSqr);
    double u = bsu(sd, R);
    return u;
};

// -------------------------------------------------
void demoThreadLambda(unsigned int n) {
    // Interestingly, this starts all CPU's right away,
    // unlike parallelMatrixMult.
    auto ts = vector<thread>();
    auto ifn = [](const unsigned int s, const unsigned int m) {
        unsigned int k = s + 114367;
        for (unsigned int i = 0; i < m; i++) {
            k = (k + 1)*(2 * k + 1);
        }
        if (0 == k) {
            k = 117;
        }
        return k;
    };

    assert(0 < ifn(7, 10));

    for (int i = 0; i < n; ++i) {
        ts.push_back(std::thread([i, ifn]() {
            string hello("Hello ");
            assert(0 < ifn(i, 500000000));
            hello += "from ";
            assert(0 < ifn(i, 500000000));
            hello += "thread";
            assert(0 < ifn(i, 500000000));
            LOG(DEBUG) << hello << i;
            assert(0 < ifn(i, 500000000));
            return;
        }));
    }

    for (auto& t : ts) {
        t.join();
    }

    auto fn2 = [](unsigned int i) {
        assert(0 < i);
        return;
    };
    KBase::groupThreads(fn2, 17, 45);
    return;
}

void demoThreadSynch(unsigned int n) {
    // define a local object
    struct Counter {
        int value;

        Counter() : value(0) {}

        void increment() {
            value = value + 1;
            return;
        }

        void decrement() {
            if (value <= 0) {
                throw "Counter::decrement value cannot be non-positive";
            }
            value = value - 1;
            return;
        }
    };

    struct CCounter {
        Counter* c = nullptr;
        mutex m; // one mutex per concurrent-counter object

        explicit CCounter(Counter* c0) {
            assert(nullptr != c0);
            c = c0;
        };

        void increment() {
            std::lock_guard<mutex> guard(m); // lock at creation, unlock at deconstruction
            c->increment();
            return;
        }

        void decrement() {
            std::lock_guard<mutex> guard(m);
            c->decrement();
            return;
        }
    };

    auto counter = new Counter();  // one shared Counter
    CCounter cc(counter); // one concurrent counter

    vector<thread> ts;
    for (int i = 0; i < n; ++i) {
        ts.push_back(thread([&cc]() {
            for (int i = 0; i < 100; ++i) {
                cc.increment();
            }
        }));
    }

    for (auto& t : ts) {
        t.join();
    }

    LOG(DEBUG) << counter->value;

    delete counter;
    counter = nullptr;

    return;
}

// -------------------------------------------------
void demoMatrix(PRNG* rng) {

    using KBase::KMatrix;
    using KBase::iMat;
    using KBase::joinH;
    using KBase::inv;
    using KBase::norm;

    KMatrix m0;
    auto m1 = KMatrix(2, 3);
    LOG(DEBUG) << "Small zero-filled matrix";
    m1.mPrintf(" %+5.2f ");
    m1(0, 0) = 4.03;
    m1(0, 1) = 2.19;
    m1(0, 2) = 6.34;
    m1(1, 0) = 8.40;
    m1(1, 1) = 6.65;
    m1(1, 2) = 2.36;
    LOG(DEBUG) << getFormattedString("Set (0,1) element to %+5.2f", m1(0, 1));
    LOG(DEBUG) << getFormattedString("Set (1,2) element to %+5.2f", m1(1, 2));
    m1.mPrintf(" %5.2f ");

    LOG(DEBUG) << "Computing norm ...";
    double nm = norm(m1);
    LOG(DEBUG) << "norm is" << nm;

    double c1 = 2.3;
    LOG(DEBUG) << getFormattedString("Pre- and post-multiply c1 and m1, where c1=%5.2f and m1=", c1);
    m1.mPrintf(" %5.2f");

    LOG(DEBUG) << "Pre-multiply c1 * m1 =";
    auto c1m1 = c1 * m1;
    c1m1.mPrintf(" %5.2f");

    LOG(DEBUG) << "Post-multiply m1*c1 =";
    auto m1c1 = m1 * c1;
    m1c1.mPrintf(" %5.2f");



    LOG(DEBUG) << "Transposition then copy";
    m0 = trans(m1); // test transpose and default constructors
    m0.mPrintf(" %5.2f ");

    LOG(DEBUG) << "Demo strictly-linear correlation";
    auto X = KMatrix::uniform(rng, 5, 8, -10, +10);
    auto ns = KMatrix::uniform(rng, 5, 8, -5, +5);
    double aAct = -2.7;
    auto Y = aAct*X + ns;
    LOG(DEBUG) << "Independent vars in X:";
    X.mPrintf(" %+6.2f ");

    LOG(DEBUG) << "Dependent vars in Y:";
    Y.mPrintf(" %+6.2f ");
    double aEst = KBase::dot(Y, X) / KBase::dot(X, X);
    LOG(DEBUG) << getFormattedString("aAct: %+.4f", aAct);
    LOG(DEBUG) << getFormattedString("aEst: %+.4f", aEst);
    double lcorr = KBase::lCorr(Y, X);
    LOG(DEBUG) << getFormattedString("Measured correlation is %+.5f", lcorr);

    auto qtDemo = [](uint64_t s0) {
        uint64_t s1 = KBase::qTrans(s0);
        LOG(DEBUG) << getFormattedString("s0: 0x%016llX", s0);
        LOG(DEBUG) << getFormattedString("s1: 0x%016llX", s1);
        return;
    };

    LOG(DEBUG) << "qTrans:";
    qtDemo(0);
    qtDemo(1);
    qtDemo(2);
    qtDemo(3);

    // if X ~ U[-sqrt(3),+sqrt3()] then mean(X)=0, stdv(X)=1
    unsigned int nr = 1500;
    unsigned int nc = 750;
    LOG(DEBUG) << getFormattedString("Testing PRNG for m~0, s~1 over [%u,%u] matrix", nr, nc);
    double s3 = sqrt(3.0);
    auto x = KMatrix::uniform(rng, nr, nc, -s3, +s3);
    LOG(DEBUG) << getFormattedString("Observed mean: %+.5E", mean(x));
    LOG(DEBUG) << getFormattedString("Observed stdv: %+.5E", stdv(x));

    LOG(DEBUG) << "Test matrix transpose, subtract, multiply";
    for (unsigned int iter = 0; iter < 10; iter++) {
        double errTol = 1E-10;
        unsigned int n1 = 5 + (rng->uniform() % 21);
        unsigned int n2 = 5 + (rng->uniform() % 21);;
        unsigned int n3 = 5 + (rng->uniform() % 21);
        auto a = KMatrix::uniform(rng, n1, n2, -10, +20);
        auto b = KMatrix::uniform(rng, n1, n2, -10, +20);
        auto c = KMatrix::uniform(rng, n3, n2, -10, +20);
        auto lhs = trans((a - b)*trans(c));
        auto rhs = c * (trans(a) - trans(b));
        double err = norm(lhs - rhs);
        LOG(DEBUG) << getFormattedString("Norm of diff T((a-b)*T(c)) - c*(T(a)-T(b)) is %.3E ", err);
        assert(err < errTol);
    }

    LOG(DEBUG) << "Test matrix inversion";
    for (unsigned int iter = 0; iter < 10; iter++) {
        const double errTol = 1E-10;
        const unsigned int n = 5;
        auto a = KMatrix::uniform(rng, n, n, -10, 20);
        if (0 == iter % 3) {
            a = a / 1000; // test inversion with smaller elements
        }
        LOG(DEBUG) << getFormattedString("\nRMS(a)=%.4f", norm(a) / n);
        auto b = inv(a);
        double diff = norm(iMat(n) - (a*b));
        LOG(DEBUG) << getFormattedString("Norm of diff I-a*inv(a) is %.3E  ", diff);
        assert(diff < errTol);
        diff = norm(iMat(n) - (b*a));
        LOG(DEBUG) << getFormattedString("Norm of diff I-inv(a)*a is %.3E  ", diff);
        assert(diff < errTol);
    }

    // JAH 20160809 added test for the new vector init
    LOG(DEBUG) << "Test matrix reshaped from vector";
    vector<double> dat = {1,2,3,4,5,6,7,8,9,10,11,12};
    // all in one row
    LOG(DEBUG) << "1 x 12";
    auto mat1 = KMatrix::vecInit(dat,1,12);
    mat1.mPrintf("%2.0f ");
    // 3 x 4
    LOG(DEBUG) << "3 x 4";
    auto mat2 = KMatrix::vecInit(dat,3,4);
    mat2.mPrintf("%2.0f ");
    // 2 x 6
    LOG(DEBUG) << "2 x 6";
    auto mat3 = KMatrix::vecInit(dat,2,6);
    mat3.mPrintf("%2.0f ");

    LOG(DEBUG) << "Row 0 of previous 2x6 matrix";
    auto row1 = KBase::hSlice(mat3, 0);
    row1.mPrintf(" %2.0f ");
    LOG(DEBUG) << "Row 1 of previous 3x4 matrix";
    auto row2 = KBase::hSlice(mat2, 1);
    row2.mPrintf(" %2.0f ");


    demoPCA(rng);

    return;
}

tuple<KMatrix, KMatrix> extendPCA (const KMatrix& xMat,
                                   const KMatrix& wght,
                                   const KMatrix& ftr) {

    const unsigned int nSample = xMat.numR();
    const unsigned int nDim = xMat.numC();
    assert (nSample == wght.numR());
    const unsigned int nFtr = wght.numC();
    assert (nFtr == ftr.numR());
    assert (nDim == ftr.numC());


    auto checkColRMS = [] (const KMatrix& m) {
        const double rmsTol = 1E-10;
        double rmsM = rms(m);
        for (unsigned int j=0; j<m.numC(); j++) {
            auto mj = KBase::vSlice(m, j);
            auto meanJ = fabs(mean(mj));
            assert (meanJ < rmsM * rmsTol);
        }
        return;
    };

    checkColRMS(xMat); // make sure sample means are zero

    auto z = xMat - (wght * ftr); // get the current residual
    checkColRMS(z);

    auto cvr = trans(z)*z; // covariance matrix
    assert (nDim == cvr.numR());
    assert (nDim == cvr.numC());

    const double evTol = 1E-15;
    auto v = KBase::firstEigenvector(cvr, evTol);  //column vector
    v = trans(v); // now a row-vector

    const double dotTol = 1E-10;
    for (unsigned int i=0; i<nFtr; i++) {
        auto vi = KBase::hSlice(ftr,i);
        assert (fabs(dot(v, vi)) < dotTol);
    }

    auto f2 = KMatrix(1+nFtr, nDim);
    for (unsigned int j=0; j<nDim; j++) {
        for (unsigned int i=0; i<nFtr; i++) {
            f2(i,j) = ftr(i,j);
        }
        f2(nFtr, j) = v(0,j);
    }
    auto w2 = xMat * trans(f2);
    auto wf = tuple<KMatrix, KMatrix> (w2,f2);
    return wf;
}


void demoPCA(PRNG* rng) {
    using KBase::mean;
    using KBase::vSlice;
    using KBase::KMatrix;
    using KBase::iMat;
    using KBase::joinH;
    using KBase::inv;
    using KBase::norm;
    using KBase::firstEigenvector;

    auto posUnitize = [] (const KMatrix & m1) {
        auto m2 = KBase::unitize(m1);
        double m = mean(m2);
        if (m < 0.0) {
            m2 = m2*(-1.0);
        }
        return m2;
    };


    const unsigned int nDim = 8;
    const unsigned int nSample = 50;
    const unsigned int nComp = 4;
    const double evTol = 1E-15;
    const double divFactor = 1.25;

    auto m1 = KMatrix::uniform(rng, nDim, nDim, 0.0, +1.0);
    LOG(DEBUG) << "Random matrix:";
    m1.mPrintf("%.4f  ");

    auto v1 = firstEigenvector(m1, evTol);
    auto s11 = dot(v1, v1);

    LOG(DEBUG) << "First eigenvector:";
    v1.mPrintf("%+.5f ");

    auto v2 = m1 * v1;
    auto s12 = dot(v2, v1);
    LOG(DEBUG) << KBase::getFormattedString("Eigenvalue: %+.4f", s12/s11);

    // Now we start a Principal Component Analysis
    LOG(DEBUG) << "Setup PCA";
    const KMatrix pcv1 = posUnitize(KMatrix::uniform(rng, 1, nDim, -10.0, +10.0));
    LOG(DEBUG) << KBase::getFormattedString("norm pcv1: %.4f", dot(pcv1, pcv1));

    const KMatrix tv2 = KMatrix::uniform(rng, 1, nDim, -10.0, +10.0);
    double a21 = dot(tv2, pcv1) ;
    const KMatrix pcv2 = posUnitize(tv2 - a21*pcv1);
    LOG(DEBUG) << KBase::getFormattedString("norm pcv2: %.4f", dot(pcv2, pcv2));
    LOG(DEBUG) << KBase::getFormattedString("dot21: %+.2e", dot(pcv2, pcv1));

    const KMatrix tv3 = KMatrix::uniform(rng, 1, nDim, -10.0, +10.0);
    double a31 = dot(tv3, pcv1);
    double a32 = dot(tv3, pcv2);
    const KMatrix pcv3 = posUnitize(tv3 - (a31*pcv1 + a32*pcv2));
    LOG(DEBUG) << KBase::getFormattedString("norm pcv3: %.4f", dot(pcv3, pcv3));
    LOG(DEBUG) << KBase::getFormattedString("dot31: %+.2e", dot(pcv3, pcv1));
    LOG(DEBUG) << KBase::getFormattedString("dot32: %+.2e", dot(pcv3, pcv2));

    const KMatrix tv4 = KMatrix::uniform(rng, 1, nDim, -10.0, +10.0);
    double a41 = dot(tv4, pcv1);
    double a42 = dot(tv4, pcv2);
    double a43 = dot(tv4, pcv3);
    const KMatrix pcv4 = posUnitize(tv4 - (a41*pcv1 + a42*pcv2 + a43*pcv3));
    LOG(DEBUG) << KBase::getFormattedString("norm pcv4: %.4f", dot(pcv4, pcv4));
    LOG(DEBUG) << KBase::getFormattedString("dot41: %+.2e", dot(pcv4, pcv1));
    LOG(DEBUG) << KBase::getFormattedString("dot42: %+.2e", dot(pcv4, pcv2));
    LOG(DEBUG) << KBase::getFormattedString("dot43: %+.2e", dot(pcv4, pcv3));

    LOG(DEBUG) << "Actual " << nDim << "-dim principal components: ";
    pcv1.mPrintf("%+7.4f  ");
    pcv2.mPrintf("%+7.4f  ");
    pcv3.mPrintf("%+7.4f  ");
    pcv4.mPrintf("%+7.4f  ");


    auto xMat = KMatrix(nSample, nDim);
    LOG(DEBUG) << "Actual weights of" << nSample << "samples:";
    for (unsigned int i=0; i<nSample; i++) {
        const double rMin = -45.0;
        const double rMax = +90.0;
        double div = 1.0;
        auto w1 = rng->uniform(rMin, rMax) / div;
        div = div * divFactor;
        auto w2 = rng->uniform(rMin, rMax) / div;
        div = div * divFactor;
        auto w3 = rng->uniform(rMin, rMax) / div;
        div = div * divFactor;
        auto w4 = rng->uniform(rMin, rMax) / div;
        div = div * divFactor;
        auto ns = KMatrix::uniform(rng, 1, nDim, rMin, rMax) / div;
        auto xi = (w1*pcv1) + (w2*pcv2) + (w3*pcv3) + (w4*pcv4) + ns;
        for (unsigned int j=0; j<nDim; j++) {
            xMat(i,j) = xi(0, j);
        }
        LOG(DEBUG) << KBase::getFormattedString("%+8.4f  %+8.4f  %+8.4f  %+8.4f", w1, w2, w3, w4);
    }
    LOG(DEBUG) << "Data (one sample per row): ";
    xMat.mPrintf("%+8.3f  ");

    auto sMean = KMatrix(1, nDim);
    // subtract sample means off each column
    for (unsigned int j=0; j<nDim; j++) {
        const KMatrix xj = KBase::vSlice(xMat, j);
        const double smj = KBase::mean(xj);
        sMean(0,j) = smj;
        for (unsigned int i=0; i<nSample; i++) {
            xMat(i,j) = xMat(i,j) - smj;
        }
    }

    LOG(DEBUG) <<"Sample means:";
    sMean.mPrintf("%+8.3f  ");

    assert (nComp <= nDim);
    const KMatrix yMat = xMat;
    auto cvrMat = (trans(yMat) * yMat) / nSample;
    auto f1 = trans(firstEigenvector(cvrMat, evTol));
    auto w1 = yMat * trans(f1); 

    auto showErr = [yMat] (const KMatrix & w, const KMatrix & f) {
        LOG(DEBUG) << "Estimated principal components:";
        f.mPrintf("%+7.4f  ");
        LOG(DEBUG) << "Estimated weights:";
        w.mPrintf("%+8.3f  ");
        auto zMat = w * f;
        LOG(DEBUG) << KBase::getFormattedString("RMS of estimated zMat: %.3e", rms(zMat));
        auto eMat = yMat - zMat;
        LOG(DEBUG) << KBase::getFormattedString("RMS of error eMat: %.3e", rms(eMat));
        double yzCorr = KBase::lCorr(yMat - mean(yMat), zMat - mean(zMat));
        LOG(DEBUG) << KBase::getFormattedString("Y-Z Affine correlation %+.4f", yzCorr);
        return;
    };
    
    showErr(w1, f1);


    for (unsigned int n=1; n<nComp; n++) {
        LOG(DEBUG) << "Extracting component" << n;

        LOG(DEBUG) << "Try to extend";
        auto wf = extendPCA(yMat, w1, f1);
        KMatrix w2 = get<0>(wf);
        KMatrix f2 = get<1>(wf);

        w1 = w2;
        f1 = f2;
        showErr(w1, f1);
    }

    return;
}


void demoABG00(PRNG* rng) {
    // min E=|Ax-b|^2 where li <= xi <= ui
    // F(x) = [dE/dxi] = 2 * trans(A) * (Ax-b)
    unsigned int dx = 6;
    unsigned int db = 4;
    auto A = KMatrix::uniform(rng, db, dx, -10, +10);
    auto x = KMatrix::uniform(rng, dx, 1, -10, +10);
    auto b = A*x;
    KMatrix l = x - 1;
    KMatrix u = x + 1;

    LOG(DEBUG) << "Feasible point, x, with 0=|Ax-b|, l<=x<=u:";
    x.mPrintf(" %8.2f ");
    LOG(DEBUG) << "Lower bound = x-1, upper bound = x+1";
    LOG(DEBUG) << "Dim of x:" << dx;
    LOG(DEBUG) << "Dim of b:" << db;

    // box constrained VI, aka MCP
    auto P = [l, u](const KMatrix & x1) {
        KMatrix x2 = x1;
        for (unsigned int i = 0; i < x1.numR(); i++) {
            double xi = x1(i, 0);
            double li = l(i, 0);
            double ui = u(i, 0);
            xi = (xi < li) ? li : xi;
            xi = (ui < xi) ? ui : xi;
            x2(i, 0) = xi;
        }
        return x2;
    };
    auto F = [A, b](const KMatrix & x) {
        return 2 * trans(A)*(A*x - b);
    };

    bool extra = false;
    auto x0 = KMatrix::uniform(rng, dx, 1, -100, +100);
    auto xie = viABG(x0, F, P, 0.5, 1E-8, 1000, extra);
    KMatrix xf = get<0>(xie);
    unsigned int iter = get<1>(xie);
    KMatrix e = get<2>(xie);

    LOG(DEBUG) << "Initial search point, x0:";
    x0.mPrintf(" %8.2f ");

    LOG(DEBUG) << "After" << iter << "iterations, found xf:";
    xf.mPrintf(" %8.2f ");
    LOG(DEBUG) << "xf-x:";
    (xf - x).mPrintf(" %8.2f ");
    LOG(DEBUG) << getFormattedString("Norm of AX-b: %.3E", norm(A*xf - b));
    LOG(DEBUG) << "Final projection error:";
    e.mPrintf(" %+.2E ");

    return;
}

// weighted Euclidean distance so that eNorm(a,x)>1 is outside the ellipsoid,
// eNorm==1 is exactly on the surface, and eNorm<1 is inside.
double eNorm(const KMatrix & a, const KMatrix &  x) {
    double sum = 0.0;
    for (unsigned int i = 0; i < a.numR(); i++) {
        double r = x(i, 0) / a(i, 0);
        sum = sum + (r*r);
    }
    return sqrt(sum);
};

// Rescale so that eNorm is 1.0
// Geometrically, draw line from x to the origin, and return
// the point where it intersects the ellipsoid surface.
// Hence eUnitize(a, x) == x if and only if x is on the surface,
// so |x-eUnitize(a, x)| is a useful error measure between x and its projection.
KMatrix eUnitize(const KMatrix & a, const KMatrix & x) {
    return (x / eNorm(a, x));
}

// Project w onto the ellipsoid, i.e. find the point in/on the ellipsoid closest to w.
//
// My algorithm uses the Lagrangian formulation to reduce it to a one-dimensional
// search, with known bounds. Because the initial bounds are quite generous, it
// usually takes 20-30 iterations, regardless of dimensionality, because
// I've reduced every problem to 1-dim within known bounds.
//
KMatrix projEllipse(const KMatrix & a, const KMatrix & w) {
    unsigned int n = a.numR();
    assert(1 == a.numC());
    assert(sameShape(a, w));

    if (eNorm(a, w) <= 1) {
        return w;    // on or inside?
    }

    double minF = 0.0;
    double maxF = 0.0;
    for (unsigned int i = 0; i < n; i++) {
        double ai = a(i, 0);
        assert(0 < ai);
        double wi = w(i, 0);
        maxF = maxF + (ai*wi)*(ai*wi);
    }
    maxF = sqrt(maxF);

    double eps0 = 1e-8; // required relative precision
    double eps = eps0 * sqrt(dot(a, a) / n);// required absolute precision
    // For example, Earth is about 6.378e6 to 6.357e6 meters along its major and minor axis.
    // A eps0=1e-8 thus means eps=6.4e-2 meters absolute precision: 6.4 cm


    // f is the Lagrange multiplier, and we seek an f-value
    // such that 1 = eNorm(a, adjust(w,f))
    auto adjust = [n, a](const KMatrix & w, double f) {
        KMatrix x = w;
        for (unsigned int i = 0; i < n; i++) {
            double wi = w(i, 0);
            double ai = a(i, 0);
            double xi = wi / (1 + (f / (ai*ai)));
            x(i, 0) = xi;
        }
        return x;
    };

    double normMax = eNorm(a, adjust(w, minF));
    double normMin = eNorm(a, adjust(w, maxF));
    assert(1.0 < normMax); // true because 1 < eNorm(a, w), and adjust(w,0)=w
    assert(1.0 > normMin); // true because maxF is constructed to make it so

    // By simple algebra, eNorm(a, adjust(w,f)) is the sum_i (wi/(ai + f/ai))
    //
    // If you define z(w,f)= sum_i (wi/ (f/ai)), then two things hold:
    // (1) z(w,f) > eNorm(a, adjust(w,f)), as ai>0
    // (2) it is trivial to solve z(w,f)=1, yielding maxF.
    // hence, z(w,maxF) = 1 > eNorm(a, adjust(w,maxF)) = normMin.
    //
    // So adjust(w,minF) is strictly outside the ellipsoid and
    // adjust(w,maxF) is strictly inside. We need only do a basic
    // binary search to solve for eNorm(a, adjust(w,f)) = 1.

    auto rl = ReportingLevel::Silent;
    auto x = w;
    double f0 = minF;
    double f1 = maxF;
    double en = eNorm(a, x);
    double err = norm(x - eUnitize(a, x)); // get this below eps
    //double e = fabs(eNorm(a, x) - 1);
    unsigned int iter = 0;

    while (err > eps) {
        if (ReportingLevel::Silent < rl) {
            if ((ReportingLevel::Low < rl) || (0 == iter)) {
                LOG(DEBUG) << "iter" << iter;
                LOG(DEBUG) << getFormattedString("%.4f  %.4e/%.4e  [%.8e, %.8e]", en, err, eps, f0, f1);
                show("x", trans(x), "%+.6f  ");
            }
        }

        // we know Y is declining in f, so it would be useful to check in debugging
        // y0 = eNorm(a, adjust(w, f0));
        //double y1 = eNorm(a, adjust(w, f1));
        double fEst = (f0 + f1) / 2;
        x = adjust(w, fEst);
        double yEst = eNorm(a, x);
        err = norm(x - eUnitize(a, x));
        en = eNorm(a, x);
        if (yEst < 1.0) {
            f1 = fEst;
        }
        if (1.0 < yEst) {
            f0 = fEst;
        }
        iter++;
    }
    if (ReportingLevel::Silent < rl) {
        LOG(DEBUG) << "iter" << iter;
        LOG(DEBUG) << getFormattedString("%.4e/%.4e  [%.8e, %.8e]", err, eps, f0, f1);
        show("x", trans(x), "%+.6f  ");
    }
    return x;
}

// define and solve an LVI with ellipsoidal constraint set.
//
// define E = {x | n(x) = 1 = sum_i (xi/ei)^2 } as the ellipsoid we must be on or inside.
// Tx = tangent plane to E at x
// = { z | beta * (z-x) = 0 } where beta_i = x_i / (e_i ^ 2) is the outward pointing surface normal.
// Hence, the LVI F(x) * (y-x) >= 0 , where x,y in E has
// F(x) = Mx+q as the inward-pointing surface normal, i.e. -beta.
// For arbitrary PSD M (e.g. A^T * A), define q = -(Mx + beta)
//
// Generally, ABG performs much better on this problem than does BSHe96.
void demoEllipseLVI(PRNG* rng, unsigned int n) {
    LOG(DEBUG) << "Construct and solve LVI with ellipsoidal K in" << n << "dimensions";
    KMatrix a = KMatrix::uniform(rng, n, 1, +5.0, +15.0);
    KMatrix A = KMatrix::uniform(rng, n, n, -1.0, +2.0);
    KMatrix M = trans(A) * A;
    KMatrix xStar = eUnitize(a, KMatrix::uniform(rng, n, 1, -1.0, +1.0)); // a random point on the ellipse

    auto bfn = [xStar, a](unsigned int i, unsigned int j) {
        double ai = a(i, 0);
        return xStar(i, j) / (ai*ai);
    };
    KMatrix  beta = KMatrix::map(bfn, xStar.numR(), xStar.numC());

    // for large values of 'n', the entries of M (and hence q) can get quite large.
    double c1 = norm(M*xStar);
    double nxs = norm(xStar);
    double c2 = rng->uniform(0.8, 1.2)*nxs;
    M = (c2*M) / c1;
    KMatrix q = -1.0 * (M*xStar + beta);

    LOG(DEBUG) << "Constructed solution";
    show("Ellipsoid:", trans(a), "%+8.4f  ");
    show("xStar:", trans(xStar), "%+8.4f  ");
    show("M*xStar+q:", trans(M*xStar + q), "%+8.4f  ");

    LOG(DEBUG) << "Constructed problem for that solution";
    show("M:", M, "%+8.4f  ");
    show("q:", trans(q), "%+8.4f  ");

    KMatrix x0 = 1.5 * eUnitize(a, KMatrix::uniform(rng, n, 1, -1.0, +1.0)); // a random point outside the ellipse
    show("initial x0:", trans(x0), "%+8.4f  ");

    auto projE = [a](KMatrix x0) {
        return projEllipse(a, x0);
    };
    unsigned int iterLim = 250 * 1000;
    double eps = 1e-6; // relative error in result inside the solution algorithm
    double tol = 1e3*eps; // relative error tolerance comparing result to known answer: 1e-2 is 1% error
    auto sfe = [](const KMatrix & x, const KMatrix & y) {
        return (2 * norm(x - y)) / (norm(x) + norm(y));
    };
    auto F = [M, q](const KMatrix & x) {
        return (M*x + q);
    };

    auto processRslt = [sfe, M, q, xStar, tol](tuple<KMatrix, unsigned int, KMatrix> r) {
        KMatrix u = get<0>(r);
        unsigned int iter = get<1>(r);
        KMatrix res = get<2>(r);
        LOG(DEBUG) << "After" << iter << "iterations";
        LOG(DEBUG) << "  solution u:";
        trans(u).mPrintf(" %+.3f ");
        LOG(DEBUG) << "  residual r:";
        trans(res).mPrintf(" %+.3f ");
        KMatrix v = M*u + q;
        double e1 = sfe(u, xStar);
        double e2 = sfe(v, M*xStar + q);
        LOG(DEBUG) << getFormattedString("SFE of u is %.3E,  SFE of v is %.3E", e1, e2);
        assert(e1 < tol); // inaccurate U
        assert(e2 < tol); // inaccurate V
        return;
    };

    LOG(DEBUG) << "Solve via ABG";
    auto r2 = viABG(x0, F, projE, 0.5, eps, iterLim, false);
    processRslt(r2);

    LOG(DEBUG) << "Solve via AEG";
    auto r2e = viABG(x0, F, projE, 0.5, eps, iterLim, true);
    processRslt(r2e);

    LOG(DEBUG) << "Solve via BSHe96";
    auto r1 = viBSHe96(M, q, projE, x0, eps, iterLim);
    processRslt(r1);

    return;
}

// Setup a problem that is exponentially difficult for Lemke's algorithm.
tuple<KMatrix, KMatrix, KMatrix, KMatrix> antiLemke(unsigned int n) {
    // M has a diagonal of 1's, with upper triangle of 2's and lower triangle of 0's
    // q is all -1.
    // Constrained to [0,+infty)^n
    // Solution: v=Mu+q, where
    // u all 0's except the last element, which is 1
    // v all 1's except the last element, which is 0

    auto M = KMatrix(n, n);
    auto q = KMatrix(n, 1);
    auto u = KMatrix(n, 1);
    auto v = KMatrix(n, 1);
    for (unsigned int i = 0; i < n; i++) {
        q(i, 0) = -1;
        if (i < n - 1) {
            u(i, 0) = 0;
            v(i, 0) = 1;
        }
        else {
            u(i, 0) = 1;
            v(i, 0) = 0;
        }
        for (unsigned int j = 0; j < n; j++) {
            if (i == j) {
                M(i, j) = 1;
            }
            if (i < j) {
                M(i, j) = 2;
            }
        }
    }
    return tuple<KMatrix, KMatrix, KMatrix, KMatrix>(M, q, u, v);
}

// Generally, BSHe96 performs much better on this problem than does ABG.
void demoAntiLemke(PRNG* rng, unsigned int n) {
    LOG(DEBUG) << "Construct and solve AntiLemke LVI in" << n << "dimensions";
    auto al = antiLemke(n);
    KMatrix M = get<0>(al);
    KMatrix q = get<1>(al);
    KMatrix u = get<2>(al);
    KMatrix v = get<3>(al);
    auto sfe = [](const KMatrix & x, const KMatrix & y) {
        return (2 * norm(x - y)) / (norm(x) + norm(y));
    };
    auto F = [M, q](const KMatrix & x) {
        return (M*x + q);
    };
    auto xInit = KMatrix::uniform(rng, n, 1, -20.0, 20.0);
    double eps = 1E-6;
    unsigned int iterLim = 10000;
    LOG(DEBUG) << "Initial point:";
    trans(xInit).mPrintf(" %+7.3f ");

    auto processRslt = [sfe, M, q, u, eps](tuple<KMatrix, unsigned int, KMatrix> r) {
        KMatrix u = get<0>(r);
        unsigned int iter = get<1>(r);
        KMatrix res = get<2>(r);
        LOG(DEBUG) << "After" << iter << "iterations";
        LOG(DEBUG) << "  solution u:";
        trans(u).mPrintf(" %+.3f ");
        LOG(DEBUG) << "  residual r:";
        trans(res).mPrintf(" %+.3f ");
        KMatrix v = M*u + q;
        double tol = 100 * eps;
        double e1 = sfe(u, u);
        double e2 = sfe(v, M*u + q);
        LOG(DEBUG) << getFormattedString("SFE of u is %.3E,  SFE of v is %.3E", e1, e2);
        assert(e1 < tol); // inaccurate U
        assert(e2 < tol); // inaccurate V
        return;
    };

    LOG(DEBUG) << "Solve via BSHe96";
    auto r1 = viBSHe96(M, q, KBase::projPos, xInit, eps, iterLim);
    processRslt(r1);

    LOG(DEBUG) << "Solve via ABG";
    auto r2 = viABG(xInit, F, KBase::projPos, 0.5, eps, iterLim, false);
    processRslt(r2);

    LOG(DEBUG) << "Solve via AEG";
    auto r2b = viABG(xInit, F, KBase::projPos, 0.5, eps, iterLim, true);
    processRslt(r2b);

    return;
}


void demoEllipse(PRNG* rng) {
    unsigned int numD = 9;
    auto a = KMatrix::uniform(rng, numD, 1, 1.0, 10.0);
    LOG(DEBUG) << "Demo projection onto ellipsoid in" << numD << "dimensions";
    LOG(DEBUG) << "Ellipse parameters:";
    trans(a).mPrintf(" %+8.4f ");
    auto w = KMatrix::uniform(rng, numD, 1, -20.0, 20.0);
    LOG(DEBUG) << "Point to be projected:";
    trans(w).mPrintf(" %+8.4f ");
    auto pe = UDemo::projEllipse(a, w);

    LOG(DEBUG) << "Projected point:";
    trans(pe).mPrintf(" %+8.4f ");

    LOG(DEBUG) << getFormattedString("eNorm(projected) = %.f", UDemo::eNorm(a, pe));

    UDemo::demoEllipseLVI(rng, 10);
    return;
}

// -------------------------------------------------

void demoGA(PRNG* rng) {
    using KBase::GAOpt;
    const unsigned int nb = 32;
    // many alternatives to search, with a many-peaked function
    // whose highest peaks do not vary much, percentage-wise.
    // Note that the value of any particular bit-vector depends heavily
    // on interactions between the bits.
    // This is also an example of how to use λ-fn
    // to redefine the eval fn w/o touching the class

    LOG(DEBUG) << "Target length = " << nb ;
    LOG(DEBUG) << "Set target: ";
    const KBase::VBool trgt = TargetedBV::randomBV(rng, nb);
    TargetedBV::setTarget(trgt);
    TargetedBV::showBits(TargetedBV::getTarget());

    unsigned int tblSize = 20;
    double minD = 0.25;
    auto wghts = vector<double>();
    auto tbl = vector<VBool>();
    auto gs = vector<TargetedBV>();
    for (unsigned int i = 0; i < tblSize; i++) {
        auto gi = TargetedBV();
        gi.randomize(rng);
        gs.push_back(gi);
        wghts.push_back(10.0 * i);
        tbl.push_back(gi.bits);
    }
    gs[tblSize - 1] = TargetedBV(trgt);
    tbl[tblSize - 1] = trgt;

    LOG(DEBUG) << "Table eval with minD = " << minD;
    for (unsigned int i = 0; i < tblSize; i++) {
        LOG(DEBUG) << getFormattedString("%2u  %8.3f  ", i, gs[i].tblEval(minD, wghts, tbl));
        TargetedBV::showBits(gs[i].bits);
    }

    auto compFn = [gs, minD, wghts, tbl](unsigned int i, unsigned int j) {
        TargetedBV gsi = gs[i];
        double vi = gsi.tblEval(minD, wghts, tbl);
        TargetedBV gsj = gs[j];
        double vj = gsj.tblEval(minD, wghts, tbl);
        LOG(DEBUG) << getFormattedString("v[%2i]/v[%2i]:  %.3f", i, j, (vi / vj));
        return;
    };
    compFn(tblSize - 1, 0);
    compFn(tblSize - 1, tblSize / 2);
    compFn(tblSize - 1, tblSize - 2);

    auto mgFn = [](PRNG* rng) {
        auto tbv = new TargetedBV();
        tbv->randomize(rng);
        return tbv;
    };

    // good example of how λ-binding lets one define and use
    // parameters which were not anticipated in the original class.
    auto evFn = [minD, wghts, tbl](const TargetedBV* tbv) {
        return tbv->tblEval(minD, wghts, tbl);
        //return tbv->evaluate();
    };

    auto shFn = [](const TargetedBV* tbv) {
        tbv->show();
        return;
    };

    auto eqFn = [](const TargetedBV* g1, const TargetedBV* g2) {
        bool e = g1->equiv(g2);
        return e;
    };

    auto muFn = [](const TargetedBV* g1, PRNG* rng) {
        auto t2 = g1->mutate(rng);
        return t2;
    };

    auto crFn = [](const TargetedBV* t1, const TargetedBV* t2, PRNG* rng) {
        auto pr = t1->cross(t2, rng);
        return pr; // memory leak?
    };


    // NOTE WELL: with multi-threading, the order of cross-over and evaluation
    // varies from run to run, so the order of PRNG generation varies,
    // as do the results.
    unsigned int pS = 50; // size of the gene pool
    double cf = 2.2; // 2.2 == everything crosses over twice, plus random 20%
    double mf = 1.5; // 1.5 == everything mutates once, plus random 50%

    LOG(DEBUG) << "Population size:" << pS;

    auto gOpt = new GAOpt<TargetedBV>(pS);
    gOpt->cross = crFn;
    gOpt->mutate = muFn;
    gOpt->eval = evFn;
    gOpt->showGene = shFn;
    gOpt->makeGene = mgFn;
    gOpt->equiv = eqFn;

    auto ip = vector<TargetedBV*>();
    ip.push_back(new TargetedBV(TargetedBV::getTarget()));
    //   gOpt->init(ip);

    gOpt->fill(rng);
    LOG(DEBUG) << "Random basic population:";
    gOpt->show();

    auto srl = KBase::ReportingLevel::Low;
    unsigned int iter = 0;
    unsigned int sIter = 0;
    LOG(DEBUG) << getFormattedString("Crossover fraction: %.3f", cf);
    LOG(DEBUG) << getFormattedString("Mutation fraction: %.3f", mf);
    gOpt->run(rng, cf, mf, 1000, 0.2, 50, srl, iter, sIter);

    LOG(DEBUG) << "Completed run after" << iter << "iterations," << sIter << "stable";
    auto vgBest = gOpt->getNth(0);
    LOG(DEBUG) << getFormattedString("Best value found: %.3f", get<0>(vgBest));
    LOG(DEBUG) << "Best gene found:";
    get<1>(vgBest)->show();
    LOG(DEBUG) << "Final gpool: ";
    gOpt->show();

    delete gOpt;
    return;
}


void demoGHC(PRNG* rng) {
    unsigned int numBits = 20;
    const VBool bv0 = rng->bits(numBits);
    auto wv0 = KMatrix::uniform(rng, numBits, 1, 1.0, 10.0);
    wv0 = 100.0 *(wv0 / sum(wv0));

    auto efn = [bv0, wv0](VBool bv) { // obviously, function<double(VBool)>
        double s = 0;
        for (unsigned int i = 0; i < bv0.size(); i++) {
            if (bv[i] == bv0[i]) {
                s = s + wv0(i, 0);
            }
            else {
                s = s - wv0(i, 0);
            }
        }
        return s;
    };

    auto sfn = [](VBool bv) {
        string bvBits;
        for (auto b : bv) {
            if (b) {
                bvBits += "+";
            }
            else {
                bvBits += "o";
            }
        }
        LOG(DEBUG) << bvBits;
        return;
    };

    function< vector<VBool>(VBool)> nfn = [](VBool bv0) {
        auto nb = ((unsigned int)(bv0.size()));
        auto bvs = vector <VBool>();
        for (unsigned int i = 0; i < nb; i++) {
            auto bv = VBool(bv0);
            bv[i] = !bv[i];
            bvs.push_back(bv);
        }
        return bvs;
    };

    el::Loggers::removeFlag(el::LoggingFlag::AutoSpacing);
    LOG(DEBUG) << "Generic hill-climbing search over " << numBits << "-bit strings";
    el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);
    LOG(DEBUG) << "Target string: ";
    sfn(bv0);
    LOG(DEBUG) << getFormattedString("Target value : %+.3f", efn(bv0));

    LOG(DEBUG) << "Starting general hill-climbing search";
    VBool p0 = rng->bits(numBits);
    LOG(DEBUG) << "Initial string: ";
    sfn(p0);
    LOG(DEBUG) << getFormattedString("Initial value: %+.3f", efn(p0));

    auto ghc = GHCSearch<VBool>();
    ghc.eval = efn;
    ghc.nghbrs = nfn;
    ghc.show = sfn;

    ghc.run(p0, KBase::ReportingLevel::Medium, 100, 3, 0.001);

    return;
}

void demoVHC00(uint64_t sd) {
    auto rng = new PRNG(sd);
    unsigned int n = (rng->uniform() % 5); // 0 to 4, median 2
    n = 2 + (n*(n+1))/2; // 2 to 12, median 5
    const auto trgt = KMatrix::uniform(rng, n, 1, -100, +100);
    auto tm = KMatrix(n, n);
    for (unsigned int i=0; i<n; i++) {
        tm(i,i)=rng->uniform(1.0, 5.0);
    }
    LOG(DEBUG) << "Dimension:"<<n;
    LOG(DEBUG) << "Target point:";
    trans(trgt).mPrintf(" %+.4f ");
    LOG(DEBUG) << "Transformation matrix:";
    tm.mPrintf(" %+.4f ");

    auto vhc = new VHCSearch();
    vhc->eval = [trgt, tm](const KMatrix & m) {
        auto v1 = tm * (m-trgt);
        double d2 = dot(v1,v1);
        //double d = norm(m - trgt);
        return 1.0 - sqrt(d2);
    };
    if (1 == n) {
        vhc->nghbrs = VHCSearch::vn1;
    }
    else {
        vhc->nghbrs = VHCSearch::vn2;
    }
    auto p0 = KMatrix::uniform(rng, n, 1, -100, +100);
    LOG(DEBUG) << "Initial point:";
    trans(p0).mPrintf(" %+.4f ");
    auto rslt = vhc->run(p0,
                         500000, 10, 1E-10,
                         1.0, 0.618, 1.25, 1e-8,
                         ReportingLevel::Low);
    double vBest = get<0>(rslt);
    KMatrix pBest = get<1>(rslt);
    unsigned int in = get<2>(rslt);
    unsigned int sn = get<3>(rslt);
    delete vhc;
    vhc = nullptr;
    LOG(DEBUG) << "Iter:" << in << "Stable:" << sn;
    LOG(DEBUG) << getFormattedString("Best value: %+.4f", vBest);
    LOG(DEBUG) << "Best point:";
    trans(pBest).mPrintf(" %+.4f ");

    // JAH 20161103 changed back to llu
    LOG(DEBUG) << getFormattedString("Used PRNG seed:  %020llu", sd);
    LOG(DEBUG) << "Target point was originally:";
    trans(trgt).mPrintf(" %+.4f ");

    return;
}

void demoVHC01(uint64_t sd) {
    auto rng = new PRNG(sd);
    using KBase::KMatrix;
    using KBase::VHCSearch;
    LOG(DEBUG) << "Nash bargaining problem, with 1D positions and 2D bargains";
    LOG(DEBUG) << "NBS seems always to be that t2i == t2j (i.e. di+dj=1)";
    LOG(DEBUG) << "But di is not quite pj (and dj is not quite pi)";
    LOG(DEBUG) << "And di-dj == pj-pi only in mutually risk-neutral cases.";

    double minR = -0.95;
    double maxR = +0.95;
    double ti = rng->uniform(0.05, 0.45);  // theta-i, the position of i
    double ri = rng->uniform(minR, maxR);  // risk-attidue of i
    double pi = rng->uniform(0.55, 0.95);  // prob ( i>j )

    double tj = rng->uniform(0.55, 0.95);  // theta-j, the position of j
    double rj = rng->uniform(minR, maxR);  // risk-attitude of j
    double pj = 1 - pi;

    LOG(DEBUG) << getFormattedString("Ti: %.5f   Tj: %.5f", ti, tj);
    LOG(DEBUG) << getFormattedString("Ri: %+.5f  Rj: %+.5f", ri, rj);
    LOG(DEBUG) << getFormattedString("Pi: %.5f   Pj: %.5f", pi, pj);

    double uii = bsu(fabs(ti - ti), ri) + bsu(fabs(ti - ti), ri);
    double uij = bsu(fabs(ti - tj), ri) + bsu(fabs(ti - tj), ri);
    double uci = pi*uii + pj*uij;  // exp. utility of conflict's outcome to i
    LOG(DEBUG) << getFormattedString("uii:  %.5f", uii);
    LOG(DEBUG) << getFormattedString("uij:  %.5f", uij);
    LOG(DEBUG) << getFormattedString("uci:  %.5f", uci);

    double uji = bsu(fabs(tj - ti), rj) + bsu(fabs(tj - ti), rj);
    double ujj = bsu(fabs(tj - tj), rj) + bsu(fabs(tj - tj), rj);
    double ucj = pi*uji + pj*ujj;  // exp. utility of conflict's outcome to j
    LOG(DEBUG) << getFormattedString("uji:  %.5f", uji);
    LOG(DEBUG) << getFormattedString("ujj:  %.5f", ujj);
    LOG(DEBUG) << getFormattedString("ucj:  %.5f", ucj);

    auto vc = new VHCSearch();
    vc->nghbrs = VHCSearch::vn2;

    auto eFn = [ti, uci, ri, tj, ucj, rj](const KMatrix & dij) {
        assert(2 == dij.numR());
        assert(1 == dij.numC());
        double di = dij(0, 0); // delta-i, the fractional shift of i toward j
        double dj = dij(1, 0); // delta-j, the fractional shift of j toward i

        double t2i = ti + di*(tj - ti); // resulting position of i
        double t2j = tj + dj*(ti - tj); // resulting position of j

        double ubi = bsu(fabs(ti - t2i), ri) + bsu(fabs(ti - t2j), ri);
        // utility of the bargain to i
        double ubj = bsu(fabs(tj - t2i), rj) + bsu(fabs(tj - t2j), rj);
        // utility of the bargain to j

        return 100.0*nProd(ubi - uci, ubj - ucj);
    };

    vc->eval = eFn;

    auto d0ij = KMatrix::uniform(rng, 2, 1, -0.05, +0.05);
    LOG(DEBUG) << "Point 1:";
    trans(d0ij).mPrintf(" %+.4f ");

    LOG(DEBUG) << "NP 1: " << eFn(d0ij);

    auto rslt = vc->run(d0ij,
                        1000, 20, 1e-16,
                        0.01, 0.618, 1.25, 1e-8, // NP already multiplied by 1000
                        ReportingLevel::Low);
    double vBest = get<0>(rslt);
    KMatrix pBest = get<1>(rslt);
    unsigned int in = get<2>(rslt);
    unsigned int sn = get<3>(rslt);

    LOG(DEBUG) << "Point 2:";
    trans(pBest).mPrintf(" %+.5f ");

    LOG(DEBUG) << "NP 2:" << vBest;
    LOG(DEBUG) << "Iter: " << in << " Stable:" << sn;

    double d2i = pBest(0, 0);
    double d2j = pBest(1, 0);

    double t2i = ti + d2i*(tj - ti);
    double t2j = tj + d2j*(ti - tj);

    // The pattern seems to be that if ri,rj both non-negative,
    // then NP2 > 0 and they do meet in the middle.
    // If ri,rj are both negative, then NP2<0 and
    // there is no bargain they'd both prefer to conflict.
    // If ri,rj are mixed, then either outcome may occur.
    //
    if (vBest < 0.0) {
        LOG(DEBUG) << "Found no bargain which both prefer to conflict";
    }
    else {
        LOG(DEBUG) << "Found a bargain which both prefer to conflict" 
            << getFormattedString("T2i: %.5f   T2j: %.5f   diff: %.5f", t2i, t2j, fabs(t2i - t2j));
        LOG(DEBUG) << "Not quite zero:";
        LOG(DEBUG) << getFormattedString("Pi - D2j:   %+.5f", pi - d2j);
        LOG(DEBUG) << getFormattedString("Pj - D2i:   %+.5f", pj - d2i);

        LOG(DEBUG) << "Not quite equal:";
        LOG(DEBUG) << getFormattedString("D2i - D2j: %+.5f", d2i - d2j);
        LOG(DEBUG) << getFormattedString(" Pj -  Pi: %+.5f", pj - pi);
    }
    return;
}

void demoVHC02(uint64_t sd) {
    auto rng = new PRNG(sd);
    using KBase::KMatrix;
    using KBase::dot;
    using KBase::maxAbs;

    LOG(DEBUG) << "Nash bargaining problem, with 4D points and 8D bargains";
    LOG(DEBUG) << "When both are risk-averse, the NBS seems always to be that t2i == tb == t2j";
    LOG(DEBUG) << "even though tb is not a weighted mix of ti and tj.";
    LOG(DEBUG) << "When one or both are risk-seeking, then there may not";
    LOG(DEBUG) << "be a bargain they both prefer to conflict. When there is one,";
    LOG(DEBUG) << "they might or might not take the same position.";

    auto vc = new KBase::VHCSearch();

    // fixed, symmetric values for testing, so I know who should compromise, and to what.
    double tmi[] = { 0.85, 0.85, 0.85, 0.85 }; // position
    double smi[] = { 0.95, 0.05, 0.50, 0.50 }; // salience

    double tmj[] = { 0.15, 0.15, 0.15, 0.15 }; // position
    double smj[] = { 0.05, 0.95, 0.05, 0.95 }; // salience

    double minR = -0.95;
    double maxR = +0.95;
    // position, salience, risk, prob-of-victory
    auto ti = KMatrix::arrayInit(tmi, 4, 1);
    auto si = KMatrix::arrayInit(smi, 4, 1);
    double ri = rng->uniform(minR, maxR);
    double pi = rng->uniform(0.51, 0.90);

    auto tj = KMatrix::arrayInit(tmj, 4, 1);
    auto sj = KMatrix::arrayInit(smj, 4, 1);
    double rj = rng->uniform(minR, maxR);
    double pj = 1 - pi;

    auto sfn = [](string s, KMatrix m) {
        LOG(DEBUG) << s << ":  ";
        trans(m).mPrintf(" %.4f ");
    };

    sfn("ti:", ti);
    sfn("si:", si);
    LOG(DEBUG) << getFormattedString("pi: %.4f  ri: %+.5f", pi, ri);
    sfn("tj:", tj);
    sfn("sj:", sj);
    LOG(DEBUG) << getFormattedString("pj: %.4f  rj: %+.5f", pj, rj);

    auto tiProp = ti + pj*(tj - ti);
    auto tjProp = tj + pi*(ti - tj);
    sfn("Prob-weighted compromise:     ", joinV(tiProp, tjProp));

    auto tiSqr = ti + pj*pj*(tj - ti);
    auto tjSqr = tj + pi*pi*(ti - tj);
    sfn("Sqr-prob-weighted  compromise:", joinV(tiSqr, tjSqr));

    double uii = bvu(ti - ti, si, ri) + bvu(ti - ti, si, ri);
    double uij = bvu(ti - tj, si, ri) + bvu(ti - tj, si, ri);
    double uci = pi*uii + pj*uij;
    LOG(DEBUG) << getFormattedString("uii:  %.5f", uii);
    LOG(DEBUG) << getFormattedString("uij:  %.5f", uij);
    LOG(DEBUG) << getFormattedString("uci:  %.5f", uci);

    double uji = bvu(tj - ti, sj, rj) + bvu(tj - ti, sj, rj);
    double ujj = bvu(tj - tj, sj, rj) + bvu(tj - tj, sj, rj);
    double ucj = pi*uji + pj*ujj;
    LOG(DEBUG) << getFormattedString("uji:  %.5f", uji);
    LOG(DEBUG) << getFormattedString("ujj:  %.5f", ujj);
    LOG(DEBUG) << getFormattedString("ucj:  %.5f", ucj);

    auto efn2 = [ti, uci, si, ri, tj, ucj, sj, rj](KMatrix t2i, KMatrix t2j) {
        double ubi = bvu(ti - t2i, si, ri) + bvu(ti - t2j, si, ri);
        double ubj = bvu(tj - t2i, sj, rj) + bvu(tj - t2j, sj, rj);
        double np = nProd(ubi - uci, ubj - ucj);
        return 100.0 * np;
    };

    auto eFn = [efn2](KMatrix v) {
        unsigned int dim = v.numR();
        assert(8 == dim);
        double m2i[] = { v(0, 0), v(1, 0), v(2, 0), v(3, 0) }; // position
        auto t2i = KMatrix::arrayInit(m2i, 4, 1);
        double m2j[] = { v(4, 0), v(5, 0), v(6, 0), v(7, 0) }; // position
        auto t2j = KMatrix::arrayInit(m2j, 4, 1);
        double np = efn2(t2i, t2j);
        return np;
    };

    // A simple closed-form estimator (which ignores risk attitudes, except as reflected in Pi, Pj):
    KMatrix eBS1P1 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik)*(pi);
        double wjk = (sjk)*(pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        eBS1P1(k, 0) = bk;
    }
    KMatrix eBS2P1 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik*sik)*(pi);
        double wjk = (sjk*sjk)*(pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        eBS2P1(k, 0) = bk;
    }
    KMatrix eBS1P2 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik)*(pi*pi);
        double wjk = (sjk)*(pj*pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        eBS1P2(k, 0) = bk;
    }
    KMatrix eBS2P2 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik*sik)*(pi*pi);
        double wjk = (sjk*sjk)*(pj*pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        eBS2P2(k, 0) = bk;
    }

    KMatrix dBS1P1 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik)*(1 - pj);
        double wjk = (sjk)*(pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        dBS1P1(k, 0) = bk;
    }

    KMatrix dBS1P2 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik)*(1 - (pj*pj));
        double wjk = (sjk)*(pj*pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        dBS1P2(k, 0) = bk;
    }

    KMatrix dBS2P1 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik*sik)*(1 - pj);
        double wjk = (sjk*sjk)*(pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        dBS2P1(k, 0) = bk;
    }

    KMatrix dBS2P2 = KMatrix(4, 1);
    for (unsigned int k = 0; k < 4; k++) {
        double tik = ti(k, 0);
        double sik = si(k, 0);
        double tjk = tj(k, 0);
        double sjk = sj(k, 0);
        double wik = (sik*sik)*(1 - (pj*pj));
        double wjk = (sjk*sjk)*(pj*pj);
        double bk = (wik*tik + wjk*tjk) / (wik + wjk);
        dBS2P2(k, 0) = bk;
    }

    // NOTE: eSnPm are symmetric, 4-dim bargains.
    // while the fSnPm are (potentially) asymmetric, 8-dim bargains.
    auto estMat = [pi, pj, si, ti, sj, tj](unsigned int sn, unsigned int pm) {
        KMatrix eSNPM = KMatrix(8, 1);
        double di = 0;
        double dj = 0;
        switch (pm) {
        case 0:
            di = (pj > pi) ? pj - pi : 0;
            dj = (pi > pj) ? pi - pj : 0;
            break;
        case 1:
            di = pj;
            dj = pi;
            break;
        case 2:
            di = pj*pj;
            dj = pi*pi;
            break;
        default:
            assert(false);
            break;
        }
        for (unsigned int k = 0; k < 4; k++) {
            double tik = ti(k, 0);
            double sik = si(k, 0);
            double tjk = tj(k, 0);
            double sjk = sj(k, 0);
            double wik = 1;
            double wjk = 1;
            switch (sn) {
            case 1:
                wik = sik;
                wjk = sjk;
                break;
            case 2:
                wik = sik*sik;
                wjk = sjk*sjk;
                break;
            default:
                assert(false);
                break;
            }
            double bik = ((wik*(1 - di)*tik) + (wjk*di*tjk)) / ((wik*(1 - di)) + (wjk*di));
            double bjk = ((wjk*(1 - dj)*tjk) + (wik*dj*tik)) / ((wjk*(1 - dj)) + (wik*dj));
            eSNPM(k, 0) = bik;
            eSNPM(k + 4, 0) = bjk;
        }
        return eSNPM;
    };

    vc->eval = eFn;
    vc->nghbrs = KBase::VHCSearch::vn2;

    auto p0 = KMatrix::uniform(rng, 8, 1, -0.05, +0.05);
    LOG(DEBUG) << getFormattedString("Initial NP: %+.4f", vc->eval(p0));
    sfn("Initial point: ", p0);

    auto rslt = vc->run(p0,
                        1000, 20, 1e-16,
                        0.01, 0.618, 1.25, 1e-8, // NP already multiplied by 1000
                        ReportingLevel::Low);

    double vBest = get<0>(rslt);
    KMatrix pBest = get<1>(rslt);  // an [8,1] column-vector
    unsigned int in = get<2>(rslt);
    unsigned int sn = get<3>(rslt);

    // The pattern seems to be that if ri,rj are both non-negative,
    // they always find a bargain, and it is always that they adopt
    // exactly the same position.
    // If ri,rj both negative, or mixed, then it seems they usually
    // find a bargain (I've not observed otherwise in dozens of runs).
    // Sometimes they adopt exactly the same position on all dimensions.
    // Sometimmes they diverge somewhat on dimensions 0, 1, and 2,
    // then diverge much more on dimension 3.
    // Roughly.
    //
    // Over 100 random trials, the observed RMS errors of the estimators were as follows:
    //   est    eS1P1   eS1P2   eS2P1   eS2P2   dS1P2   dS2P2   fS1P0   fS1P1   fS1P2   S2P0    fS2P1   fS2P2
    //  mean    0.0782  0.1183  0.0893  0.0609  0.1618  0.0642  0.2927  0.0782  0.1421  0.2895  0.0893  0.1130
    //  stdv    0.0268  0.0554  0.0446  0.0409  0.0495  0.0488  0.0449  0.0268  0.0387  0.0356  0.0446  0.0374
    //  min     0.0333  0.0345  0.0044  0.0003  0.0615  0.0001  0.2196  0.0333  0.0666  0.2227  0.0044  0.0383
    //  max     0.1499  0.252   0.1645  0.1528  0.2657  0.1622  0.4209  0.1499  0.2373  0.3846  0.1645  0.2257
    //
    // So, in terms of mean & stdv of the RMS estimation error
    // eS2P2 and dS2P2 were significantly better than the other others,
    // but not significantly different from each other. eS1P1 was almost as good,
    // while the asymmetric fSnPm were either identical (eS1P1 == fS1P1, eS2P1 == fS1P1)
    // or much worse, like fS1P0 and fS2P0. Interestingly, the asymmetric fSnP0 predictors
    // were all worse than all of the symmetric ones, while the asymmetric fSnP2 predictors
    // were were than all the symmetric except those using P2.
    //
    // Overall, eS2P2 does appear insignificantly better than dS2P2, in terms of mean and max,
    // for both RMS and MaxAbs metrics. Note that the xxxP1 metrics predict that the actors will
    // meet somewhere in the middle for each component, while the xxxP2 metrics do not.
    // At least to me, eS2P2 does appear a little simpler, so I will use it as the
    // closed form estimator.

    LOG(DEBUG) << "Iter:" << in <<  "Stable:" << sn;
    LOG(DEBUG) << getFormattedString("Best NP:  %+.4f", vBest);
    sfn("Best point: ", pBest);
    if (vBest < 0) {
        LOG(DEBUG) << "Found no bargain which both sides prefer to conflict";
    }
    else {
        LOG(DEBUG) << "Found a bargain which both sides prefer to conflict";
        LOG(DEBUG) << getFormattedString("0:4  %.4f", fabs(pBest(0, 0) - pBest(4, 0)));
        LOG(DEBUG) << getFormattedString("1:5  %.4f", fabs(pBest(1, 0) - pBest(5, 0)));
        LOG(DEBUG) << getFormattedString("2:6  %.4f", fabs(pBest(2, 0) - pBest(6, 0)));
        LOG(DEBUG) << getFormattedString("3:7  %.4f", fabs(pBest(3, 0) - pBest(7, 0)));

        double mb[] = {
            (pBest(0, 0) + pBest(4, 0)) / 2,
            (pBest(1, 0) + pBest(5, 0)) / 2,
            (pBest(2, 0) + pBest(6, 0)) / 2,
            (pBest(3, 0) + pBest(7, 0)) / 2
        }; // position
        auto b = KMatrix::arrayInit(mb, 4, 1);
        // now if b = p*ti + (1-p)*tj
        //       = ti + p*(tj-ti)
        //       = tj + (1-p)*(ti-tj)
        // then
        // p = ((b - ti)*(tj - ti)) / ((ti - tj)*(ti - tj));


        // This does a strictly linear regress to find the most accurate
        // linear interpolation possible. It's RMS error is much larger
        // than any of the Prob-Sal estimators, as it does standard
        // vector interpolation which treats all components the same, regardless of salience.
        //
        double estDI = dot(b - ti, tj - ti) / dot(ti - tj, ti - tj);
        double estDJ = 1 - estDI;
        LOG(DEBUG) << getFormattedString("Effective fractional shifts:  %+.5f  %+.5f", estDI, estDJ);
        KMatrix estB = ti + estDI*(tj - ti);
        sfn("Interpolated bargain:", estB);
        double estErr = rms(estB - b);
        LOG(DEBUG) << getFormattedString("Resulting RMS estimation error: %.4f", estErr);
        // note that dBS1P1 and dBS1P1 are identical to other estimators

        auto fS1P0 = estMat(1, 0);
        auto fS1P1 = estMat(1, 1);
        auto fS1P2 = estMat(1, 2);

        auto fS2P0 = estMat(2, 0);
        auto fS2P1 = estMat(2, 1);
        auto fS2P2 = estMat(2, 2);

        auto joinedRMS = [ pBest](const KMatrix & e) {
            return rms(KBase::joinV(e, e) - pBest);
        };

        sfn("Prob-Sal-weighted eBS1P1:", eBS1P1);
        sfn("Prob-Sal-weighted eBS1P2:", eBS1P2);
        sfn("Prob-Sal-weighted eBS2P1:", eBS2P1);
        sfn("Prob-Sal-weighted eBS2P2:", eBS2P2);
        sfn("Prob-Sal-weighted dBS1P2:", dBS1P2);
        sfn("Prob-Sal-weighted dBS2P2:", dBS2P2);

        LOG(DEBUG) << "Resulting RMS estimation error:";
        LOG(DEBUG) << "RMS of eS1P1  eS1P2  eS2P1  eS2P2  dS1P2  dS2P2  "
            << "fS1P0 fS1P1 fS1P2 S2P0 fS2P1 fS2P2 "
            << getFormattedString("%7.4f  %7.4f  %7.4f  %7.4f ",
                joinedRMS(eBS1P1), joinedRMS(eBS1P2), joinedRMS(eBS2P1), joinedRMS(eBS2P2))
            << getFormattedString("%7.4f  %7.4f  ", joinedRMS(dBS1P2), joinedRMS(dBS2P2))
            << getFormattedString("%7.4f  %7.4f  %7.4f  %7.4f  %7.4f  %7.4f  ",
                rms(fS1P0 - pBest), rms(fS1P1 - pBest), rms(fS1P2 - pBest),
                rms(fS2P0 - pBest), rms(fS2P1 - pBest), rms(fS2P2 - pBest));

        LOG(DEBUG) << "Resulting MaxAbs estimation error:";
        LOG(DEBUG) << "MaxAbs of eS1P1  eS1P2  eS2P1  eS2P2  dS1P2  dS2P2  "
            << getFormattedString("%7.4f  %7.4f  %7.4f  %7.4f ",
                maxAbs(eBS1P1 - b), maxAbs(eBS1P2 - b), maxAbs(eBS2P1 - b), maxAbs(eBS2P2 - b))
            << getFormattedString("%7.4f  %7.4f", maxAbs(dBS1P2 - b), maxAbs(dBS2P2 - b));
    }
    return;
}

void demoVHC03(uint64_t sd) {
    auto rng = new PRNG(sd);
    using KBase::KMatrix;
    using KBase::dot;
    using KBase::maxAbs;

    LOG(DEBUG) << "Nash bargaining problem, with 4D points and 8D bargains, vector salience and vector capabilities";
    LOG(DEBUG) << "Note the log-rolling on dimensions 0 and 1";

    auto vc = new KBase::VHCSearch();

    // this was setup so that log-rolling would be likely.
    // I cares about 0, but cannot much affect it.
    // I affects 1, but does not care much about it.
    // J cares about 1, but cannot much affect it.
    // J affects 0, but does not care much about it.
    // I've set it up so that in the initial power-weighted compromise along dim0 favors J,
    // si0*ci0 = 0.95 * 1 << 0.05 * 50 = sj0*cj0 and vice-versa on dim1.
    // Hence, we'd expect I to give J what J wants on 1, and J gives I what I wants on 0.
    //               dim0  dim1  dim2  dim3
    double tmi[] = { 0.85, 0.85, 0.85, 0.85 }; // position
    double smi[] = { 0.95, 0.05, 0.50, 0.50 }; // salience
    double cmi[] = { 1.0, 50.0, 25.0, 25.0 }; // capability

    double tmj[] = { 0.15, 0.15, 0.15, 0.15 }; // position
    double smj[] = { 0.05, 0.95, 0.05, 0.95 }; // salience
    double cmj[] = { 50.0, 1.0, 25.0, 25.0 }; // capability

    double minR = -0.95;
    double maxR = +0.95;
    // position, salience, prob-of-victory along each dimension separately
    auto ti = KMatrix::arrayInit(tmi, 4, 1);
    auto si = KMatrix::arrayInit(smi, 4, 1);
    auto ci = KMatrix::arrayInit(cmi, 4, 1);
    double ri = rng->uniform(minR, maxR);

    auto tj = KMatrix::arrayInit(tmj, 4, 1);
    auto sj = KMatrix::arrayInit(smj, 4, 1);
    auto cj = KMatrix::arrayInit(cmj, 4, 1);
    double rj = rng->uniform(minR, maxR);

    auto piFn = [ci, cj](unsigned int k, unsigned int n) {
        assert(0 == n); // column vector
        double cik = ci(k, 0);
        double cjk = cj(k, 0);
        return cik / (cik + cjk);
    };

    auto pi = KMatrix::map(piFn, 4, 1);
    KMatrix pj = (-1.0)*pi + 1;

    auto sfn = [](string s, KMatrix m) {
        LOG(DEBUG) << s << ":";
        trans(m).mPrintf(" %7.4f ");
    };

    // risk attitudes found by random search,
    // so J gains and I loses by BATNA
    ri = -0.1;
    rj = 0.9;

    LOG(DEBUG) << getFormattedString("ri: %+.3f", ri);
    sfn("ti:", ti);
    sfn("si:", si);
    sfn("ci:", ci);
    LOG(DEBUG) << getFormattedString("rj: %+.3f", rj);
    sfn("tj:", tj);
    sfn("sj:", sj);
    sfn("cj:", cj);

    sfn("pi:", pi);
    sfn("pj:", pj);

    auto oneDimBargain = [si, sj, pi, pj, ti, tj](unsigned int k, unsigned int n) {
        // This is the eS2P2 estimator, in one dimension
        double eps = 1e-8;
        assert(0 == n);
        double wi = si(k, 0) * pi(k, 0);
        double wj = sj(k, 0) * pj(k, 0);
        wi = (wi*wi) + eps;
        wj = (wj*wj) + eps;
        double bi = ((wi * ti(k, 0)) + (wj * tj(k, 0))) / (wi + wj);
        return bi;
    };

    auto batna = KMatrix::map(oneDimBargain, 4, 1);

    LOG(DEBUG) << "BATNA determined by bargaining along each dimension separately, using eS2P2.";
    sfn("BATNA: ", batna);

    // util to each actor of SQ
    double uiSQ = bvu(ti - ti, si, ri) + bvu(ti - tj, si, ri);
    double ujSQ = bvu(tj - ti, sj, rj) + bvu(tj - tj, sj, rj);

    // util to each actor of both adopting BATNA
    double uiB = bvu(ti - batna, si, ri) + bvu(ti - batna, si, ri);
    double ujB = bvu(tj - batna, sj, rj) + bvu(tj - batna, sj, rj);

    LOG(DEBUG) << getFormattedString("uiSQ: %.4f", uiSQ);
    LOG(DEBUG) << getFormattedString("uiB:  %.4f", uiB);
    LOG(DEBUG) << getFormattedString("delta: %+.4f", uiB - uiSQ);

    LOG(DEBUG) << getFormattedString("ujSQ: %.4f", ujSQ);
    LOG(DEBUG) << getFormattedString("ujB:  %.4f", ujB);
    LOG(DEBUG) << getFormattedString("delta: %+.4f", ujB - ujSQ);

    // Nash-product, compared to BATNA
    auto efn0 = [ti, uiB, si, ri, tj, ujB, sj, rj](const KMatrix & t2i, const KMatrix & t2j) {
        double ubi = bvu(ti - t2i, si, ri) + bvu(ti - t2j, si, ri);
        double ubj = bvu(tj - t2i, sj, rj) + bvu(tj - t2j, sj, rj);
        double np = nProd(ubi - uiB, ubj - ujB);
        return 100.0 * np;
    };

    double npv = efn0(batna, batna);
    LOG(DEBUG) << "NP of the BATNA: " << npv; // just a double-check

    auto getT2I = [](const KMatrix & v) {
        double m2i[] = { v(0, 0), v(1, 0), v(2, 0), v(3, 0) }; // position
        auto t2i = KMatrix::arrayInit(m2i, 4, 1);
        return t2i;
    };

    auto getT2J = [](const KMatrix & v) {
        double m2j[] = { v(4, 0), v(5, 0), v(6, 0), v(7, 0) }; // position
        auto t2j = KMatrix::arrayInit(m2j, 4, 1);
        return t2j;
    };

    auto eFn = [getT2I, getT2J, efn0](KMatrix v) {
        unsigned int dim = v.numR();
        assert(8 == dim);
        double np = efn0(getT2I(v), getT2J(v));
        return np;
    };

    vc->eval = eFn;
    vc->nghbrs = KBase::VHCSearch::vn2;

    auto p0 = KMatrix::uniform(rng, 8, 1, -0.5, +1.5); // deliberately outside the allowed [0,1] range
    LOG(DEBUG) << getFormattedString("Initial NP: %+.4f", vc->eval(p0));
    sfn("Initial point: ", p0);

    auto rslt = vc->run(p0,
                        1000, 20, 1e-16,
                        0.01, 0.618, 1.25, 1e-8, // NP already multiplied by 1000
                        ReportingLevel::Low);

    double vBest = get<0>(rslt);
    KMatrix pBest = get<1>(rslt);  // an [8,1] column-vector
    unsigned int in = get<2>(rslt);
    unsigned int sn = get<3>(rslt);

    LOG(DEBUG) << "Iter:" << in << "  Stable:" << sn;
    LOG(DEBUG) << getFormattedString("Best NP:  %+.4f", vBest);
    sfn("Best point: ", pBest);
    if (vBest < 0) {
        LOG(DEBUG) << "Found no bargain which both sides prefer to BATNA";
    }
    else {
        // it turns out to have exactly the log-rolling which was expected
        // from the initial setup. Interestingly, it turns out to be symmetric
        // in that both actors adopt the same position at the Nash Bargain,
        // even though they use different metrics and risk attitudes.
        LOG(DEBUG) << "Found a bargain which both sides prefer to BATNA";
        auto t2I = getT2I(pBest);
        auto t2J = getT2J(pBest);
        sfn("t2I:", t2I);
        sfn("t2J:", t2J);
        LOG(DEBUG) << getFormattedString("Norm of t2I shift from BATNA: %.3f", norm(t2I - batna));
        LOG(DEBUG) << getFormattedString("Norm of t2J shift from BATNA: %.3f", norm(t2J - batna));
    }
    return;
}



VBool TargetedBV::target;

TargetedBV::TargetedBV() {
    bits = VBool();
    auto n = ((const unsigned int)(target.size()));
    for (unsigned int i = 0; i < n; i++) {
        bits.push_back(false);
    }
}


TargetedBV::TargetedBV(const VBool & b) {
    bits = b;
}

TargetedBV::~TargetedBV() {  }
void TargetedBV::setTarget(vector< bool > trgt)
{
    assert(0 < trgt.size());
    target = trgt;
    return;
}
VBool TargetedBV::getTarget() {
    return target;
}
VBool TargetedBV::randomBV(PRNG* rng, unsigned int nb) {
    uint64_t b = rng->uniform();
    auto bv = VBool();
    bv.resize(nb);
    for (unsigned int i = 0; i < nb; i++) {
        bv[i] = (1 == (b & 0x1));
        if (0 == b) {
            b = rng->uniform();
        }
        b = b >> 1;
    }
    return bv;
}
void TargetedBV::randomize(PRNG* rng) {
    bits = randomBV(rng, target.size());
    return;
}
TargetedBV * TargetedBV::mutate(PRNG * rng) const {
    auto g2 = new TargetedBV();
    g2->bits = bits;
    unsigned int n = ((unsigned int)(rng->uniform() % target.size()));
    g2->bits[n] = !(bits[n]);
    unsigned int m = ((unsigned int)(rng->uniform() % target.size()));
    g2->bits[m] = !(bits[m]);
    return g2;
}
tuple<TargetedBV*, TargetedBV*>  TargetedBV::cross(const TargetedBV * g2, PRNG * rng) const {
    unsigned int nc = crossSite(rng, target.size());
    auto h1 = new TargetedBV();
    auto h2 = new TargetedBV();

    for (unsigned int i = 0; i < target.size(); i++) {
        bool b1 = this->bits[i];
        bool b2 = g2->bits[i];
        if (i < nc) {
            h1->bits[i] = b1;
            h2->bits[i] = b2;
        }
        else {
            h1->bits[i] = b2;
            h2->bits[i] = b1;
        }
    }
    auto pr = tuple<TargetedBV*, TargetedBV*>(h1, h2);
    return pr;
}
void TargetedBV::show() const {
    showBits(bits);
    return;
}
void TargetedBV::showBits(VBool bv) {
    string bitVals;
    for (unsigned int i = 0; i < bv.size(); i++) {
        bool bi = bv[i];
        if (bi) {
            bitVals += "+";
        }
        else {
            bitVals += "o";
        }
    }
    LOG(DEBUG) << bitVals;
    return;
}
bool TargetedBV::equiv(const TargetedBV * g2) const {
    bool e = true;
    for (unsigned int i = 0; i < target.size(); i++) {
        bool b1 = this->bits[i];
        bool b2 = g2->bits[i];
        e = e && (b1 == b2);
    }
    return e;
}
double TargetedBV::evaluate() {
    double v = 100.0 - hDist(target);
    return v;
}
unsigned int TargetedBV::hDist(VBool bv) const {
    assert(bv.size() == target.size());
    unsigned int hd = 0;
    for (unsigned int i = 0; i < bv.size(); i++) {
        bool b1 = bits[i];
        bool b2 = bv[i];
        if (b1 != b2) {
            hd = hd + 1;
        }
    }
    return hd;
}
double TargetedBV::tblEval(double minD, vector<double> wght, vector<VBool> tbl) const {
    assert(0 < minD);
    double num = 0.0;
    double dnm = 0.0;
    for (unsigned int i = 0; i < tbl.size(); i++) {
        VBool ti = tbl[i];
        double di = minD + hDist(ti);
        num = num + (wght[i] / di);
        dnm = dnm + (1.0 / di);
    }
    return (num / dnm);
}

void parallelMatrixMult(PRNG * rng) {
    // Interestingly, this does not start all CPU's right away,
    // unlike demoThreadLambda

    using std::async;
    using std::future;
    using std::launch;

    // With [150,7000]*[7000,150], this flock of threads spends about the first 15% of its time
    // effectively single-threaded, then suddenly switches over to using all four CPUs
    // for the other 85%.
    // With shorter tasks, the "one-cpu-leadin" is reduced or eliminated.
    const unsigned int crc = 150 * 7000 * 150 / 2;
    const unsigned int r1 = 100;
    const unsigned int c2 = r1;
    const unsigned int cr = crc / (r1*c2);
    LOG(DEBUG) << getFormattedString(
        "Starting concurrent matrix multiply of [%u,%u] by [%u, %u]", r1, cr, cr, c2);
    LOG(DEBUG) << "This will launch" << r1*c2 << "threads";

    auto m1 = KMatrix::uniform(rng, r1, cr, -10, 50);
    auto m2 = KMatrix::uniform(rng, cr, c2, -10, 50);
    auto m3A = m1 * m2;
    auto m3B = KMatrix(r1, c2);

    auto mik = [&m1, &m2, &m3B](unsigned int i, unsigned int k) {
        const unsigned int cr = m1.numC();
        const unsigned m = 7;
        const unsigned int st = 1;
        assert(m2.numR() == cr);
        double s = 0.0;
        for (unsigned int j = 0; j < cr; j++) {
            if (0 == (j%m)) { // force interleaving
                //printf("Thread %3i,%3i sleeping %2i milliseconds \n", i, k, st);
                //cout << flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(st));
            }
            s = s + m1(i, j)*m2(j, k);
        }
        m3B(i, k) = s;
        LOG(DEBUG) << "Thread (" << i << "," << k << ") completed";
        return;
    };

    auto pfs = vector< thread >();
    for (unsigned int i = 0; i < r1; i++) {
        for (unsigned int k = 0; k < c2; k++) {
            pfs.push_back(thread([mik, i, k]() {
                mik(i, k);
                return;
            }));
        }
    }

    for (auto & t : pfs) {
        t.join();
    }

    LOG(DEBUG) << getFormattedString("Diff in matrix mults: %.3E", norm(m3A - m3B));

    return;
}

}// namespace

// -------------------------------------------------

int main(int ac, char **av) {
    // Set logging configuration from a file
    el::Configurations confFromFile("./kutils-logger.conf");
    el::Loggers::reconfigureAllLoggers(confFromFile);

    using KBase::dSeed;
    using UDemo::TargetedBV;
    auto sTime = KBase::displayProgramStart();
    uint64_t seed = dSeed;
    bool matrixP = false;
    bool goptP = false;
    bool vhcP = false;
    unsigned int vhcN = 0;
    bool ghcP = false;
    // unsigned int ghcN = 0;
    bool pMultP = false;
    bool vimcpP = false;
    unsigned int vimcpN = 0;
    bool threadP = false;
    bool uiP = false;
    bool run = true;

    // tmp args
    //vimcpP = true;
    //vimcpN = 2;

    auto showHelp = []() {
        printf("\n");
        printf("Usage: specify one or more of these options\n");
        printf("\n");
        printf("--help            print this message and exit \n");
        printf("\n");
        printf("--matrix          matrix functions \n");
        printf("\n");
        printf("--pMult           asynchronous parallel matrix multiply (very slow) \n");
        printf("\n");
        printf("--gopt            genetic optimization \n");
        printf("\n");
        printf("--ui              unique indices \n");
        printf("\n");
        printf("--vhc <n>         vector hill-climbing \n");
        printf("                  0: maximizing a simple quadratic \n");
        printf("                  1: Nash bargaining between two agents in 1D \n");
        printf("                  2: Nash bargaining between two agents in 4D, with scalar capabiities \n");
        printf("                  3: Nash bargaining between two agents in 4D, with vector capabilities \n");
        printf("\n");
        printf("--ghc             general hill-climbing to maximize a function of bit-vectors \n");
        printf("\n");
        printf("--vimcp <n>       VI and MCP \n");
        printf("                  0: the MCP minimize a quadratic subject to box constraints \n");
        printf("                  1: linear VI with ellipsoidal constraints \n");
        printf("                  2: Anti-Lemke linear VI \n");
        printf("\n");
        printf("--thread          several thread operations \n");
        printf("\n");
        printf("--seed <n>        set a 64bit seed \n");
        printf("                  0 means truly random \n");
        printf("                  default: %020llu \n", dSeed);
    };

    // a list of <keyword, description, lambda-fn>
    // might be enough to do this - except for the arguments to options.
    if (ac > 1) {
        for (int i = 1; i < ac; i++) {
            if (strcmp(av[i], "--seed") == 0) {
                i++;
                seed = std::stoull(av[i]);
            }
            else if (strcmp(av[i], "--matrix") == 0) {
                matrixP = true;
            }
            else if (strcmp(av[i], "--pMult") == 0) {
                pMultP = true;
            }
            else if (strcmp(av[i], "--thread") == 0) {
                threadP = true;
            }
            else if (strcmp(av[i], "--gopt") == 0) {
                goptP = true;
            }
            else if (strcmp(av[i], "--vhc") == 0) {
                vhcP = true;
                i++;
                vhcN = std::stoi(av[i]);
            }
            else if (strcmp(av[i], "--ghc") == 0) {
                ghcP = true;
                //i++;
                //ghcN = stoi(av[i]);
            }
            else if (strcmp(av[i], "--ui") == 0) {
                uiP = true;
            }
            else if (strcmp(av[i], "--vimcp") == 0) {
                vimcpP = true;
                i++;
                vimcpN = std::stoi(av[i]);
            }
            else if (strcmp(av[i], "--help") == 0) {
                run = false;
            }
            else {
                run = false;
                printf("Unrecognized argument: %s\n", av[i]);
            }
        }
    }
    else { // no arguments
        run = false;
    }

    if (!run) {
        showHelp();
        return 0;
    }


    PRNG * rng = new PRNG();
    if (0 == seed) {
        seed = rng->setSeed(seed); // 0 == get a random number
    }

    //printf("Using PRNG seed:  %020llu \n", seed);
    //printf("Same seed in hex:   0x%016llX \n", seed);
    // Unix correctly prints all digits with lu, lX, llu, and llX.
    // Windows only prints part, with lu, lX, llu, and llX.
    // JAH 20161103 changed back to llu/llx
    LOG(DEBUG) << getFormattedString("Using PRNG seed:  %020llu", seed);
    LOG(DEBUG) << getFormattedString("Same seed in hex:   0x%016llx", seed);


    //    UDemo::demoCoords(rng);

    if (threadP) {
        UDemo::demoThreadLambda(10);
        LOG(DEBUG) << "Demo using mutex to protect counter ...";
        UDemo::demoThreadSynch(10);
        UDemo::demoThreadSynch(10);
        UDemo::demoThreadSynch(10);
        UDemo::demoThreadSynch(10);
        UDemo::demoThreadSynch(10);
    }

    if (matrixP) {
        rng->setSeed(seed);
        UDemo::demoMatrix(rng);
    }

    if (pMultP) {
        rng->setSeed(seed);
        UDemo::parallelMatrixMult(rng);
    }

    if (goptP) {
        rng->setSeed(seed);
        UDemo::demoGA(rng);
    }

    if (ghcP) {
        rng->setSeed(seed);
        UDemo::demoGHC(rng);
    }

    if (vhcP) {
        //rng->setSeed(seed);
        switch (vhcN) {
        case 0:
            UDemo::demoVHC00(seed);
            break;
        case 1:
            UDemo::demoVHC01(seed);
            break;
        case 2:
            UDemo::demoVHC02(seed);
            break;
        case 3:
            UDemo::demoVHC03(seed);
            break;
        default:
            LOG(DEBUG) << "Unrecognized vhcN: " << vhcN;
        }
    }

    if (vimcpP) {
        rng->setSeed(seed);
        switch (vimcpN) {
        case 0:
            UDemo::demoABG00(rng);
            break;
        case 1:
            UDemo::demoEllipse(rng);
            break;
        case 2:
            UDemo::demoAntiLemke(rng, 25);
            break;
        default:
            LOG(DEBUG) << "Unrecognized vimcpN: " << vimcpN;
        }
    }

    if (uiP) {
        UDemo::demoUIndices();
    }

    delete rng;
    KBase::displayProgramEnd(sTime);
    return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
