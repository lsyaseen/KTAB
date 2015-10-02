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
// Demonstrate some very basic bargaining
// over how to minimize water usage.
// -------------------------------------------------

#include "vimcp.h"
#include "minwater.h"

using std::cout;
using std::endl;
using std::flush;
using std::get;
using std::string;
using std::tuple;

using KBase::KMatrix;
using KBase::PRNG;


namespace DemoWaterMin {
using std::function;
using std::vector;
using KBase::ReportingLevel;
using KBase::VHCSearch;

using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::State;
using KBase::VotingRule;


// -------------------------------------------------
double waterMinProb(ReportingLevel rl, const KMatrix & p0) {
    // Report the RMS error in predicted probabilities
    // TODO: change this to get both initial-year and nominal-policy
    // p0 is a set of modifiers on initial weights, w

    //const unsigned int numA = 4;
    //const unsigned int numP = 4;
    assert(p0.numR() == numA);
    assert(p0.numC() == 1);

    // base weights
    //double wArray[] = { 1600, 7000, 100, 1300 }; // numA
    //const KMatrix w0 = KMatrix::arrayInit(wArray, numA, 1);
    const double w0Sum = KBase::sum(w0);

    KMatrix w = KMatrix(numA, 1);
    for (unsigned int i = 0; i < numA; i++) {
        double pi = p0(i, 0);
        w(i, 0) = exp(pi) * w0(i, 0);
    }
    double wSum = KBase::sum(w);
    w = (w0Sum / wSum)*w;

    double pRMS = KBase::norm(p0) / sqrt(p0.numC() * p0.numR()); // RMS of p0;

    auto vpm = Model::VPModel::Linear;

    // voting in base-year scenario
    auto v0fn = [w, p0](unsigned int k, unsigned int i, unsigned int j) {
        double wk = w(k, 0);
        if (0 == k) {
            wk = wk / 1.0E5;
        }
        double vkij = Model::vote(KBase::VotingRule::Proportional, wk, uInit(k, i), uInit(k, j));
        return vkij;
    };
    auto c0 = Model::coalitions(v0fn, numA, numP); // [numP, numP]
    auto pv0 = Model::vProb(vpm, c0); // [numP, numP]
    auto pr0 = Model::condPCE(pv0); // [numP,1]
    double err0 = pr0(0, 0) - trgtP0;

    // voting in nominal-policy scenario
    auto v1fn = [w, p0](unsigned int k, unsigned int i, unsigned int j) {
        double vkij = Model::vote(KBase::VotingRule::Proportional, w(k, 0), uInit(k, i), uInit(k, j));
        return vkij;
    };
    auto c1 = Model::coalitions(v1fn, numA, numP); // [numP, numP]
    auto pv1 = Model::vProb(vpm, c1); // [numP, numP]
    auto pr1 = Model::condPCE(pv1); // [numP,1]
    double err1 = pr1(1, 0) - trgtP1;



    //double err = KBase::norm(pr1 - pInit) / sqrt(pr1.numC() * pr1.numR()); // RMS of difference in distributions

    double err = sqrt(((err0*err0) + (err1*err1)) / 2.0); // RMS difference of the two critical probabilities


    if (ReportingLevel::Silent < rl) {
        cout << "Actor-cap matrix" << endl;
        w.printf(" %8.1f ");
        cout << endl << flush;

        cout << "Raw actor-pos util matrix" << endl;
        uInit.printf(" %.4f ");
        cout << endl << flush;

        cout << "Coalition strength matrix" << endl;
        c1.printf(" %+9.3f ");
        cout << endl << flush;

        cout << "Probability Opt_i > Opt_j" << endl;
        pv1.printf(" %.4f ");
        cout << endl;

        cout << "Estimated probability Opt_i" << endl;
        pr1.printf(" %.4f ");
        cout << endl << flush;

        cout << "Desired probability Opt_i" << endl;
        pInit.printf(" %.4f ");
        cout << endl << flush;

        printf("RMS error: %.4f \n", err);

        printf("RMS weight factors: %.4f \n", pRMS);
    }

    return (err + (pRMS * prmsW));
}

void minProbErr() {
    //const unsigned int numA = 4;
    auto vhc = new VHCSearch();
    auto eRL = ReportingLevel::Low;
    auto rRL = ReportingLevel::Medium;

    vhc->eval = [eRL](const KMatrix & wm) {
        const double err = waterMinProb(eRL, wm);
        return 1 - err;
    };

    vhc->nghbrs = VHCSearch::vn1;
    auto p0 = KMatrix(numA, 1); // all zeros
    cout << "Initial point: ";
    trans(p0).printf(" %+.4f ");
    cout << endl;
    auto rslt = vhc->run(p0,
                         100, 10, 1E-5, // iMax, sMax, sTol
                         1.0, 0.618, 1.25, 1e-8, // step, shrink, grow, minStep
                         rRL);
    double vBest = get<0>(rslt);
    KMatrix pBest = get<1>(rslt);
    unsigned int in = get<2>(rslt);
    unsigned int sn = get<3>(rslt);
    delete vhc;
    vhc = nullptr;
    printf("Iter: %i  Stable: %i \n", in, sn);
    printf("Best value: %+.4f \n", vBest);
    cout << "Best point:    ";
    trans(pBest).printf(" %+.4f ");
    cout << endl;
    return;
}


RsrcMinLP::RsrcMinLP() {
    clear();
}



RsrcMinLP::~RsrcMinLP() {
    clear();
}



void RsrcMinLP::clear() {
    numPortC = 0;
    numProd = 0;
    xInit = KMatrix();
    rCosts = KMatrix();
    bounds = KMatrix();
    portRed = KMatrix();
    portWghts = KMatrix();
}

RsrcMinLP* RsrcMinLP::makeRMLP(PRNG* rng, unsigned int numPd, unsigned int numPt) {
    auto rmlp = new RsrcMinLP();
    rmlp->numProd = numPd;
    rmlp->numPortC = numPt;
    rmlp->xInit = KMatrix::uniform(rng, numPd, 1, 100.0, 990.0);
    rmlp->rCosts = KMatrix::uniform(rng, numPd, 1, 10.0, 90.0);
    auto rgFn = [rng](unsigned int i, unsigned int j) {
        double f = 0.0;
        switch (j) {
        case 0: // reduction
            f = rng->uniform(0.05, 0.20); // 5% to 20% decrease
            break;
        case 1: // growth
            f = rng->uniform(0.50, 1.00); // 50% to 100% increase (i.e. double)
            break;
        default:
            assert(false);
            break;
        }
        return f;
    };
    rmlp->bounds = KMatrix::map(rgFn, numPd, 2);
    rmlp->portRed = KMatrix::uniform(rng, numPt, 1, 0.0, 0.10); // reductions are 0 to 10 percent
    rmlp->portWghts = KMatrix::uniform(rng, numPt, numPd, 0.0, 1.0);

    auto wfn = [rng](unsigned int r, unsigned int c) {
        double wij = rng->uniform(0.0, 1.0);
        if (wij < 0.5) {
            wij = 0.0; // not in this portfolio
        }
        return wij;
    };
    rmlp->portWghts = KMatrix::map(wfn, numPt, numPd);
    return rmlp;
}

} // namespace

// -------------------------------------------------

int main(int ac, char **av) {
    using std::cout;
    using std::endl;
    using std::flush;
    using KBase::dot;
    using KBase::joinV;
    using KBase::joinH;
    using KBase::trans;
    using DemoWaterMin::RsrcMinLP;

    auto sTime = KBase::displayProgramStart();
    uint64_t dSeed = 0xD67CC16FE69C2868; // arbitrary
    uint64_t seed = dSeed;
    bool run = true;

    auto showHelp = [dSeed]() {
        printf("\n");
        printf("Usage: specify one or more of these options\n");
        printf("--help            print this message\n");
        printf("--seed <n>        set a 64bit seed\n");
        printf("                  0 means truly random\n");
        printf("                  default: %020lu \n", dSeed);
    };

    // tmp args
    // seed = 0;


    if (ac > 1) {
        for (int i = 1; i < ac; i++) {
            if (strcmp(av[i], "--seed") == 0) {
                i++;
                seed = std::stoull(av[i]);
            }
            else if (strcmp(av[i], "--help") == 0) {
                run = false;
            }
            else {
                run = false;
                printf("Unrecognized argument %s\n", av[i]);
            }
        }
    }

    if (!run) {
        showHelp();
        return 0;
    }

    PRNG * rng = new PRNG();
    seed = rng->setSeed(seed); // 0 == get a random number
    printf("Using PRNG seed:  %020lu \n", seed);
    printf("Same seed in hex:   0x%016lX \n", seed);

    // note that we reset the seed every time, so that in case something
    // goes wrong, we need not scroll back too far to find the
    // seed required to reproduce the bug.

    cout << "-----------------------------------" << endl;

    //auto p = DemoWaterMin::waterMinProb(KBase::ReportingLevel::Medium, KBase::KMatrix());

    auto pfn = [](string lbl, string f, KMatrix m) {
        cout << lbl << endl;
        m.printf(f);
        cout << endl << flush;
        return;
    };

    const unsigned int numP = 50;
    const unsigned int numC = numP / 5;
    const unsigned int iterLim = 100 * 1000;

    auto rmlp = RsrcMinLP::makeRMLP(rng, numP, numC);
    printf("Number of products: %i \n", rmlp->numProd);
    pfn("Initial X: ", "%.2f ", rmlp->xInit);
    pfn("Resource costs: ", "%.2f ", rmlp->rCosts);
    const double rsrc0 = dot(rmlp->xInit, rmlp->rCosts);
    printf("Initial cost: %.2f \n", rsrc0);
    cout << endl << flush;

    pfn("Fractional reduction / growth bounds: ", "%.4f ", rmlp->bounds);

    auto rBounds = KMatrix(rmlp->numProd, 1);
    auto gBounds = KMatrix(rmlp->numProd, 1);
    for (unsigned int i = 0; i < rmlp->numProd; i++) {
        double xi = rmlp->xInit(i, 0);
        assert(0.0 <= xi);
        double ri = rmlp->bounds(i, 0);
        assert(0.0 <= ri);
        assert(ri <= 1.0);
        double gi = rmlp->bounds(i, 1);
        assert(0.0 <= gi);
        rBounds(i, 0) = (1.0 - ri)*xi;
        gBounds(i, 0) = (1.0 + gi)*xi;
    }

    pfn("Reduction bounds: ", "%7.2f ", rBounds);
    pfn("Growth bounds: ", "%7.2f ", gBounds);

    printf("Number of portfolios: %i \n", rmlp->numPortC);
    pfn("Portfolio weights: ", "%.3f ", rmlp->portWghts);
    pfn("Portfolio reductions: ", "%.3f ", rmlp->portRed);

    auto initPV = rmlp->portWghts * rmlp->xInit;
    pfn("Initial portfolio values: ", "%8.2f", initPV);
    auto portFn = [initPV, rmlp](unsigned int i, unsigned int j) {
        double bij = initPV(i, j)*(1 - rmlp->portRed(i, j));
        return bij;
    };
    auto minPV = KMatrix::map(portFn, initPV.numR(), initPV.numC());
    pfn("Minimum portfolio values: ", "%8.2f", minPV);

    // assemble all the above into a problem like
    // min c*x
    // Ax >= b
    // x >= 0

    // start with portfolio constraints
    auto matA = rmlp->portWghts;
    auto matB = minPV;

    auto ident = KBase::iMat(rmlp->numProd);

    // new value >= lower bound
    matA = joinV(matA, ident);
    matB = joinV(matB, rBounds);

    // new value <= upper bound, or
    // -(new value) >= -(upper bound)
    matA = joinV(matA, -1.0 * ident);
    matB = joinV(matB, -1.0 * gBounds);

    pfn("Full A-matrix: ", "%+.3f ", matA);
    pfn("Full b-vector: ", "%+.3f ", matB);

    const unsigned int N = rmlp->numProd;
    const unsigned int K = rmlp->numPortC;
    const unsigned int M = K + (2 * N);
    assert(M == matA.numR());
    assert(N == matA.numC());
    assert(M == matB.numR());
    assert(1 == matB.numC());

    auto topMat = joinH(KMatrix(N, N), -1.0*trans(matA));
    auto botMat = joinH(matA, KMatrix(M, M));

    auto matM = joinV(topMat, botMat);
    auto matQ = joinV(rmlp->rCosts, -1.0 * matB);

    assert(3 * N + K == matM.numR());
    assert(3 * N + K == matM.numC());
    assert(3 * N + K == matQ.numR());
    assert(1 == matQ.numC());

    const double eps = 1E-6;

    auto sfe = [](const KMatrix & x, const KMatrix & y) {
        return (2 * norm(x - y)) / (norm(x) + norm(y));
    };

    auto processRslt = [N, sfe, matM, matQ, eps](tuple<KMatrix, unsigned int, KMatrix> r) {
        KMatrix u = get<0>(r);
        unsigned int iter = get<1>(r);
        KMatrix res = get<2>(r);
        printf("After %i iterations  \n", iter);
        cout << "  LCP solution u:  ";
        trans(u).printf(" %+.3f ");
        cout << endl << flush;
        cout << "  LCP residual r:  ";
        trans(res).printf(" %+.3f ");
        cout << endl << flush;
        cout << "Dimensions: " << res.numR() << endl << flush;
        KMatrix v = matM*u + matQ;
        double tol = 100 * eps;
        double e1 = sfe(u, u);
        double e2 = sfe(v, matM*u + matQ);
        printf("SFE of u is %.3E,  SFE of v is %.3E \n", e1, e2);
        assert(e1 < tol); // inaccurate U
        assert(e2 < tol); // inaccurate V
        auto sFn = [u](unsigned int i, unsigned int j) {
            assert(0 == j);
            return u(i, 0);
        };
        auto x = KMatrix::map(sFn, N, 1);
        cout << "  LP solution x:  ";
        trans(x).printf(" %+.3f ");
        cout << endl << flush;
        return x;
    };


    auto start = joinV(rmlp->xInit, KMatrix(M, 1));
    auto F = [matM, matQ](const KMatrix & x) {
        return (matM*x + matQ);
    };

    if (true) {
        try {
            cout << endl << flush;
            cout << "Solve via BSHe96" << endl;
            auto r1 = solveLVI_BSHe96(matM, matQ, KBase::projPos, start, eps, iterLim);
            auto x1 = processRslt(r1);

            printf("Initial resource usage:   %10.2f \n", rsrc0);
            const double rsrc1 = dot(x1, rmlp->rCosts);
            printf("Minimized resource usage: %10.2f \n", rsrc1);
            printf("Percentage change %+.3f", (100.0*(rsrc1 - rsrc0) / rsrc0));
        }
        catch (...) {
            cout << "Caught exception" << endl << flush;
        }
    }

    if (false) { //  exceeds the iteration limit much more often than BSHe96
        try {
            cout << endl << flush;
            cout << "Solve via ABG" << endl;
            auto r2 = viABG(start, F, KBase::projPos, 0.5, eps, iterLim);
            auto x2 = processRslt(r2);

            printf("Initial resource usage:   %10.2f \n", rsrc0);
            const double rsrc2 = dot(x2, rmlp->rCosts);
            printf("Minimized resource usage: %10.2f \n", rsrc2);
            printf("Percentage change %+.3f", (100.0*(rsrc2 - rsrc0) / rsrc0));
        }
        catch (...) {
            cout << "Caught exception" << endl << flush;
        }
    }

    delete rmlp;
    rmlp = nullptr;


    //cout << "Error-minimizing search ..." << endl << flush;
    //DemoWaterMin::minProbErr();

    delete rng;
    rng = nullptr;
    KBase::displayProgramEnd(sTime);
    return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
