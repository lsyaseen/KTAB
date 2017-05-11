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
// Define the functions and methods unique to the Priority case.
// -------------------------------------------------

#include "kutils.h"
#include "reformpriorities.h"
#include "rplib2.h"
#include <tuple>
#include <easylogging++.h>

INITIALIZE_EASYLOGGINGPP

namespace RfrmPri {
// namespace to hold everything related to the
// "priority of reforms" CDMP. Note that KBase has no access.

using KBase::KMatrix;
using KBase::PRNG;
using KBase::VUI;
using KBase::printVUI;
using std::get;

// -------------------------------------------------
// function definitions

// return the list of the most self-interested position of each actor,
// with the CP last.
// As a side-affect, set each actor's min/max permutation values so as to
// compute normalized utilities later.
vector<VUI> scanAllPossiblePositions(const RPModel * rpm) {
  unsigned int numA = rpm->numAct;
  unsigned int numRefItem = rpm->numItm;
  assert(numRefItem == rpm->numCat);

  LOG(DEBUG) << "There are" << numA << "actors and" << numRefItem << "reform items";



  KMatrix aCap = KMatrix(1, numA);
  for (unsigned int i = 0; i < numA; i++) {
    auto ri = ((const RPActor *)(rpm->actrs[i]));
    aCap(0, i) = ri->sCap;
  }
  LOG(DEBUG) << "Actor capabilities: ";
  aCap.mPrintf(" %.2f ");


  LOG(DEBUG) << "Effective gov cost of items:";
  (rpm->govCost).mPrintf("%.3f ");
  LOG(DEBUG) << "Government budget: " << rpm->govBudget;
  assert(0 < rpm->govBudget);


  string log("Value to actors (rows) of individual reform items (columns):");
  for (unsigned int i = 0; i < rpm->actrs.size(); i++) {
    auto rai = ((const RPActor*)(rpm->actrs[i]));
    for (unsigned int j = 0; j < numRefItem; j++) {
      double vij = rai->riVals[j];
      log += KBase::getFormattedString(" %6.2f", vij);
    }
  }
  LOG(DEBUG) << log;

  LOG(DEBUG) << "Computing positions ... ";
  vector<VUI> allPositions; // list of all possiblepositions
  VUI pstn;
  // build the first permutation: 0,1,2,3,...
  for (unsigned int i = 0; i < numRefItem; i++) {
    pstn.push_back(i);
  }
  allPositions.push_back(pstn);
  while (next_permutation(pstn.begin(), pstn.end())) {
    allPositions.push_back(pstn);
  }
  const unsigned int numPos = allPositions.size();
  LOG(DEBUG) << "For" << numRefItem << "reform items there are"
   << numPos << "positions";


  // -------------------------------------------------
  // The next section sets up actor utilities.
  // First, we compute the unnormalized, raw utilities. The 'utilActorPos' checks
  // to see if pvMin/pvMax have been set, and returns the raw scores if not.
  // Then we scan across rows to find that actor's pvMin/pvMax, and record that
  // so utilActorPos can use it in the future. Finally, we normalize the rows and
  // display the normalized utility matrix.
  auto ruFn = [allPositions, rpm](unsigned int ai, unsigned int pj) {
    auto pstn = allPositions[pj];
    double uip = rpm->utilActorPos(ai, pstn);
    return uip;
  };

  LOG(DEBUG) << "Computing utilities of positions ... ";
  // rows are actors, columns are all possible positions
  auto rawUij = KMatrix::map(ruFn, numA, numPos);

  // set the min/max for each actor
  for (unsigned int i = 0; i < numA; i++) {
    double pvMin = rawUij(i, 0);
    double pvMax = rawUij(i, 0);
    for (unsigned int j = 0; j < numPos; j++) {
      double rij = rawUij(i, j);
      if (rij < pvMin) {
        pvMin = rij;
      }
      if (rij > pvMax) {
        pvMax = rij;
      }
    }
    assert(0 <= pvMin);
    assert(pvMin < pvMax);
    auto ai = ((RPActor*)(rpm->actrs[i]));
    ai->posValMin = pvMin;
    ai->posValMax = pvMax;
  }
  LOG(DEBUG) << "Normalizing utilities of positions ... ";
  KMatrix uij = KBase::rescaleRows(rawUij, 0.0, 1.0); // von Neumann utility scale

  string utilMtx("Complete (normalized) utility matrix of all possible positions (rows) versus actors (columns) \n");
  for (unsigned int pj = 0; pj < numPos; pj++) {
    utilMtx += KBase::getFormattedString("%3u  ", pj);
    auto pstn = allPositions[pj];
    //printVUI(pstn);
    utilMtx += KBase::stringVUI(pstn);
    utilMtx += "  ";
    for (unsigned int ai = 0; ai < numA; ai++) {
      double uap = uij(ai, pj);
      utilMtx += KBase::getFormattedString("%6.4f, ", uap);
    }
    utilMtx += KBase::getFormattedString("\n");
  }
  LOG(DEBUG) << utilMtx;

  // -------------------------------------------------
  // The next section determines the most self-interested positions for each actor,
  // as well as the 'central position' over all possible reform priorities
  // (which 'office seeking politicans' would adopt IF proportional voting).
  LOG(DEBUG) << "Computing best position for each actor";
  vector<VUI> bestAP; // list of each actor's best position (followed by CP)
  for (unsigned int ai = 0; ai < numA; ai++) {
    unsigned int bestJ = 0;
    double bestV = 0;
    for (unsigned int pj = 0; pj < numPos; pj++) {
      if (bestV < uij(ai, pj)) {
        bestJ = pj;
        bestV = uij(ai, pj);
      }
    }
    string bestMtx("Best position for ");
    string ais = std::to_string(ai);
    //string bjs = std::to_string(bestJ);
    string ps = KBase::stringVUI(allPositions[bestJ]);
    bestMtx += ais + " is " + ps;
    LOG(DEBUG) << bestMtx; 
    //LOG(DEBUG) << "Best for" << ai << "is ";
    //printVUI(positions[bestJ]);
    bestAP.push_back(allPositions[bestJ]);
  }


  LOG(DEBUG) << "Computing zeta ... ";
  KMatrix zeta = aCap * uij;
  assert((1 == zeta.numR()) && (numPos == zeta.numC()));


  LOG(DEBUG) << "Sorting positions from most to least net support ...";

  auto betterPR = [](tuple<unsigned int, double, VUI> pr1,
      tuple<unsigned int, double, VUI> pr2) {
    double v1 = get<1>(pr1);
    double v2 = get<1>(pr2);
    bool better = (v1 > v2);
    return better;
  };

  auto pairs = vector<tuple<unsigned int, double, VUI>>();
  for (unsigned int i = 0; i < numPos; i++) {
    auto pri = tuple<unsigned int, double, VUI>(i, zeta(0, i), allPositions[i]);
    pairs.push_back(pri);
  }

  sort(pairs.begin(), pairs.end(), betterPR);

  const unsigned int maxDisplayed = 720; // factorial(6)
  unsigned int  numPr = (pairs.size() < maxDisplayed) ? pairs.size() : maxDisplayed;

  LOG(DEBUG) << "Displaying highest" << numPr;
  for (unsigned int i = 0; i < numPr; i++) {
    auto pri = pairs[i];
    unsigned int ni = get<0>(pri);
    double zi = get<1>(pri);
    VUI pi = get<2>(pri);
    string ps = KBase::stringVUI(pi);
    LOG(DEBUG) << KBase::getFormattedString(" %3u: %4u  %.2f  %s", i, ni, zi, ps.c_str());
    //printVUI(pi);
  }

  VUI bestPerm = get<2>(pairs[0]);

  bestAP.push_back(bestPerm);
  return bestAP;
}

}
// end of namespace

// -------------------------------------------------

namespace RfrmPri2 {
using KBase::KMatrix;
using KBase::PRNG;
using KBase::sum;

void rp2Creation(uint64_t sd) {

  // primarily test instantiation of templates
  LOG(DEBUG) << "Create RP2Model";
  auto pmm = pmmCreation(sd);
  LOG(DEBUG) << "Create RP2Pos";
  auto pmp = pmpCreation(pmm);
  LOG(DEBUG) << "Create RP2State";
  auto pms = pmsCreation(pmm);


  LOG(DEBUG) << "Delete RP2Pos";
  delete pmp;
  pmp = nullptr;
  LOG(DEBUG) << "Delete RP2Model";
  delete pmm;
  pmm = nullptr;

  //Note that deleting pmm deletes pms
  pms = nullptr;

  LOG(DEBUG) << "Done deleting.";

  initScen(sd);
  return;
}

}
// end of namespace


// -------------------------------------------------

int main(int ac, char **av) {
  el::Configurations confFromFile("./rpdemo-logger.conf");
  el::Loggers::reconfigureAllLoggers(confFromFile);

  using KBase::ReportingLevel;
  using KBase::PRNG;
  using KBase::MtchPstn;
  using KBase::dSeed;
  using RfrmPri::RPModel;
  using RfrmPri::RPState;
  using RfrmPri::RPActor;
  //using RfrmPri::printPerm;

  auto sTime = KBase::displayProgramStart(RfrmPri::appName, RfrmPri::appVersion);
  uint64_t seed = dSeed; // arbitrary;
  bool siP = true;
  bool cpP = false;
  bool runP = true;
  unsigned int sNum = 1;
  bool xmlP = false;
  bool rp2P = false;
  std::string inputXML = "";

  auto showHelp = [sNum]() {
    printf("\n");
    printf("Usage: specify one or more of these options\n");
    printf("\n");
    printf("--cp              start all actors from the central position \n");
    printf("--rp2             create RP2 objects \n");
    printf("--si              start each actor from their most self-interested position \n");
    printf("                  If neither si nor cp are specified, it will use si. \n");
    printf("                  If both si and cp are specified, it will use second specified. \n");
    printf("--help            print this message and exit \n");
    printf("--seed <n>        set a 64bit seed \n");
    printf("                  0 means truly random \n");
    printf("                  default: %020llu \n", dSeed);
    printf("--sNum <n>        choose a scenario number \n");
    printf("                  default: %u \n", sNum);
    printf("                  0: random example, potentially VERY large \n");
    printf("                  1: hard-coded example, moderate size \n");
    printf("--xml <f>         read XML scenario from a given file \n");
    printf("\n");
    printf("For example, rpdemo --si  --xml rpdata.xml, would read the file rpdata.xml \n");
    printf("and start all actors from self-interested positions.\n");
    printf("\n");
    printf("If both scenario number and XML file are specified, it will use only the XML.\n");
    printf("\n");
    printf("If neither scenario number nor XML file are specified, \n");
    printf("it will run a hard-coded example, as if --sNum 1 had been specified.\n");
    printf("\n");
  };

  // a list of <keyword, description, λ-fn>
  // might be enough to do this - except for the arguments to options.
  if (ac > 1) {
    for (int i = 1; i < ac; i++) {
      if (strcmp(av[i], "--seed") == 0) {
        i++;
        seed = std::stoull(av[i]);
      }
      else if (strcmp(av[i], "--sNum") == 0) {
        i++;
        sNum = atoi(av[i]);
      }
      else if (strcmp(av[i], "--xml") == 0) {
        xmlP = true;
        i++;
        inputXML = av[i];
      }
      else if (strcmp(av[i], "--rp2") == 0) {
        rp2P = true;
      }
      else if (strcmp(av[i], "--si") == 0) {
        cpP = false;
        siP = true;
      }
      else if (strcmp(av[i], "--cp") == 0) {
        siP = false;
        cpP = true;
      }
      else if (strcmp(av[i], "--help") == 0) {
        runP = false;
      }
      else {
        runP = false;
        printf("Unrecognized argument: %s\n", av[i]);
      }
    }
  }
  else {
    runP = false;
  }

  if (!runP) {
    showHelp();
    return 0;
  }


  if (0 == seed) {
    PRNG * rng = new PRNG();
    seed = rng->setSeed(0); // 0 == get a random number
    delete rng;
    rng = nullptr;
  }
  LOG(DEBUG) << KBase::getFormattedString("Using PRNG seed:  %020llu", seed);
  LOG(DEBUG) << KBase::getFormattedString("Same seed in hex:   0x%016llX", seed);
  // Unix correctly prints all digits with lu, lX, llu, and llX.
  // Windows only prints part, with lu, lX, llu, and llX.


  const bool parP = KBase::testMultiThreadSQLite(false, KBase::ReportingLevel::Medium);  
    if (parP) {
      LOG(DEBUG) << "Can continue with multi-threaded execution";
    }
    else {
      LOG(DEBUG) << "Must continue with single-threaded execution";
    }

  if (rp2P) {
    RfrmPri2::rp2Creation(seed);
    return 0;
  }

  auto rpm = new RPModel("", seed);
  if (xmlP) {
    rpm->readXML(inputXML);
    LOG(DEBUG) << "done reading XML";
  }
  else {
    switch (sNum) {
    case 0:
    case 1:
      rpm->initScen(sNum);
      break;

    default:
      LOG(DEBUG) << "Unrecognized scenario number" << sNum;
      assert(false);
      break;
    }
  }

  unsigned int numA = rpm->numAct; // the actors are as yet unnamed
  unsigned int numR = rpm->numItm;
  //unsigned int numC = rpm->numCat;

  auto rps0 = new RPState(rpm);
  rpm->addState(rps0);

  // NOTE WELL: Records each actor's most self-interested position, but does not set them.
  // Further, it appends Central Position after the last actor position
  auto siPstns = RfrmPri::scanAllPossiblePositions(rpm);
  assert(numA + 1 == siPstns.size());
  const KBase::VUI bestPerm = siPstns[numA];
  assert(numR == bestPerm.size());
  assert(numA == rps0->pstns.size()); // pre-allocated by constructor, all nullptr's

  // Move them all to either the CP or to SI positions which
  // maximize their direct utility, regardless of expected utility.
  for (unsigned int i = 0; i < numA; i++) {
    auto pi = new  MtchPstn();
    pi->numCat = numR;
    pi->numItm = numR;
    if (cpP) {
      pi->match = bestPerm;
    }
    else {
      pi->match = siPstns[i];
    }
    rps0->pstns[i] = pi;
  }
  assert(numA == rps0->pstns.size());

  rps0->step = [rps0]() {
    return rps0->stepSUSN();
  };
  unsigned int maxIter = 100;
  rpm->stop = [maxIter, rpm](unsigned int iter, const KBase::State * s) {
    bool doneP = iter > maxIter;
    if (doneP) {
      LOG(DEBUG) << "Max iteration limit of" << maxIter << "exceeded";
    }
    auto s2 = ((const RPState *)(rpm->history[iter]));
    for (unsigned int i = 0; i < iter; i++) {
      auto s1 = ((const RPState *)(rpm->history[i]));
      if (RPModel::equivStates(s1, s2)) {
        doneP = true;
        LOG(DEBUG) << "State number" << iter << "matched state number" << i;
      }
    }

    return doneP;
  };

  rps0->setUENdx();
  rpm->run();

  // we already displayed each state as it was processed,
  // so there is no need to show it again
  //rpm->showHist();


  delete rpm; // and actors, and states
  rpm = nullptr;
  rps0 = nullptr;

  KBase::displayProgramEnd(sTime);

  return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
