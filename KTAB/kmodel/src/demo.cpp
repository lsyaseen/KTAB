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

#include "demo.h"
#include "edemo.h"
#include "tinyxml2demo.h" 
#include "csvdemo.h"
#include <easylogging++.h>

INITIALIZE_EASYLOGGINGPP

using std::get;
using std::string;
using std::tuple;
using std::vector;

using KBase::PRNG;
using KBase::KMatrix;

using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::ReportingLevel;
using KBase::State;

using KBase::PCEModel;
using KBase::VotingRule;
using KBase::VPModel;

namespace MDemo { // a namespace of which M, A, P, S know nothing

// --------------------------------------------

void demoPCE(uint64_t s, PRNG* rng) {
  LOG(DEBUG) << KBase::getFormattedString("demoPCE using PRNG seed:  %020llu", s);
  rng->setSeed(s);

  LOG(DEBUG) << "Demonstrate minimal PCE";

  VPModel vpm = VPModel::Linear;
  switch (rng->uniform() % 6) {
  case 0:
  case 1:
    vpm = VPModel::Linear;
    break;
  case 2:
    vpm = VPModel::Square;
    break;
  case 3:
    vpm = VPModel::Quartic;
    break;
  case 4:
    vpm = VPModel::Octic;
    break;
  case 5: // unused
    vpm = VPModel::Binary;
    break;
  default:
    LOG(DEBUG) << "Unrecognized VPModel option";
    assert(false);
    break;
  }

  LOG(DEBUG) << "Using VPModel " << vpm;

  LOG(DEBUG) << "First, stable distrib is exactly as expected in bilateral conflict";

  auto cFn = [rng](unsigned int i, unsigned int j) {
    if (i == j) {
      return 0.0;
    }
    else {
      double c = rng->uniform(1.0, 10.0);
      return (c*c);
    }
  };

  auto show = [](const KMatrix & cMat, const KMatrix & pvMat, const KMatrix & pVec) {
    LOG(DEBUG) << "Coalitions matrix:";
    cMat.mPrintf(" %6.3f ");
    LOG(DEBUG) << "prob[i>j] Markov transitions matrix:";
    pvMat.mPrintf(" %.4f ");
    LOG(DEBUG) << "limiting stable prob[i] vector:";
    pVec.mPrintf(" %.4f ");
    return;
  };
  LOG(DEBUG) << "Compare simple " << vpm << " ratios to2-by-2  Markov-uniform ...";
  auto c = KMatrix::map(cFn, 2, 2);
  auto p3 = Model::markovIncentivePCE(c, vpm);
  auto ppv = Model::probCE2(PCEModel::MarkovUPCM, vpm, c);
  auto p2 = get<0>(ppv); // column
  auto pv = get<1>(ppv); //square
  LOG(DEBUG) << "2-Option Markov Uniform ";
  show(c, pv, p2);
  LOG(DEBUG) << "Markov Incentive";
  show(c, pv, p3);

  p3 = get<0>(Model::probCE2(PCEModel::MarkovIPCM, vpm, c));
  LOG(DEBUG) << "2-Option Markov Incentive";
  show(c, pv, p3);
  LOG(DEBUG) << "But not so clear with three options ...";
  c = KMatrix::map(cFn, 3, 3);
  ppv = Model::probCE2(PCEModel::MarkovUPCM, vpm, c);
  p2 = get<0>(ppv); // column
  pv = get<1>(ppv); //square
  LOG(DEBUG) << "3-Option Markov Uniform ";
  p3 = Model::markovIncentivePCE(c, vpm);

  LOG(DEBUG) << "Markov Uniform  ";
  show(c, pv, p2);
  LOG(DEBUG) << "Markov Incentive";
  show(c, pv, p3);

  LOG(DEBUG) << "3-Option Markov Incentive";
  LOG(DEBUG) << "probCE2 ... ";
  p3 = get<0>(Model::probCE2(PCEModel::MarkovIPCM, vpm, c));
  LOG(DEBUG) << "tmpMarkovIncentivePCE ... ";
  show(c, pv, p3);
  // ---------------------------
  LOG(DEBUG) << "Conditional PCE model: ";
  p2 = get<0>(Model::probCE2(PCEModel::ConditionalPCM, vpm, c));
  show(c, pv, p2);
  return;
}

// simple demo of shared pointers (largely a test that the compiler is recent enough).
void demoSpVSR(uint64_t s, PRNG* rng) {
  using std::function;
  using std::get;
  using std::make_shared;
  using std::shared_ptr;
  using std::tuple;

  LOG(DEBUG) << KBase::getFormattedString("demoSpVSR using PRNG seed:  %020llu", s);
  rng->setSeed(s);

  LOG(DEBUG) << "Demonstrate shared_ptr<void> for returns";
  LOG(DEBUG) << "This will be necessary to return arbitrary structures ";
  LOG(DEBUG) << "from bestTarget without using a bare void* pointer.";

  // if x is of type shared_ptr(T), then x.get() is of type T*,
  // so we dereference a shared pointer as *(x.get()), aka *x.get()

  auto sp1 = make_shared<int>(42); // shared pointer to an integer
  printf("Use count sp1: %li \n", sp1.use_count());
  //int* p1 = sp1.get(); // gets the  pointer
  LOG(DEBUG) << "The shared integer is " << *sp1.get();
  {   // create another reference
    auto sp2 = sp1;
    printf("Use count sp1: %li \n", sp1.use_count());
    // let it go out-of-scope
  }
  printf("Use count sp1: %li \n", sp1.use_count());
  const string fs = " %+6.2f ";
  // function<shared_ptr<void>(unsigned int, unsigned int)> fn
  auto fn = [rng, &fs](unsigned int nr, unsigned int nc) {
    auto m1 = KMatrix::uniform(rng, nr, nc, -10, +50);
    auto d = KBase::norm(m1);
    LOG(DEBUG) << "Inside lambda:";
    m1.mPrintf(fs);
    shared_ptr<void> rslt = make_shared<tuple<double, KMatrix>>(d, m1); // shared_ptr version of (void*)
    return rslt;
  };


  shared_ptr<void> r1 = fn(5, 3); // DO NOT try "void* r = fn(5,3).get()" - it will segv
  printf("Reference count: %li \n", r1.use_count());

  auto r53 = *((tuple<double, KMatrix>*) r1.get());
  // we get() the bare pointer, a void*,
  // cast it to tuple<...>*,
  // then dereference that pointer.
  LOG(DEBUG) << "As retrieved:";
  get<1>(r53).mPrintf(fs);

  return;
}
}
// end of namespace MDemo
// -------------------------------------------------


int main(int ac, char **av) {
  // Set logging configuration from a file
  el::Configurations confFromFile("./kmodel-logger.conf");
  el::Loggers::reconfigureAllLoggers(confFromFile);
  
  using KBase::dSeed;

  auto sTime = KBase::displayProgramStart();
  uint64_t seed = dSeed;
  bool run = true;
  bool pceP = false;
  bool spvsrP = false;
  bool sqlP = false;
  bool emodP = false;
  bool tx2P = false;
  bool miP = false;
  bool cpP = true;
  bool helpP = true;
  bool csvSMP = false;
  string inputXML = "";
  string inputCSVSMP = "";
  string inputCSVPMat = "";

  auto showHelp = []() {
    printf("\n");
    printf("Usage: specify one or more of these options\n");
    printf("--help            print this message\n");
    printf("--csvSMP  <file>  demo minicsv library on SMP data file \n");
    printf("--pce             simple PCE\n");
    printf("--mi              markov incentives PCE\n");
    printf("--emod  (si|cp)   simple enumerated model, starting at self-interested or central position \n");
    //printf("--fit             fit weights \n"); // now in pmatrix demo
    printf("--spvsr           demonstrated shared_ptr<void> return\n");
    printf("--sql             demo SQLite \n");
    printf("--tx2  <file>     demo TinyXML2 library \n"); // e.g. dummyData_3Dim.xml
    printf("--seed <n>        set a 64bit seed \n");
    printf("                  0 means truly random\n");
    printf("                  default: %020llu \n", dSeed);
  };

  // tmp args

  if (ac > 1) {
    for (int i = 1; i < ac; i++) {
      LOG(DEBUG) << "Argument" << i << "is -|" << av[i] << "|-";
      if (strcmp(av[i], "--seed") == 0) {
        i++;
        seed = std::stoull(av[i]);
      }
      else if (strcmp(av[i], "--csvSMP") == 0) {
        csvSMP = true;
        i++;
        inputCSVSMP = av[i];
      }
      else if (strcmp(av[i], "--pce") == 0) {
        pceP = true;
      }
      else if (strcmp(av[i], "--spvsr") == 0) {
        spvsrP = true;
      }
      else if (strcmp(av[i], "--mi") == 0) {
        miP = true;
      }
      else if (strcmp(av[i], "--tx2") == 0) {
        tx2P = true;
        i++;
        inputXML = av[i];
      }
      else if (strcmp(av[i], "--emod") == 0) {
        emodP = true;
        i++;
        assert(i < ac);
        cpP = (strcmp(av[i], "cp") == 0);
      }
      else if (strcmp(av[i], "--sql") == 0) {
        sqlP = true;
      }
      else if (strcmp(av[i], "--help") == 0) {
        helpP = true;
        run = false;
      }
      else {
        run = false;
        printf("Unrecognized argument %s\n", av[i]);
      }
    }
  }
  else {
    run = false;
  }

  if (!run) {
    showHelp();
    return 0;
  }

  PRNG * rng = new PRNG();
  seed = rng->setSeed(seed); // 0 == get a random number
  LOG(DEBUG) << KBase::getFormattedString("Using PRNG seed:  %020llu", seed);
  LOG(DEBUG) << KBase::getFormattedString("Same seed in hex:   0x%016llX", seed);


  // note that we reset the seed every time, so that in case something
  // goes wrong, we need not scroll back too far to find the
  // seed required to reproduce the bug.
  if (pceP) {
    LOG(DEBUG) << "-----------------------------------";
    MDemo::demoPCE(seed, rng);
  }
  if (spvsrP) {
    LOG(DEBUG) << "-----------------------------------";
    MDemo::demoSpVSR(seed, rng);
  }
  if (csvSMP) {
    LOG(DEBUG) << "-----------------------------------";
    MDemo::demoMiniCSV(inputCSVSMP);
  }

  if (miP) {
    unsigned int vNum = rng->uniform() % 3;
    auto vpm = VPModel::Linear;
    switch (vNum) {
    case 0:
      vpm = VPModel::Linear;
      break;
    case 1:
      vpm = VPModel::Square;
      break;
    case 2:
      vpm = VPModel::Quartic;
      break;
    }
    unsigned int n = 16;
    auto coalitions = KMatrix(n, n);
    for (unsigned int i = 0; i < n; i++) {
      for (unsigned int j = 0; j < n; j++) {
        double cij = (i == j) ? 0.0 : rng->uniform(1.0, 10.0);
        coalitions(i, j) = cij * cij;
      }
    }
    auto pDist = Model::markovIncentivePCE(coalitions, vpm);
    LOG(DEBUG) << "Markov Incentive probabiities with " << vpm;
    trans(pDist).mPrintf(" %.4f");
  }


  if (emodP) {
    LOG(DEBUG) << "-----------------------------------";
    MDemo::demoEMod(seed);
  }

  if (sqlP) {
    LOG(DEBUG) << "-----------------------------------";
    Model::demoSQLite();
    MDemo::demoDBObject();
  }

  if (tx2P) {
    LOG(DEBUG) << "-----------------------------------";
    TXDemo::demoTX2(inputXML);
  }

  LOG(DEBUG) << "-----------------------------------";

  delete rng;
  KBase::displayProgramEnd(sTime);
  return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
