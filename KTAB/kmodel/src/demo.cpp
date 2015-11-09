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
// Demonstrate some very basic functionality of
// the fundamental KBase namespace, from classes
// and functions in the MDemo namespace.
// --------------------------------------------

#include "kutils.h"
#include "kmodel.h"
#include "gaopt.h"
#include "hcsearch.h"
#include "demo.h"
#include "demomtch.h"
#include "demoleon.h"


using KBase::PRNG;
using KBase::KMatrix;

using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::State;
using KBase::VotingRule;
using KBase::vrName;

using KBase::GAOpt;
using KBase::GHCSearch;

namespace MDemo { // a namespace of which M, A, P, S know nothing
using std::cout;
using std::endl;
using std::flush;
using std::get;
using KBase::ReportingLevel;


void demoPCE(uint64_t s, PRNG* rng) {
    printf("Using PRNG seed: %020llu \n", s);
    rng->setSeed(s);

    cout << "Demonstrate minimal PCE" << endl << endl;

    Model::VPModel vpm;
    if (0 == (rng->uniform() % 2)) {
        vpm = Model::VPModel::Linear;
    }
    else {
        vpm = Model::VPModel::Square;
    }

    cout << "First, stable distrib is exactly as expected in bilateral conflict" << endl;

    auto cFn = [rng](unsigned int i, unsigned int j) {
        if (i == j) {
            return 0.0;
        }
        else {
            double c = rng->uniform(1.0, 10.0);
            return (c*c);
        }
    };

    auto c = KMatrix::map(cFn, 2, 2);
    double w01 = c(0, 1);
    double w10 = c(1, 0);

    auto p1 = KMatrix(2, 1);
    switch (vpm) {
    case  Model::VPModel::Linear:
        // no change
        break;
    case Model::VPModel::Square:
        w01 = w01*w01;
        w10 = w10*w10;
        break;
    }
    p1(0, 0) = w01 / (w01 + w10);
    p1(1, 0) = w10 / (w01 + w10);

    cout << "By simple " << Model::VPMName(vpm) << " ratios ..." << endl;
    printf("Prob[0>1] = %.4f \n", p1(0, 0));
    printf("Prob[1>0] = %.4f \n", p1(1, 0));
    cout << endl;

    auto pv = Model::vProb(vpm, c);
    auto p2 = Model::probCE(pv);

    auto show = [](const KMatrix & cMat, const KMatrix & pMat, const KMatrix & pVec) {
        cout << "Coalitions matrix:" << endl;
        cMat.printf(" %6.3f ");
        cout << endl;
        cout << "prob[i>j] Markov transitions matrix:" << endl;
        pMat.printf(" %.4f ");
        cout << endl;
        cout << "limiting stable prob[i] vector:" << endl;
        pVec.printf(" %.4f ");
        cout << endl;
        return;
    };

    show(c, pv, p2);
    printf("Norm of difference: %.2E \n", norm(p1 - p2));
    cout << endl;

    cout << "But not so clear with tri-lateral conflict ..." << endl;
    c = KMatrix::map(cFn, 3, 3);
    pv = Model::vProb(vpm, c);
    p2 = Model::probCE(pv);
    cout << " Markov model: " << endl;
    show(c, pv, p2);
    p2 = Model::condPCE(pv);
    cout << " CondProb model: " << endl;
    show(c, pv, p2);

    return;
}



void demoSpVSR(uint64_t s, PRNG* rng) {
    using std::function;
    using std::get;
    using std::make_shared;
    using std::shared_ptr;
    using std::tuple;

    printf("Using PRNG seed: %020llu \n", s);
    rng->setSeed(s);

    cout << "Demonstrate shared_ptr<void> for returns" << endl;
    cout << "This will be necessary to return arbitrary structures " << endl;
    cout << "from bestTarget without using a bare void* pointer." << endl;
    cout << endl;

    // if x is of type shared_ptr(T), then x.get() is of type T*,
    // so we dereference a shared pointer as *(x.get()), aka *x.get()

    auto sp1 = make_shared<int>(42); // shared pointer to an integer
    printf("Use count sp1: %li \n", sp1.use_count());
    int* p1 = sp1.get(); // gets the  pointer
    cout << "The shared integer is " << *sp1.get() << endl;
    {   // create another reference
        auto sp2 = sp1;
        printf("Use count sp1: %li \n", sp1.use_count());
        // let it go out-of-scope
    }
    printf("Use count sp1: %li \n", sp1.use_count());
    cout << endl;
    const string fs = " %+6.2f ";
    // function<shared_ptr<void>(unsigned int, unsigned int)> fn
    auto fn = [rng, &fs](unsigned int nr, unsigned int nc) {
        auto m1 = KMatrix::uniform(rng, nr, nc, -10, +50);
        auto d = KBase::norm(m1);
        cout << "Inside lambda:" << endl;
        m1.printf(fs);
        shared_ptr<void> rslt = make_shared<tuple<double, KMatrix>>(d, m1); // shared_ptr version of (void*)
        return rslt;
    };


    shared_ptr<void> r1 = fn(5, 3); // DO NOT try "void* r = fn(5,3).get()" - it will segv
    printf("Reference count: %li \n", r1.use_count());

    auto r53 = *((tuple<double, KMatrix>*) r1.get());
    // we get() the bare pointer, a void*,
    // cast it to tuple<...>*,
    // then dereference that pointer.
    cout << endl << "As retrieved:" << endl;
    get<1>(r53).printf(fs);

    return;
}
} // namespace
// -------------------------------------------------


int main(int ac, char **av) {
    using std::cout;
    using std::endl;

    auto sTime = KBase::displayProgramStart();
    uint64_t dSeed = 0xD67CC16FE69C185C; // arbitrary
    uint64_t seed = dSeed;
    bool run = true;
    bool pceP = false;
    bool spvsrP = false;
    bool sqlP = false;

    auto showHelp = [dSeed]() {
        printf("\n");
        printf("Usage: specify one or more of these options\n");
        printf("--help            print this message\n");
        printf("--pce             simple PCE\n");
        printf("--spvsr           demonstrated shared_ptr<void> return\n");
        printf("--sql             demo SQLite \n"); 
        printf("--seed <n>        set a 64bit seed\n");
        printf("                  0 means truly random\n");
        printf("                  default: %020llu \n", dSeed);
    };

    // tmp args


    if (ac > 1) {
        for (int i = 1; i < ac; i++) {
            if (strcmp(av[i], "--seed") == 0) {
                i++;
                seed = std::stoull(av[i]);
            }
            else if (strcmp(av[i], "--pce") == 0) {
                pceP = true;
            }
            else if (strcmp(av[i], "--spvsr") == 0) {
                spvsrP = true;
            }
            else if (strcmp(av[i], "--sql") == 0) {
                sqlP = true;
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
    printf("Using PRNG seed:  %020llu \n", seed);
    printf("Same seed in hex:   0x%016llX \n", seed);


    // note that we reset the seed every time, so that in case something
    // goes wrong, we need not scroll back too far to find the
    // seed required to reproduce the bug.
    if (pceP) {
        cout << "-----------------------------------" << endl;
        MDemo::demoPCE(seed, rng);
    }
    if (spvsrP) {
        cout << "-----------------------------------" << endl;
        MDemo::demoSpVSR(seed, rng);
    }

    if (sqlP) {
        MDemo::demoSQLite();
    }


    cout << "-----------------------------------" << endl;

    delete rng;
    KBase::displayProgramEnd(sTime);
    return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
