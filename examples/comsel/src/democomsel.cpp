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
//
// Demonstrate a very basic, but highly parameterizable, model of committee selection
//
// --------------------------------------------

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>

#include "kutils.h"
#include "kmatrix.h"
#include "prng.h"
#include "kmodel.h"

#include "comsel.h"
#include "csmain.h"
#include "democomsel.h"


using KBase::PRNG;
using KBase::KMatrix;
using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::State;
using KBase::VotingRule;


namespace DemoComSel {
 
  using std::cout;
  using std::endl;
  using std::flush;
  using std::function;
  using std::get;
  using std::string;

  using KBase::ReportingLevel;

  using ComSelLib::CSModel;
  using ComSelLib::CSActor;
  using ComSelLib::CSState;

  // -------------------------------------------------
  void demoActorUtils(const uint64_t s, PRNG* rng) {
    printf("Using PRNG seed: %020llu \n", s);
    rng->setSeed(s);
    return;
  }


  void demoCS(unsigned int nParty, unsigned int nDim, const uint64_t s, PRNG* rng) {
    printf("Using PRNG seed: %020llu \n", s);
    rng->setSeed(s);

    if (0 == nParty) {
      nParty = 2 + (rng->uniform() % 4); // i.e. [2,6] inclusive
    }
    if (0 == nDim) {
      nDim = 1 + (rng->uniform() % 7); // i.e. [1,7] inclusive
    }

    printf("Num parties: %u \n", nParty);
    printf("Num dimensions: %u \n", nDim);
    
    Fl::scheme("standard"); // standard, plastic, gtk+, gleam

    auto mw = new CSMain();
    mw->mainWindow->show();
    Fl::run();
    delete mw;
    mw = nullptr;
 
    return;
  }


} // end of namespace


int main(int ac, char **av) {
  using std::cout;
  using std::endl;
  using std::string;

  auto sTime = KBase::displayProgramStart();
  uint64_t dSeed = 0xD67CC16FE69C185C; // arbitrary
  uint64_t seed = dSeed;
  bool run = true;

  cout << "comselApp version " << DemoComSel::appVersion << endl << endl;

  auto showHelp = [dSeed]() {
    printf("\n");
    printf("Usage: specify one or more of these options\n");
    printf("--help       print this message\n");
    printf("--seed <n>   set a 64bit seed\n");
    printf("             0 means truly random\n");
    printf("             default: %020llu \n", dSeed);
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
  printf("Using PRNG seed: %020llu \n", seed);
  printf("Same seed in hex: 0x%016llX \n", seed);

  // note that we reset the seed every time, so that in case something
  // goes wrong, we need not scroll back too far to find the
  // seed required to reproduce the bug.
  DemoComSel::demoCS(4, 6, seed, rng);

  delete rng;
  KBase::displayProgramEnd(sTime);
  return 0;
}

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
