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

#include <cstdio>
#include <string>
#include <vector>

#include "kutils.h"
#include "kmatrix.h"
#include "prng.h"
#include "kmodel.h"

#include "smp.h"
#include "comsel.h"
#include "democomsel.h"


using namespace std;
using KBase::VUI;
using KBase::PRNG;
using KBase::KMatrix;
using KBase::Actor;
using KBase::Model;
using KBase::Position;
using KBase::State;
using KBase::VotingRule;


namespace DemoComSel {

  using std::function;
  using std::get;
  using std::string;

  using KBase::ReportingLevel;
  using KBase::MtchPstn;

  using ComSelLib::CSModel;
  using ComSelLib::CSActor;
  using ComSelLib::CSState;

  using ComSelLib::intToVB;
  using ComSelLib::vbToInt;

  // -------------------------------------------------
  // like printVUI, but with range-checking and boolean output
  string printCS(const VUI& v) {
    unsigned int n = v.size();
    string cs("[CS ");
    for (unsigned int i = 0; i < n; i++) {
      switch (v[i]) {
      case 0:
        cs += "-";
        break;
      case 1:
        cs += "+";
        break;
      default:
        LOG(INFO) << "printCS:: unrecognized case";
        exit(-1);
        break;
      }
    }
    cs += "]";
    return cs;
  }

  void demoActorUtils(const uint64_t s, PRNG* rng) {
    LOG(INFO) << KBase::getFormattedString("Using PRNG seed: %020llu", s);
    rng->setSeed(s);
    return;
  }


  void demoCSC(unsigned int numA, unsigned int nDim, 
               bool cpP, bool siP, const uint64_t s) {
    // need a template for ...
    // position type  (each is a committee)
    // utility to an actor of a position (committee)
    // getting actor's scalar capability

    LOG(INFO) << KBase::getFormattedString("Using PRNG seed: %020llu", s);
    auto trng = new PRNG();
    trng->setSeed(s);

    if (0 == numA) { numA = 2 + (trng->uniform() % 4); } // i.e. [2,6] inclusive 
    if (0 == nDim) { nDim = 1 + (trng->uniform() % 7); } // i.e. [1,7] inclusive 

    LOG(INFO) << "Num parties:" << numA;
    LOG(INFO) << "Num dimensions:" << nDim;


    unsigned int numItm = numA;
    unsigned int numCat = 2; // out or in, respectively
    unsigned int numPos = exp2(numA); // i.e. numCat ^^ numItm
    vector<VUI> positions = {};
    for (unsigned int i = 0; i < numPos; i++) {
      const VUI vbi = intToVB(i, numA);
      const unsigned int j = vbToInt(vbi);
      assert(j == i);
      const VUI vbj = intToVB(j, numA);
      assert(vbj == vbi);
      positions.push_back(vbi);
    }
    assert(numPos == positions.size());

    LOG(INFO) << "Num positions:" << numPos;

    auto ndfn = [](string ns, unsigned int i) {
      auto ali = KBase::newChars(10 + ((unsigned int)(ns.length())));
      std::sprintf(ali, "%s%i", ns.c_str(), i);
      auto s = string(ali);
      delete ali;
      ali = nullptr;
      return s;
    };

    // create a model to hold some random actors
    auto csm = new CSModel(nDim, "csm0", s);

    LOG(INFO) << "Configuring actors: randomizing";
    for (unsigned int i = 0; i < numA; i++) {
      auto ai = new CSActor(ndfn("csa-", i), ndfn("csaDesc-", i), csm);
      ai->randomize(csm->rng, nDim);
      csm->addActor(ai);
    }
    assert(csm->numAct == numA);
    csm->numItm = numA;
    assert(csm->numCat == 2);

    if ((9 == numA) && (2 == nDim)) {
      for (unsigned int i = 0; i < 3; i++) {
        auto ci = KMatrix::uniform(csm->rng, nDim, 1, 0.1, 0.9);
        for (unsigned int j = 0; j < 3; j++) {
          auto ej = KMatrix::uniform(csm->rng, nDim, 1, -0.05, +0.05);
          const KMatrix pij = clip(ci + ej, 0.0, 1.0);
          unsigned int n = (3 * i) + j;
          auto an = ((CSActor *)(csm->actrs[n]));
          an->vPos = VctrPstn(pij);
        }
      }
    }

    LOG(INFO) << "Scalar positions of actors (fixed) ...";
    for (auto a : csm->actrs) {
      auto csa = ((CSActor*)a);
      LOG(INFO) << csa->name << "v-position:";
      trans(csa->vPos).mPrintf(" %5.2f");
      LOG(INFO) << a->name << "v-salience:";
      trans(csa->vSal).mPrintf(" %5.2f");
    }

    LOG(INFO) << "Getting scalar strength of actors ...";
    KMatrix aCap = KMatrix(1, csm->numAct);
    for (unsigned int i = 0; i < csm->numAct; i++) {
      auto ai = ((const CSActor *)(csm->actrs[i]));
      aCap(0, i) = ai->sCap;
    }

    aCap = (100.0 / sum(aCap)) * aCap;
    LOG(INFO) << "Scalar strengths:";
    for (unsigned int i = 0; i < csm->numAct; i++) {
      auto ai = ((CSActor *)(csm->actrs[i]));
      ai->sCap = aCap(0, i);
      LOG(INFO) << KBase::getFormattedString("%3i  %6.2f", i, ai->sCap);
    }

    LOG(INFO) << "Computing utilities of positions ... ";
    for (unsigned int i = 0; i < numA; i++) {
      // (0,0) causes computation of entire table
      double uii = csm->getActorCSPstnUtil(i, i); 
    }

    // At this point, we have almost a generic enumerated model,
    // so much of the code below should be easily adaptable to EModel.

    // rows are actors, columns are all possible position
    KMatrix uij = KMatrix(numA, numPos);  
    LOG(INFO) << "Complete (normalized) utility matrix of all possible positions (rows)"
      <<" versus actors (columns)";
    string utilMtx;
    for (unsigned int pj = 0; pj < numPos; pj++) {
      utilMtx += std::to_string(pj) + "  ";
      auto pstn = positions[pj];
      utilMtx += printCS(pstn) + "  ";
      for (unsigned int ai = 0; ai < numA; ai++) {
        const double uap = csm->getActorCSPstnUtil(ai, pj);
        uij(ai, pj) = uap;
        utilMtx += KBase::getFormattedString("%6.4f, ", uap);
      }
    }

    LOG(INFO) << utilMtx;

    LOG(INFO) << "Computing best position for each actor";
    vector<VUI> bestAP = {}; // list of each actor's best position (followed by CP)
    for (unsigned int ai = 0; ai < numA; ai++) {
      unsigned int bestJ = 0;
      double bestV = 0;
      for (unsigned int pj = 0; pj < numPos; pj++) {
        if (bestV < uij(ai, pj)) {
          bestJ = pj;
          bestV = uij(ai, pj);
        }
      }
      LOG(INFO) << "Best for" << ai << "is" << bestJ << " " << printCS(positions[bestJ]);
      bestAP.push_back(positions[bestJ]);
    }

    trans(aCap).mPrintf("%5.2f ");

    
    // which happens to indicate the PCW *if* proportional voting,
    // when we actually use PropBin
    LOG(INFO) << "Computing zeta ... "; 
    KMatrix zeta = aCap * uij;
    assert((1 == zeta.numR()) && (numPos == zeta.numC()));

    LOG(INFO) << "Sorting positions from most to least net support ...";
    auto betterPR = [](tuple<unsigned int, double, VUI> pr1,
      tuple<unsigned int, double, VUI> pr2) {
      double v1 = get<1>(pr1);
      double v2 = get<1>(pr2);
      bool better = (v1 > v2);
      return better;
    };

    auto pairs = vector<tuple<unsigned int, double, VUI>>();
    for (unsigned int i = 0; i < numPos; i++) {
      auto pri = tuple<unsigned int, double, VUI>(i, zeta(0, i), positions[i]);
      pairs.push_back(pri);
    }

    sort(pairs.begin(), pairs.end(), betterPR);

    const unsigned int maxDisplayed = 256;
    unsigned int  numPr = (pairs.size() < maxDisplayed) ? pairs.size() : maxDisplayed;

    LOG(INFO) << "Displaying highest " << numPr;
    for (unsigned int i = 0; i < numPr; i++) {
      auto pri = pairs[i];
      unsigned int ni = get<0>(pri);
      double zi = get<1>(pri);
      VUI pi = get<2>(pri);

      LOG(INFO) << KBase::getFormattedString(" %3u: %4u  %7.2f  ", i, ni, zi)
        << printCS(pi);
    }

    VUI bestCS = get<2>(pairs[0]);

    bestAP.push_back(bestCS); // last one is the CP


    auto css0 = new CSState(csm);
    csm->addState(css0);
    assert(numA == css0->pstns.size()); // pre-allocated by constructor, all nullptr's
    // Either start them all at the CP or have each choose an initial position which
    // maximizes their direct utility, regardless of expected utility.
    for (unsigned int i = 0; i < numA; i++) {
      auto pi = new  MtchPstn();
      pi->numCat = numCat;
      pi->numItm = numItm;
      if (cpP) {
        pi->match = bestCS;
      }
      if (siP) {
        pi->match = bestAP[i];
      }
      css0->pstns[i] = pi;

      assert(numA == css0->pstns.size()); // must be invariant
    }
    

    css0->step = [css0]() {
      return css0->stepSUSN();
    };

    unsigned int maxIter = 1000;
    csm->stop = [maxIter, csm](unsigned int iter, const KBase::State * s) {
      bool doneP = iter > maxIter;
      if (doneP) {
        LOG(INFO) << "Max iteration limit of" << maxIter << "exceeded";
      }
      auto s2 = ((const CSState *)(csm->history[iter]));
      for (unsigned int i = 0; i < iter; i++) {
        auto s1 = ((const CSState *)(csm->history[i]));
        if (CSModel::equivStates(s1, s2)) {
          doneP = true;
          LOG(INFO) << "State number" << iter << "matched state number" << i;
        }
      }

      return doneP;
    };

    css0->setUENdx();


    csm->run();

    return;
  }

  
} // end of namespace


int main(int ac, char **av) {
  using std::string;
  using KBase::dSeed;
  
  el::Configurations confFromFile("./comsel-logger.conf");
  el::Loggers::reconfigureAllLoggers(confFromFile);
  
  auto sTime = KBase::displayProgramStart(DemoComSel::appName, DemoComSel::appVersion);
  uint64_t seed = dSeed; // arbitrary
  bool run = true;
   bool cpP = false;
  bool siP = true;

  auto showHelp = []() {
    printf("\n");
    printf("Usage: specify one or more of these options\n");
    printf("--cp        start each actor from the central position \n");
    printf("--si        start each actor from their most self-interested position \n");
    printf("            If neither cp nor si are specified, it will use si \n");
    printf("            If both cp and si are specified, it will use the second specified \n");
    printf("--help      print this message \n");
    printf("--seed <n>  set a 64bit seed \n");
    printf("            0 means truly random \n");
    printf("            default: %020llu \n", dSeed);
  };

  // tmp args
  // seed = 0;

  if (ac > 1) {
    for (int i = 1; i < ac; i++) {
      if (strcmp(av[i], "--seed") == 0) {
        i++;
        seed = std::stoull(av[i]);
      }
      else if (strcmp(av[i], "--cp") == 0) {
        siP = false;
        cpP = true;
      }
      else if (strcmp(av[i], "--si") == 0) {
        cpP = false;
        siP = true;
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

  if (0 == seed) {
    PRNG * rng = new PRNG();
    seed = rng->setSeed(0); // 0 == get a random number
    delete rng;
    rng = nullptr;
  }
  LOG(INFO) << KBase::getFormattedString("Using PRNG seed: %020llu", seed);
  LOG(INFO) << KBase::getFormattedString("Same seed in hex: 0x%016llX", seed);


  LOG(INFO) << "Creating objects from SMPLib ... ";
  auto sm = new SMPLib::SMPModel("", seed); // , "SMPScen-010101"
  auto sa = new SMPLib::SMPActor("Bob", "generic spatial actor");
  delete sm;
  sm = nullptr;
  delete sa;
  sa = nullptr;
  LOG(INFO) << "Done creating objects from SMPLib.";

  
    // note that we reset the seed every time, so that in case something
    // goes wrong, we need not scroll back too far to find the
    // seed required to reproduce the bug.
    DemoComSel::demoCSC(9, // actors trying to get onto committee
      2, // issues to be addressed by the committee
      cpP, siP,
      seed);
  KBase::displayProgramEnd(sTime);
  return 0;
}

// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
