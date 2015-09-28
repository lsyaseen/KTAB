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
// Demonstrate some very basic functionality in
// the fundamental KBase namespace
// -------------------------------------------------

#include "minwater.h"

using KBase::PRNG;

namespace DemoWaterMin {
using std::cout;
using std::endl;
using std::flush;
using std::function;
using std::get;
using std::tuple;
using std::vector;


using KBase::KMatrix;
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
        double wk = w(k,0);
        if (0 == k) {
            wk = wk / 1.0E5;
        }
        double vkij = Model::vote(KBase::VotingRule::Proportional, wk, uInit(k, i), uInit(k, j));
        return vkij;
    };
    auto c0 = Model::coalitions(v0fn, numA, numP); // [numP, numP]
    auto pv0 = Model::vProb(vpm, c0); // [numP, numP]
    auto pr0 = Model::condPCE(pv0); // [numP,1] 
    double err0 = pr0(0,0) - trgtP0;

    // voting in nominal-policy scenario
    auto v1fn = [w, p0](unsigned int k, unsigned int i, unsigned int j) {
        double vkij = Model::vote(KBase::VotingRule::Proportional, w(k, 0), uInit(k, i), uInit(k, j));
        return vkij;
    };
    auto c1 = Model::coalitions(v1fn, numA, numP); // [numP, numP]
    auto pv1 = Model::vProb(vpm, c1); // [numP, numP]
    auto pr1 = Model::condPCE(pv1); // [numP,1] 
    double err1 = pr1(1,0) - trgtP1;



    //double err = KBase::norm(pr1 - pInit) / sqrt(pr1.numC() * pr1.numR()); // RMS of difference in distributions

    double err = sqrt( ((err0*err0) + (err1*err1)) / 2.0); // RMS difference of the two critical probabilities
    

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

} // namespace


int main(int ac, char **av) {
    using std::cout;
    using std::endl;
    using std::flush;

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

    cout << "Error-minimizing search ..." << endl << flush;
    DemoWaterMin::minProbErr();

    delete rng;
    KBase::displayProgramEnd(sTime);
    return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
