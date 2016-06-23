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
// start of a simple Tetris clone
//---------------------------------------------

#include "kutils.h"
#include "tutils.h"
#include "prng.h"
#include "qmmain.h"
#include "qmUI.h"


namespace QuadMap {


QMApp::QMApp(uint64_t s) {
    rng = new PRNG();
    rng->setSeed(s);
    theApp = this;
}


void QMApp::run() {
    /*
    auto tw = new TetrisUI(defaultRows, defaultClms);
    setLevel(defaultLevel);
    setRC(defaultRows, defaultClms);
    tw->mainW->show();
    applyControlState(ControlState()); // use factory defaults
    Fl::run();
    */
    return;
}

QMApp::~QMApp() {
    delete rng;
    rng = nullptr;
    theApp = nullptr;
}



void QMApp::quit() {
    /*
    auto tw = TetrisUI::theUI;
    if (nullptr != tw) {
      tw->mainW->hide();
      tw->about->aw->hide();
      tw->controls->mw->hide();
      tw->notice->nw->hide();
    }
    */
    return;
}





}; // end of namespace


//---------------------------------------------
using QuadMap::QMApp;

QMApp* QMApp::theApp = nullptr;
QuadMapUI* QuadMapUI::theUI = nullptr;

int main(int ac, char ** av) {
    using std::cout;
    using std::endl;
    using std::string;
    using KBase::PRNG;

    auto sTime = KBase::displayProgramStart();
    uint64_t seed = KBase::dSeed;
    bool testP = false;
    bool run = true;
    auto showHelp = []() {
        printf("\n");
        printf("Usage: specify one or more of these options\n");
        printf("--help            print this message\n");
        printf("--test            test some basic utilities\n");
        printf("--seed <n>        set a 64bit seed\n");
        printf("                  0 means truly random\n");
        printf("                  default: %020llu \n", KBase::dSeed);
    };
    if (ac > 1) {
        for (int i = 1; i < ac; i++) {
            if (strcmp(av[i], "--seed") == 0) {
                i++;
                seed = std::stoull(av[i]);
            }
            else if (strcmp(av[i], "--test") == 0) {
                testP = true;
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
    printf("Using PRNG seed:  %020llu \n", seed);
    printf("Same seed in hex:   0x%016llX \n", seed);

    unsigned int n = 1000;
    printf("\nTesting %i ... ", n);
    QuadMap::testX(n);
    printf("done \n");

    printf("\nFilling occurance matrix ...");
    auto om = QuadMap::fillOccuranceMatrix(1000, 1000,  0.0, 4.0);
    printf("done \n");

    auto t = new QuadMap::QMApp(seed);
    t->run();

    delete t;
    t = nullptr;
    KBase::displayProgramEnd(sTime);
    return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
